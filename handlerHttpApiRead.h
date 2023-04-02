#include "allheader.h"
#include "httpMsg.h"

void doHttpApiRead(const HttpReq &req, HttpRsp &rsp) {
    // /api/read/{index}/{filename}

    string more = req.url.substr(strlen("/api/read/"));

    rsp.body = read(more);
    rsp.status = 200;
}
