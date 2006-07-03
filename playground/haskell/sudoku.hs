{-
Sudoku Solver in Haskell98
Includes a random board creation facility, a debug printout
and a solver.
-}

{-
DONE: creation, display, verification, obscurification
TODO: initial permutation, solver
-}

-- board structure:
-- b = [c c c], c = [b b b]^T, b = [l l l]^T

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

-- Liefert alle möglichen Rotationen
allperms_rec :: [a] -> [a] -> ([a] -> [a]) -> [[a]]
allperms_rec f [] fun = []
allperms_rec f (x:xs) fun = allperms_rec p xs fun ++ [p]
                            where p = fun f
--allperms :: [a] -> ([a] -> [a]) -> [[a]]
allperms f fun = allperms_rec f f fun

-- Rotiert die Zahlen in einer Zeile
permute :: [a] -> [a]
permute (x:xs) = xs ++ [x]

-- Rotiert die Spalten in einem Block
blockcolumnpermute :: [[a]] -> [[a]]
blockcolumnpermute [] = []
blockcolumnpermute (x:xs) = [permute x] ++ blockcolumnpermute xs

-- Rotiert die Zeilen in einem Block ([[a]] -> [[a]])
blocklinepermute = permute

-- Erstellt eine (vertikale?) Reihe aus n Blöcken
buildline :: [[a]] -> [[[a]]]
buildline block = allperms block blockcolumnpermute

-- Erzeugung eines einzigen Blocks mit n Zeilen und Spalten
createblock :: Integer -> [[Integer]]
createblock n | n > 0 = [[b .. b + n - 1] | b <- [b | b <- [1, n + 1 .. n * n]]]
              | n <= 0 = [[]]

-- Erstellung des Spielbretts aus m (vertikalen?) Reihen zu je n Blöcken
createboard :: Integer -> [[[[Integer]]]]
createboard n = map buildline (allperms block blocklinepermute)
                where block = createblock n

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

int2char :: Integer -> Char
int2char x = case x of
 1 -> '1'
 2 -> '2'
 3 -> '3'
 4 -> '4'
 5 -> '5'
 6 -> '6'
 7 -> '7'
 8 -> '8'
 9 -> '9'
 _ -> '?'

-- displays a single row of a block, i.e. [x,x,x]
displayrow :: [Integer] -> IO()
displayrow [] = putChar ' '
displayrow (x:xs) = do putChar (int2char x)
                       displayrow xs

-- displays a line of [[x,x,x], [y,y,y], ...]
displayline :: [[Integer]] -> IO()
displayline [] = putChar '\n'
displayline (x:xs) = do displayrow x
                        displayline xs

-- picks the n-th element out of a list
pick :: [a] -> Int -> a
--pick [] _ = 0 -- error
pick (x:xs) 0 = x
pick (x:xs) n = pick xs (n - 1)

-- from a list of lists, create a list of the n-th element of each of those
orthos :: [[a]] -> Int -> [a]
orthos [] _ = []
orthos (x:xs) n = [pick x n] ++ orthos xs n

-- displays one line with blocks, i.e. [b b b]
displayblockrow :: [[[Integer]]] -> IO()
displayblockrow x = do displayline (orthos x 0)
                       displayline (orthos x 1)
                       displayline (orthos x 2)
                       putChar '\n'

-- displays a whole board
displayboard :: [[[[Integer]]]] -> IO()
displayboard x = do displayblockrow (orthos x 0)
                    displayblockrow (orthos x 1)
                    displayblockrow (orthos x 2)

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

-- convert a list [[x,x,x], [y,y,y], ...] to [x,x,x,y,y,y,...]
flatten :: [[a]] -> [a]
flatten [] = []
flatten (x:xs) = x ++ flatten xs

-- list of all possible orthos
allorthos_rec :: [[a]] -> Int -> [[a]]
allorthos_rec x (-1) = []
allorthos_rec x n = [orthos x n] ++ allorthos_rec x (n - 1)
--allorthos :: [[a]] -> [[a]]
allorthos x = allorthos_rec x (length x - 1)

-- list of all possible orthos of each element of a list
allorthosb :: [[[a]]] -> [[a]]
allorthosb [] = []
allorthosb (x:xs) = flatten (allorthos x) : allorthosb xs

allorthos2 :: [[[[a]]]] -> [[[a]]]
allorthos2 [] = []
allorthos2 (x:xs) = allorthosb x : allorthos2 xs

flatten_inner :: [[[a]]] -> [[a]]
flatten_inner [] = []
flatten_inner (x:xs) = flatten x : flatten_inner xs

allorthos3 :: [[[[a]]]] -> [[[a]]]
allorthos3 [] = []
allorthos3 (x:xs) = flatten_inner (allorthos x) : allorthos3 xs

flatboard :: [[[[a]]]] -> [a]
flatboard board = flatten (flatten (allorthos3 (allorthos board)))

-- returns how often an element is in the list, 0 if not found
contains :: [Integer] -> Integer -> Int
contains x e | e /= 0 = length (filter (== e) x)
             | otherwise = 0

