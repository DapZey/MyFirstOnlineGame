# MyFirstOnlineGame (2 players, non p2p)

# Setup:

install raylib and connect winsock.

Raylib docs: 
https://www.raylib.com/examples.html

Helpful Winsock UDP guide:
https://www.youtube.com/watch?v=uIanSvWou1M&t=1156s


Please note:


On the client project raylib and winsock have some conflicts which can be solved by refactoring the raylib header file

#Run Guide

Stuff is still a bit buggy if you dont run in a proper order:

1) Run the server file

2) Run the client file twice

3) in the first client (assuming server is on localhost) connect by typing in the textarea: 127.0.0.1/1/a

4) in the second client: 127.0.0.1/2/b

5) press escape to terminate the client program, type *stop* in the servers terminal to safely exit

# TODOS

1) implement average response time so that frequency of messages isnt only dependant on past response time *DONE*

2) move player based on vector (+1,-1) etc. instead of coordinates to have a consistant buffer size *IN PROGRESS*

3) (maybe) experiment with a player count larger with two by utilizing fd_sets and/or multiple sockets on the same port

4) add a "world environment" class to the server file and get started on the actual game loop
