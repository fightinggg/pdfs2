#pragma once

class InputStream {
public:

    // return !=1, if nothing to read.
    virtual bool read(char *) {
        printf("InputStream.h virtual int read");
        exit(-1);
    }

public:
    virtual ~InputStream() = default;

    virtual void close() {
        printf("InputStream.h virtual void close");
        exit(-1);
    }

    // return how much bytes remained to read
    // return -1 if unknown
    virtual int size() {
        return -1;
    }

    string readNbytes(int n = -1) {
        if (size() != -1) {
            n = minInt(n, size());
        }
        string res;
        while (true) {
            static int i = 0;
            i++;
            if (i % 1000 == 0) {
                i++;
            }
            if (n != -1 && res.size() == n) {

                fflush(stdout);
                return res;
            }

            char ch;
            if (!read(&ch)) {
                return res;
            } else {
                res += ch;
            }
        }
    }
};