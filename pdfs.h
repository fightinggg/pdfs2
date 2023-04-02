
#include "allheader.h"


struct PdfsBlock {
    int fsIndex;
    string status; // =uploading, =ready
    string blockname;
};


struct PdfsMenu {
    map<string, PdfsMenu> submenu;
    map<string, vector<vector<PdfsBlock>>> files;
};


struct Pdfs {
    PdfsMenu pdfsMenu;
    vector<map<string, string>> fs;
};

void initPdfs(Pdfs *pdfs) {
    map<string, string> config;
    config["type"] = "githubapi";
    pdfs->fs.push_back(config);
}


