FROM fightinggg/cpp-dev-env:master
RUN yum install libcurl-devel -y
COPY . /app
WORKDIR /app
RUN cmake .
RUN make

FROM centos:8
COPY --from=0 /app/pdfs /usr/local/pdfs/bin/pdfs
ENV PATH="/usr/local/pdfs/bin:${PATH}"
CMD "pdfs"



#  docker build -t  pdfs .  && docker rm -f pdfs &&  docker run -d -p 8081:8080 --name pdfs pdfs && docker logs -f pdfs
#  docker build -t  pdfs .  && docker rm -f pdfs &&  docker run -d -p 8081:8080 --name pdfs pdfs && docker stats
