GNU Go integration will happen in multiple steps.

Step 1
======
- goclient creates 2 fifos /tmp/goclient.*
- goclient launches cgoban
- user must select program play communicating over stdin/stdout
- program is goclient.helper, communicates with goclient via fifos
- goclient thus arbiters between cgoban and the server

- goserver launches gnugo in background using gmp mode
- using redirected stdin/stdout
- arbiters between the program and the client

Step 2
======
- a custom client (vai) is written e.g. using pygame
- this client uses the gtp mode (optionally)

Step 3
======
- the server intercepts all moves
- it will thus be able to track game results and scores

Alternatives
============
- instead of using goclient, ggzwrap is used... this requires
  changes in cgoban! (but not with vai)
- why does cgoban not write to stdin/stdout directly? (wishlist)
- other gnugo clients like dingoui

