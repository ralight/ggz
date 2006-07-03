{-
-}

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

-- returns how often an element is in the list, 0 if not found
contains :: [Integer] -> Integer -> Int
contains x e | e /= 0 = length (filter (== e) x)
             | otherwise = 0

-- validates a block, a row or a column
-- returns 0 if everything ok, >0 if error (ie. double element)
validateentity :: [Integer] -> Int
validateentity [] = 0
validateentity (x:xs) = (contains xs x) + validateentity xs

-- validates all blocks
validate :: [[Integer]] -> Int
validate [] = 0
validate (x:xs) = validateentity x + validate xs

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

stick :: [a] -> Integer -> a -> [a]
stick [] _ _ = []
stick (x:xs) 0 e = e : xs
stick (x:xs) n e = x : stick xs (n - 1) e

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////
-- !! from sudoku.hs

-- picks the n-th element out of a list
pick :: [a] -> Int -> a
--pick [] _ = 0 -- error
pick (x:xs) 0 = x
pick (x:xs) n = pick xs (n - 1)

-- from a list of lists, create a list of the n-th element of each of those
orthos :: [[a]] -> Int -> [a]
orthos [] _ = []
orthos (x:xs) n = [pick x n] ++ orthos xs n

allorthos_rec :: [[a]] -> Int -> [[a]]
allorthos_rec x (-1) = []
allorthos_rec x n = [orthos x n] ++ allorthos_rec x (n - 1)
--allorthos :: [[a]] -> [[a]]
allorthos x = allorthos_rec x (length x - 1)

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

flatten :: [[a]] -> [a]
flatten [] = []
flatten (x:xs) = x ++ flatten xs

restructure :: Int -> [a] -> [[a]]
restructure _ [] = []
restructure n x = (take n x) : restructure n (drop n x)

isqrt :: Int -> Int -> Int -> Int
isqrt n w res | res == w = res
              | otherwise = isqrt n nw (n `div` nw)
                where nw = (res + w) `div` 2

isqrt_unsafe :: Int -> Int
isqrt_unsafe n = isqrt n n 0

-- !! Zeilen/Spalten
restructure_auto board = restructure (isqrt_unsafe (length board)) board

-- !! Blöcke
restructure_blocks board = map flatten (flatten (restructure_board board))

-- !! Originalboard für Display
restructure_board board = map allorthos (map (restructure size) tmp)
                          where tmp = (map (restructure size) (restructure (size*size*size) board))
                                size = isqrt_unsafe (isqrt_unsafe (length board))

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

findunknown :: [Integer] -> Integer -> Integer -> (Integer, Integer)
findunknown [] _ _ = (-1, -1)
findunknown (x:xs) pos size | x == 0 = (pos `mod` size, pos `div` size)
                            | otherwise = findunknown xs (pos + 1) size

set_at_flat :: Integer -> Integer -> [a] -> Integer -> a -> [a]
set_at_flat a b board len e = stick board (b * len + a) e

-- FIXME: too simple!
validation_flat :: [Integer] -> Int
validation_flat board = validate rows + validate columns + validate blocks
                        where rows = restructure_auto board
                              columns = allorthos (restructure_auto board)
                              blocks = restructure_blocks board

-- flatten (map allorthos (map (restructure 3) (map (restructure 3) (restructure 27 board))))
-- (blocks)

--validation_flat_small :: [Integer] -> Int
--validation_flat_small board = validate blocks
--                              where blocks = restructure_auto board

--validation_flat_pseudo :: [Integer] -> Int
--validation_flat_pseudo _ = 0

--fworking :: [[Integer]] -> Int -> Int -> [Integer]
--fworking [] _ _ = []
--fworking (x:xs) len max | (f == (-1, -1) && validation_flat b == 0) = b
--                        | otherwise = fworking xs len max
--                          where b = solver_int x len max
--                                f = findunknown b 0 len

fworkingsim :: [[Integer]] -> Integer -> Int -> [Integer]
fworkingsim [] _ _ = []
fworkingsim (x:xs) len max | (f == (-1, -1) && validation_flat b == 0) = b
                           | otherwise = fworkingsim xs len max
                             where b = solver_int x len max
                                   f = findunknown b 0 len

