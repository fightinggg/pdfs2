rpcgen nfs.x

# for ubuntu
https://askubuntu.com/questions/1360945/fatal-error-rpc-rpc-h-no-such-file-or-directory

sudo apt-get install apt-file
sudo apt-file update
sudo apt-file search rpc/pmap_clnt.h
make -f Makefile.nfs CFLAGS=-I/usr/include/ntirpc/

