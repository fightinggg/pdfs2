
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

void initPdfs(Pdfs *pdfs, int argc, char **argv) {
    map<string, string> config;


    static struct option long_options[] = {
//            {"githubUsername", required_argument, nullptr, 0},
//            {"githubRepoName", required_argument, nullptr, 0},
            {"githubToken",    required_argument, nullptr, 0},
            {0, 0,                                0,       0}  // 添加 {0, 0, 0, 0} 是为了防止输入空值
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc,
                              argv,
                              "",
                              long_options,
                              &option_index)) != -1) {
//        printf("opt = %c\n", opt); // 命令参数，亦即 -a -b -n -r
//        printf("optarg = %s\n", optarg); // 参数内容
//        printf("optind = %d\n", optind); // 下一个被处理的下标值
//        printf("argv[optind - 1] = %s\n",  argv[optind - 1]); // 参数内容
//        printf("option_index = %d\n", option_index);  // 当前打印参数的下标值
//        printf("\n");
        printf("param: %s=%s\n", long_options[option_index].name, optarg);
        config[long_options[option_index].name] = optarg;
    }

    config["type"] = "githubapi";
    pdfs->fs.push_back(config);

}


