#include "allheader.h"
#include "stream.h"
#include "fsGithubApi.h"

void fswrite(map<string, string> fs, int start, int end, InputStream *inputStream) {
    if (fs["type"] == "systemfile") {

    } else if (fs["type"] == "githubapi") {
        githubApiFswrite(fs, start, end, inputStream);
    }
}

InputStream *fsRead(map<string, string> fs, int start, int end) {
    if (fs["type"] == "systemfile") {
        return nullptr;
    } else if (fs["type"] == "githubapi") {
        return githubApiFsRead(fs, start, end);
    }
    return nullptr;
}