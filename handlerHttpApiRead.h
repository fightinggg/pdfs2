#include "allheader.h"
#include "httpMsg.h"

void doHttpApiRead(const HttpReq &req, HttpRsp &rsp) {
    // /api/read/{index}/{filename}

    string more = req.url.substr(strlen("/api/read/"));

    rsp.body = read(0, 1024);
    rsp.status = 200;
}
