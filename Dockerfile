FROM fightinggg/cpp-dev-env:master
COPY . /app
WORKDIR /app
RUN g++ -lpthread main.cpp -o pdfs

FROM centos:8
COPY --from=0 /app/pdfs /usr/local/pdfs/bin/pdfs
ENV PATH="/usr/local/pdfs/bin:${PATH}"
CMD "pdfs"