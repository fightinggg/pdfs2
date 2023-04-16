#pragma once

#include "../stream.h"


class BinaryStringInputStream : public InputStream {
    shared_ptr<InputStream> in;

    int totalSize = 0;
    int a[100];
    int queueReadIndex = 8;
    char queue[8]{'0', 'x', ' ', ' ', '(', ' ', ')', ' '}; // '0xff '

    bool read(char *ch) override {
        if (totalSize == 0) {
            return false;
        }
        totalSize--;
        if (queueReadIndex == 8) {
            queueReadIndex = 0;
            string binaryString = "0123456789abcdef";

            if (!in->read(ch)) {
                return false;
            }

            queue[2] = binaryString[(*ch) & 0xf];
            queue[3] = binaryString[(*ch) & 0xf];
            if (*ch >= 32 && *ch <= 126) {
                queue[5] = *ch;
            } else {
                queue[5] = '.';
            }

        }
        if (queueReadIndex >= 8) {
            exit(-1);
        }
        *ch = queue[queueReadIndex++];
        return true;
    }

    void close() override {
        in->close();
    }

    int size() override {
        return totalSize;
    }

public:

    explicit BinaryStringInputStream(const shared_ptr<InputStream>& in) {
        this->in = in;
        this->totalSize = in->size();
        if (totalSize != -1) {
            totalSize *= 8;
        }
    }
};