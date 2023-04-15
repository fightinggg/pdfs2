FROM ubuntu
RUN apt-get update
RUN apt-get install make cmake -y
RUN apt-get install gcc g++ -y
RUN apt-get install libcurl4-gnutls-dev -y
RUN apt-get install nbd-client nbdkit -y
RUN apt-get install kmod -y
RUN apt-get install linux-modules-extra-$(uname -r) -y
RUN apt-get install curl fdisk  -y
RUN apt-get install net-tools -y
COPY . /app
WORKDIR /app
RUN cmake .
RUN make





#  docker build -t  pdfs .  && docker rm -f pdfs &&  docker run -d -p 8080:8080 --name pdfs pdfs && docker stats
# docker pull  fightinggg/pdfs:master && docker rm -f pdfs &&  docker run -d -p 8080:8080 --name pdfs fightinggg/pdfs:master && docker logs -f pdfs