# 8080 端口用作webapi，不对外开放
# 9999 端口用作nbd server，对bdb client开放，不对外开发

nbdkit curl -p 9988 url=http://localhost:8080/a.txt
/app/pdfs $@







###############################
###############################
###############################
###############################
###############################
###############################
###############################
###############################
# docker build -t  pdfs .  && docker rm -f nbdclient &&  docker run -d --net=host  --name nbdclient pdfs bash -c "nbd-client 172.17.0.2 9999 /dev/nbd9915 && bash /app/loop.sh" && docker logs -f nbdclient

# nbdkit curl -p 9988 url=http://localhost:8080/a.txt


#nbd-client localhost 9988 /dev/nbd9988
#nbdkit curl -p 9999 url=http://localhost:8080/a.txt
#modprobe nbd
#/app/pdfs > a.txt
#nbd-client localhost 9999 /dev/nbd9999d
#fdisk /dev/nbd9999
#mkdir /mnt/nbd9999
#mkfs -t ext4 /dev/nbd9999
#mount /dev/nbd9999 /mnt/nbd9999

#
# nbdkit memory 1M -p 9014
# nbd-client localhost 9014 /dev/nbd9014

# --privileged --cap-add=ALL -v /dev:/dev -v /libsrc/modules:/libsrc/modules

# docker build -t  pdfs .  && docker rm -f pdfs &&  docker run -it --privileged  --name pdfs  pdfs bash

