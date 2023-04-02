#include "allheader.h"
#include "stream.h"
#include "fsGithubApi.h"

void fswrite(map<string, string> fs, const string &name, InputStream *inputStream) {
    if (fs["type"] == "systemfile") {

    } else if (fs["type"] == "githubapi") {
        githubApiFswrite(fs, name, inputStream);
    }
}

InputStream *fsRead(map<string, string> fs, const string &name) {
    if (fs["type"] == "systemfile") {
        return nullptr;
    } else if (fs["type"] == "githubapi") {
        return githubApiFsRead(fs, name);
    }
    return nullptr;
}