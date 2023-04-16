
#include "allheader.h"
#include "pdfs.h"
#include "stringUtils.h"
#include "fs.h"

Pdfs pdfs;

void initPdfsSystem(int argc, char **args) {
    initPdfs(&pdfs, argc, args);
}

shared_ptr<InputStream> read(int start, int end) {
    return fsRead(pdfs.fs[0], start, end);
}


void write(int start, int end, shared_ptr<InputStream> in) {
    fswrite(pdfs.fs[0], start, end, in);
}















