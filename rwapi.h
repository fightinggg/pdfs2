
#include "allheader.h"
#include "pdfs.h"
#include "stringUtils.h"
#include "fs.h"

Pdfs pdfs;

void initPdfsSystem(int argc, char **args) {
    initPdfs(&pdfs, argc, args);
}

//char buf[100 << 20];

shared_ptr<InputStream> read(int start, int end) {
//    return shared_ptr<InputStream>(new StringInputStream(string(buf + start, buf + end + 1)));

    ::printf("vbdio read: %d-%d\n", start, end);
    return fsRead(pdfs.fs[0], start, end);
}


void write(int start, int end, shared_ptr<InputStream> in) {
//    for (int i = start; i <= end; i++) {
//        in->read(buf + i);
//    }

    ::printf("vbdio write: %d-%d\n", start, end);
    fswrite(pdfs.fs[0], start, end, in);
}















