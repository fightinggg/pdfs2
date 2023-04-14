
# pdfs2

```
docker pull fightinggg/pdfs:master && \
docker rm -f pdfs &&  \
docker run -d \
-p 8080:8080 --name pdfs fightinggg/pdfs:master
```

mkdir /mnt/8088
mkdir /mnt/8080

sudo mount -t davfs http://localhost:8088 /mnt/8088
sudo mount -t davfs http://localhost:8080 /mnt/8080

sudo umount /mnt/dav