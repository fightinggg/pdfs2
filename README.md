
# pdfs2

```shell
# 启动 server
# docker build -t  pdfs .  && docker rm -f pdfs &&  docker run -d -p 9999:9999 --name pdfs pdfs bash /app/start.sh && docker logs -f pdfs
# 装载设备 【有问题，因此无法合并到第一步中】 & 分区 & 挂载
# modprobe nbd
# nbd-client $(docker inspect -f '{{range.NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $( docker ps -aqf "name=pdfs")) 9999 /dev/nbd9001
# fdisk /dev/nbd9001
# mkfs -t ext4 /dev/nbd9001
# mkdir -p /mnt/nbd9001 && mount /dev/nbd9001 /mnt/nbd9001
# chmod -R 777 /mnt/nbd9001 && chown -R root:root /mnt/nbd9001
# 启动 webdav 服务
# docker rm -f webdav  && docker run --restart always --detach --name webdav --publish 7000:8080 --env WEBDAV_USERNAME=myuser --env WEBDAV_PASSWORD=mypassword  --volume  /mnt/nbd9001:/media ionelmc/webdav
```


内存泄漏分析

```shell
 valgrind --leak-check=full ./cmake-build-debug/pdfs --githubToken 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 

```