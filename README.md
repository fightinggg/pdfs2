
# pdfs2

[http://43.153.75.227:8080/read/0/100000](http://43.153.75.227:8080/read/0/100000)

使用
```shell
# 启动 server
docker run --restart=always -d -p 9999:9999 -p 8080:8080 --privileged --name pdfs fightinggg/pdfs:test bash /app/start.sh
# 装载设备 【有问题，因此无法合并到第一步中】 & 分区 & 挂载
# modprobe nbd
# docker run -d --privileged --net=host  --name nbdclient  fightinggg/pdfs:test bash -c "nbd-client $(docker inspect -f '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $( docker ps -aqf "name=pdfs")) 9999 /dev/nbd9901 && bash /app/loop.sh" && docker logs -f nbdclient
# fdisk /dev/nbd9901
# mkfs -t ext4 /dev/nbd9901
# mkdir -p /mnt/nbd9901 && mount /dev/nbd9901 /mnt/nbd9901
# chmod -R 777 /mnt/nbd9901 && chown -R root:root /mnt/nbd9901
# 启动 webdav 服务
# docker rm -f webdav  && docker run --restart always --detach --name webdav --publish 7000:8080 --env WEBDAV_USERNAME=myuser --env WEBDAV_PASSWORD=mypassword  --volume  /mnt/nbd9001:/media ionelmc/webdav
```

开发
```shell

# 启动 server
docker build -t  pdfs .  && docker rm -f pdfs &&  docker run -d -p 9999:9999 --name pdfs pdfs bash /app/start.sh --githubToken=$GITHUBTOKEN && docker logs -f pdfs
# 装载设备 【有问题，因此无法合并到第一步中】 & 分区 & 挂载
modprobe nbd
nbd-client $(docker inspect -f '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $( docker ps -aqf "name=pdfs")) 9999 /dev/nbd9005
fdisk /dev/nbd9005
mkfs -t ext4 /dev/nbd9002
mkdir -p /mnt/nbd9002 && mount /dev/nbd9002 /mnt/nbd9002
chmod -R 777 /mnt/nbd9002 && chown -R root:root /mnt/nbd9001
# 启动 webdav 服务
# docker rm -f webdav  && docker run --restart always --detach --name webdav --publish 7000:8080 --env WEBDAV_USERNAME=myuser --env WEBDAV_PASSWORD=mypassword  --volume  /mnt/nbd9001:/media ionelmc/webdav
```



内存泄漏分析

```shell
 valgrind --leak-check=full ./cmake-build-debug/pdfs --githubToken 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 

```