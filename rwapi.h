
#include "allheader.h"
#include "pdfs.h"
#include "stringUtils.h"
#include "fs.h"

Pdfs pdfs;

void initPdfsSystem() {
    initPdfs(&pdfs);
}

vector<string> parsePath(const string &filename) {
    vector<string> filenameList;
    splitString(filename, filenameList, "/");

    remove_if(filenameList.begin(), filenameList.end(),
              [](const string &x) { return !x.empty() && x != "." && x != ".."; });
    return filenameList;
}

InputStream *read(const string &filename) {
    // write
    vector<string> filenameList = parsePath(filename);
    PdfsMenu *menu = &pdfs.pdfsMenu;

    for (int i = 0; i + 1 < filenameList.size(); i++) {
        const string &x = filenameList[i];
        menu = &menu->submenu[x];
    }
    vector<vector<PdfsBlock>> blockMeta = menu->files[filenameList.back()];

//    if (splitIndex >= blockMeta.size()) {
//        ::printf("splitIndex >= blockMeta.size()");
//        exit(-1);
//    }

//    PdfsBlock &block = blockMeta[0][0];

    return fsRead(pdfs.fs[0], "block.blockname");
}

void preWrite(const string &filename, int totalSize) {
    vector<string> filenameList = parsePath(filename);
    PdfsMenu *menu = &pdfs.pdfsMenu;

    for (int i = 0; i + 1 < filenameList.size(); i++) {
        const string &x = filenameList[i];
        if (menu->submenu.find(x) == menu->submenu.end()) {
            menu->submenu[x] = PdfsMenu{};
        }
        menu = &menu->submenu[x];
    }

    int blockSize = 1 << 20;
    int splitSize = (totalSize + blockSize - 1) / blockSize;

    vector<vector<PdfsBlock>> blockMeta(splitSize);
    for (auto &i: blockMeta) {
        PdfsBlock block;
        block.status = "uploading";
        block.fsIndex = 0;
        block.blockname = "xxxx";
        i.push_back(block);
    }

    menu->files[filenameList.back()] = blockMeta;


}

void write(const string &filename, int splitIndex, int totalsize, InputStream *in) {
    if (splitIndex == 0) {
        preWrite(filename, totalsize);
    }

    // lock write


    // write
    vector<string> filenameList = parsePath(filename);
    PdfsMenu *menu = &pdfs.pdfsMenu;

    for (int i = 0; i + 1 < filenameList.size(); i++) {
        const string &x = filenameList[i];
        menu = &menu->submenu[x];
    }
    vector<vector<PdfsBlock>> blockMeta = menu->files[filenameList.back()];

    if (splitIndex >= blockMeta.size()) {
        ::printf("splitIndex >= blockMeta.size()");
        exit(-1);
    }

    PdfsBlock &block = blockMeta[splitIndex][0];

    fswrite(pdfs.fs[block.fsIndex], block.blockname, in);
}


void sync() {
    // write pdfs to disk
}