-- same, but watch for zero!! plus, different type!
contains_int :: [Int] -> Int -> Int
contains_int x e = length (filter (== e) x)

-- old variant
contains_eq :: Eq a => [a] -> a -> Int
contains_eq x e = length (filter (== e) x)

--contains [] _ = 0
--contains (x:xs) e = contains xs e + equal
--                    where equal | x == e = 1
--                                | otherwise = 0

-- validates a block, a row or a column
-- returns 0 if everything ok, >0 if error (ie. double element)
validateentity :: [Integer] -> Int
validateentity [] = 0
validateentity (x:xs) = (contains xs x) + validateentity xs

-- validates all blocks
validate :: [[Integer]] -> Int
validate [] = 0
validate (x:xs) = validateentity x + validate xs

-- returns 0 if board is valid, >0 otherwise
validation :: [[[[Integer]]]] -> Int
validation board = validate columns + validate rows + validate blocks
                   where columns = flatten (allorthos2 board)
                         rows = flatten (allorthos3 (allorthos board))
                         blocks = flatten_inner (flatten board)

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

-- +---m
-- |+y
-- |x+
-- n
--
-- set field at block column m, block (row) n, row y, column x
get_at :: Int -> Int -> [[[[a]]]] -> a
get_at a b board = pick (pick (pick (pick board m) n) y) x
                   where x = a `mod` (length board)
                         y = b `mod` (length board)
                         m = a `div` (length board)
                         n = b `div` (length board)

stick :: [a] -> Int -> a -> [a]
stick [] _ _ = []
stick (x:xs) 0 e = e : xs
stick (x:xs) n e = x : stick xs (n - 1) e

--stick [[1,1,1], [2,2,2], [3,3,3]] 1 (stick [0,0,0,0,0] 2 888)

set_at :: Int -> Int -> [[[[a]]]] -> a -> [[[[a]]]]
set_at a b board e = stick board m (stick l3 n (stick l2 y (stick l1 x e)))
                     where x = a `mod` (length board)
                           y = b `mod` (length board)
                           m = a `div` (length board)
                           n = b `div` (length board)
                           l3 = pick board m
                           l2 = pick l3 n
                           l1 = pick l2 y

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

board = createboard 3
test = displayboard board

middle = get_at 4 4 board

--hunk :: [[[[Integer]]]] -> [[[[Integer]]]]
--hunk board = set_at 4 4 board 0

notamb :: [[[[Integer]]]] -> Int -> Int -> Int
notamb board a b = contains_int (map validation (map (set_at a b board) [1..len])) 0
                   --where len = (length board) * (length board)
                   where len = 9

obscureifnotamb :: [[[[Integer]]]] -> Int -> Int -> [[[[Integer]]]]
obscureifnotamb board a b | notamb board a b == 1 = set_at a b board 0
                          | otherwise = board

obscureall :: [[[[Integer]]]] -> [(Int, Int)] -> [[[[Integer]]]]
obscureall board [] = board
obscureall board (x:xs) = obscureall (obscureifnotamb board a b) xs
                          where (a, b) = x

obscure :: [[[[Integer]]]] -> [[[[Integer]]]]
obscure board = obscureall board [(x,y) | x <- [0..8], y <- [0..8]]

test2 = obscure board
--b2 = obscureifnotamb (obscureifnotamb board 0 0) 0 1

--test2 = displayboard (hunk board)

-- /////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////

findunknown :: [Integer] -> Int -> Int -> (Int, Int)
findunknown [] _ _ = (-1, -1)
findunknown (x:xs) pos size | x == 0 = (pos `mod` size, pos `div` size)
                            | otherwise = findunknown xs (pos + 1) size

set_at_flat :: Int -> Int -> [a] -> Int -> a -> [a]
set_at_flat a b board len e = stick board (b * len + a) e

-- FIXME: too simple!
validation_flat :: [Integer] -> Int
validation_flat board = validate blocks
                        where blocks = (restructure board 9)

firstworking :: [[Integer]] -> Int -> [Integer]
firstworking [] _ = []
--firstworking (x:xs) len | b == x = b
--                        | otherwise = firstworking xs len
--                          where b = solver x len
firstworking (x:xs) len | (f == (-1, -1) && validation_flat b == 0) = b
                        | otherwise = firstworking xs len
                          where b = solver x len
                                f = findunknown b 0 len

solverpos :: Int -> Int -> [Integer] -> Int -> [Integer]
solverpos a b board len | f /= [] = f
                        | otherwise = board
                          where f = firstworking (map (set_at_flat a b board len) [1..9]) 9 --len
-- use 1..len!

solver :: [Integer] -> Int -> [Integer]
solver board len | pos == (-1, -1) = board
                 | otherwise = solverpos a b board len
                   where pos = findunknown board 0 len
                         (a, b) = pos

test3 = solver (flatboard test2) ((length board) * (length board))

restructure :: [Integer] -> Int -> [[Integer]]
restructure [] _ = []
restructure x n = (take n x) : restructure (drop n x) n

test4 = restructure test3 ((length board) * (length board))

test5 = displayline test4

