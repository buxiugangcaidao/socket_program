# socket_program
a simple case about epoll/kqueue
using kqueue(macos or other freebsd kernel system) or epoll(linux) to develop a more efficient multi-user socket communication tool

still in developing 

be glad to hear any question or comment from you

## test process

compile server:
gcc usp_epoll.c -c 
gcc test_server.c usp_epoll.o -o server

compile client:
gcc test_client.c -o cilent


## connect and communicatio

run server and client seperately
and then we can input in the client first and recv in the server
we can also input in the server and then recv in the client and so on;


