gcc master.c -o master
gcc up.c -o up
gcc np.c -o np
gcc socket.c -o socket
gcc cb.c -o cb -pthread -lrt

./master

