

#include "allheader.h"
#include "handlerHttpApiRead.h"
#include "handlerHttpApiWrite.h"
#include "handlerHttpApiList.h"
#include "handlerHttpDefault.h"
#include "stringUtils.h"


void doApi(const HttpReq &req, HttpRsp &rsp) {
    if (req.method == "GET" && startsWith(req.url, "/api/read/")) {
        doHttpApiRead(req, rsp);
    } else if (req.method == "POST" && startsWith(req.url, "/api/write/")) {
        doHttpApiWrite(req, rsp);
    } else if (req.method == "GET" && startsWith(req.url, "/api/list/")) {
        doHttpApiList(req, rsp);
    } else {
        doHttpDefault(req, rsp);
    }
}
