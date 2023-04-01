

void doApi(const string &url, const map<string, string> &header, inputStream, int outfd) {
    if (startsWith(url, "/api/read/")) {
        string rspdata = "URL = " + url + "\n";
        string rsphead = "HTTP/1.1 200 OK\nContent-Length: " + to_string(rspdata.size()) + "\n\n";
        send(outfd, rsphead.data(), rsphead.size(), 0);
        send(outfd, rspdata.data(), rspdata.size(), 0);
    } else if (startsWith(url, "/api/write/")) {
        string rspdata = "URL = " + url + "\n";
        string rsphead = "HTTP/1.1 200 OK\nContent-Length: " + to_string(rspdata.size()) + "\n\n";
        send(outfd, rsphead.data(), rsphead.size(), 0);
        send(outfd, rspdata.data(), rspdata.size(), 0);
    } else {
        string rspdata = "URL = " + url + "\n";
        string rsphead = "HTTP/1.1 200 OK\nContent-Length: " + to_string(rspdata.size()) + "\n\n";
        send(outfd, rsphead.data(), rsphead.size(), 0);
        send(outfd, rspdata.data(), rspdata.size(), 0);
    }
}