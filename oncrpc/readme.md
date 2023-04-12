rpcgen nfs.x

# for ubuntu
https://askubuntu.com/questions/1360945/fatal-error-rpc-rpc-h-no-such-file-or-directory

sudo apt-get install apt-file
sudo apt-file update
sudo apt-file search rpc/pmap_clnt.h


https://stackoverflow.com/questions/50590546/unable-to-build-rpc-project-with-libtirpc

make -f Makefile.nfs CFLAGS=-I/usr/include/tirpc/ LDLIBS=-ltirpc


https://zhuanlan.zhihu.com/p/564309388
sudo apt-get install portmap