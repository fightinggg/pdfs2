#include "../stream.h"

class SplitNewLineStream : public InputStream {
    string s;
    int readed = 0;


    bool read(char *ch) override {
        if (readed < s.size()) {
            *ch = s[readed++];
            return 1;
        } else {
            return 0;
        }
    }

    void close() override {
    }

    int size() override {
        return ((int) s.size()) - readed;
    }

public:

    explicit SplitNewLineStream(InputStream *in, int size) {
        string all = in->readNbytes();
        for (int i = 0; i < all.size(); i++) {
            this->s += all[i];
            if (i % size == size - 1) {
                this->s += '\n';
            }
        }
    }
};


class SplitNewLineDecodeStream : public InputStream {
    string s;
    int readed = 0;


    bool read(char *ch) override {
        if (readed < s.size()) {
            *ch = s[readed++];
            return 1;
        } else {
            return 0;
        }
    }

    void close() override {
    }

    int size() override {
        return ((int) s.size()) - readed;
    }

public:

    explicit SplitNewLineDecodeStream(InputStream *in) {
        string all = in->readNbytes();
        for (char i: all) {
            if (i != '\n') {
                this->s += i;
            }
        }
    }
};