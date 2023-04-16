#include "allheader.h"
#include "httpMsg.h"

volatile long vv = 0;


void doHttpDefault(const HttpReq &req, HttpRsp &rsp) {
    string rspdata = "Welcome TO PDFS V2.0, We Will Come back Soon vv = " + to_string(++vv) + "\n";


    rspdata += "method = " + req.method + "\n";
    rspdata += "URL = " + req.url + "\n";
    rspdata += "404 NOT FOUND ERROR\n";

    rsp.status = 200;
    rsp.body = shared_ptr<InputStream>(new StringInputStream(rspdata));
}