solverpos :: Integer -> Integer -> [Integer] -> Integer -> Int -> [Integer]
solverpos a b board len max | f /= [] = f
                            | otherwise = board -- []
                              where f = fworkingsim (map (set_at_flat a b board len) tries) len max
                                    tries = [1..len]

solver_int :: [Integer] -> Integer -> Int -> [Integer]
solver_int board len max | max == 0 = board
                         | pos == (-1, -1) = board
                         | otherwise = solverpos a b board len (max - 1)
                           where pos = findunknown board 0 len
                                 (a, b) = pos

solver_int_wrapper :: [Integer] -> Integer -> Int -> [Integer]
solver_int_wrapper board len max | pos == (-1, -1) = b
                                 | otherwise = []
                                   where pos = findunknown b 0 len
                                         b = solver_int board len max

--solver :: [Integer] -> Int -> IO() -> [Integer]
--solver board len = do putChar '*'
--                      return solver_int board len

------board = [0,9,7,5,6,4,0,3,1,0,6,4,0,3,1,8,9,7,0,3,1,0,9,7,0,6,4,0,7,8,6,4,5,0,1,2,0,4,5,0,1,2,9,7,8,0,1,2,0,7,8,0,4,5,0,8,9,4,5,6,0,2,3,0,5,6,0,2,3,7,8,9,0,2,3,0,8,9,0,5,6]
--board = [0,9,7,5,6,4,0,3,1,1,0,4,1,3,1,8,0,7,1,3,1,0,9,7,1,6,4,0,7,8,6,4,5,1,1,2,1,4,5,1,1,2,9,7,8,1,1,2,1,7,8,1,4,5,1,8,9,4,5,6,1,2,3,1,5,6,1,2,3,7,8,9,1,2,3,1,8,9,1,5,6]

--board = [0,0,0,0,0,0,3,4,1,2,3,4,1,2,3,4]
board_orig = [8,9,7,5,6,4,2,3,1,5,6,4,2,3,1,8,9,7,2,3,1,8,9,7,5,6,4,9,7,8,6,4,5,3,1,2,6,4,5,3,1,2,9,7,8,3,1,2,9,7,8,6,4,5,7,8,9,4,5,6,1,2,3,4,5,6,1,2,3,7,8,9,1,2,3,7,8,9,4,5,6]
board = [0,0,0,0,0,0,0,0,0,5,6,4,0,3,1,8,9,7,2,3,1,8,9,7,5,6,4,9,7,8,6,4,5,3,1,2,6,4,5,3,1,2,9,7,8,3,1,2,9,7,8,6,4,5,7,8,9,4,5,6,1,2,3,4,5,6,1,2,3,7,8,9,1,2,3,7,8,9,4,5,6]

-------solved = solver_int board 9 20 --21
--solved = solver_int board 9 6 -- with 5: extremely slow, 6: fast???
--solved = solver_int_wrapper board 4 6
solved = solver_int_wrapper board 9 (9*9)
--(isqrt_unsafe (length board)) 3

--data Nsum = IO Int

--xsolv :: [a] -> IO a
--xsolv x = do n <- readFile x
--             putChar '*'

--showfile file = do input <- readFile file
--                   putStrLn input
--                   putChar '*'
--                   putStrLn "ddd"

equal :: Eq a => [a] -> [a] -> Int
equal [] [] = 1
equal [] _ = 0
equal _ [] = 0
equal (x:xs) (y:ys) | x == y = equal xs ys
                    | otherwise = 0

eq = equal board solved

tuplepack [] = []
tuplepack (x:xs) = (x,x) : tuplepack xs

diff :: Eq a => [a] -> [a] -> [(a,a)]
diff [] [] = []
diff [] x = tuplepack x
diff x [] = tuplepack x
diff (x:xs) (y:ys) | x == y = diff xs ys
                   | otherwise = (x,y) : (diff xs ys)

difftest = diff board solved

