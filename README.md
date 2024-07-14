# MyFirstOnlineGame (2 players, non p2p)

# Setup:

install raylib and connect winsock.

Raylib docs: 
https://www.raylib.com/examples.html

Helpful Winsock UDP guide:
https://www.youtube.com/watch?v=uIanSvWou1M&t=1156s


Please note:


On the client project raylib and winsock have some conflicts which can be solved by refactoring some of the methods the raylib header file: (closewindow -> raylibclosewindow. drawtext-> raylibdrawtext. Rectangle -> raylibrectangle. showcursor -> raylibshowcursor. drawtextex-> raylibdrawtext. loadimage -> raylibloadimage. playsound -> raylibplaysound)

# Run Guide

Stuff is still a bit buggy if you dont run in a proper order:

1) Run the server file

2) Run the client file twice

3) in the first client (assuming server is on localhost) connect by typing in the textarea: 127.0.0.1/1/a

4) in the second client: 127.0.0.1/2/b

5) if the ball gets stuck press r to reset it

6) press escape to terminate the client program, type *stop* in the servers terminal to safely exit

   *note* To experiment with different PC's simultaniously make sure port forwarding is enabled and firewall is configured properly

   Typically to do this you go -> control panel -> firewall -> advanced and then on both inbound and outbound: -> new rule -> port -> udp -> ports 54000-54001

   learn and utilize windows firewall api to handle creating/enabling automatically

# TODOS

1) learn and utilize windows firewall api to handle creating/enabling automatically

2) automate connecting to a static proxy server instead of having to type in everything

3) create a proxy that handles connecting/disconnecting clients
