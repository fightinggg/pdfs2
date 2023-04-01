#include "bits/stdc++.h"
#include "stream.h"

using namespace std;

volatile long vv = 0;


void doHandler(const string &method, const string &url, const map<string, string> &header, inputStream, int outfd) {

    string rspdata = "Welcome TO PDFS V2.0, We Will Come back Soon, vv = " + to_string(++vv) + "\n";

    rspdata += "method = " + method + "\n";
    rspdata += "URL = " + url + "\n";

    string rsphead = "HTTP/1.1 200 OK\nContent-Length: " + to_string(rspdata.size()) + "\n\n";
    send(outfd, rsphead.data(), rsphead.size(), 0);
    send(outfd, rspdata.data(), rspdata.size(), 0);
}