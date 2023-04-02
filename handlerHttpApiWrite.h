#include "allheader.h"
#include "httpMsg.h"

void doHttpApiWrite(const HttpReq &req, HttpRsp &rsp) {
    // /api/write/{index}/{filename}

    string more = req.url.substr(strlen("/api/write/"));

    write(more, 0, 100, req.body);
}
