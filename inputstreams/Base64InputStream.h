#pragma once

/*
    static string base64Map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int base64[128];
    for (int i = 0; i < base64Map.size(); i++) {
        base64[base64Map[i]] = i;
    }
    for (int i = 0; i < 128; i++) {
        printf("0x%02x, ", 0x3f & base64[i]);
        if (i && i % 8 == 7) {
            printf("\n");
        }
    }

    for (int i = 0; i < 64; i++) {
        printf("%d ",  base64[base64Map[i]]);
    }
    return 0;

    0x20, 0x2c, 0x08, 0x2c, 0x08, 0x2c, 0x00, 0x00,
    0x20, 0x2c, 0x08, 0x2c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x30, 0x13, 0x18, 0x13,
    0x38, 0x13, 0x00, 0x00, 0x10, 0x13, 0x38, 0x13,
    0x10, 0x13, 0x10, 0x13, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3f,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00,
    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63


 */
static string base64Map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int base64RevMap[128] = {
        0x20, 0x2c, 0x08, 0x2c, 0x08, 0x2c, 0x00, 0x00,
        0x20, 0x2c, 0x08, 0x2c, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x30, 0x13, 0x18, 0x13,
        0x38, 0x13, 0x00, 0x00, 0x10, 0x13, 0x38, 0x13,
        0x10, 0x13, 0x10, 0x13, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3f,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
        0x3c, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
        0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
        0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
        0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
        0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00,
};

class Base64EncoderInputStream : public InputStream {
    shared_ptr<InputStream>in;
    int rawSize;
    int newSize;

    int queueReadIndex = 4;
    char queue[4]{};

    bool read(char *ch) override {
        if (newSize == 0) {
            return false;
        }
        newSize--;

        int readSize = 0;
        if (queueReadIndex == 4) {
            queue[0] = queue[1] = queue[2] = queue[3] = 0;
            readSize += (rawSize-- > 0 && in->read(queue + 0)) ? 1 : 0;
            readSize += (rawSize-- > 0 && in->read(queue + 1)) ? 1 : 0;
            readSize += (rawSize-- > 0 && in->read(queue + 2)) ? 1 : 0;
            queue[3] = char((queue[2])); // 00000000 00000000 00******
            queue[2] = char((queue[2] >> 6) | (queue[1] << 2)); // 00000000 0000**** **000000
            queue[1] = char((queue[1] >> 4) | (queue[0] << 4));  // 000000** ****0000 00000000
            queue[0] = char((queue[0] >> 2));// ******00 00000000 00000000

            queue[3] = base64Map[0x3f & queue[3]];
            queue[2] = base64Map[0x3f & queue[2]];
            queue[1] = base64Map[0x3f & queue[1]];
            queue[0] = base64Map[0x3f & queue[0]];

            if (readSize == 1) {
                queue[3] = queue[2] = '=';
            }
            if (readSize == 2) {
                queue[3] = '=';
            }

            queueReadIndex = 0;
        }
        *ch = queue[queueReadIndex++];
        return true;
    }

    void close() override {
        in->close();
    }

    int size() override {
        return newSize;
    }

public:


    explicit Base64EncoderInputStream(shared_ptr<InputStream>in, int rawSize) {
        this->in = in;
        this->rawSize = rawSize;
        this->newSize = (rawSize + 2) / 3 * 4;
    }

    void remove() {
        in = nullptr;
    }
};

class Base64DecoderInputStream : public InputStream {
    shared_ptr<InputStream>in;
    int sizeBeforeEncode;
    int sizeAfterEncode;

    int queueReadIndex = 3;
    char queue[4]{};

    bool read(char *ch) override {
        if (sizeBeforeEncode == 0) {
            return false;
        }
        sizeBeforeEncode--;

        int readSize = 0;
        if (queueReadIndex == 3) {
            queue[0] = queue[1] = queue[2] = queue[3] = 0;
            readSize += (sizeAfterEncode-- > 0 && in->read(queue + 0)) ? 1 : 0;
            readSize += (sizeAfterEncode-- > 0 && in->read(queue + 1)) ? 1 : 0;
            readSize += (sizeAfterEncode-- > 0 && in->read(queue + 2)) ? 1 : 0;
            readSize += (sizeAfterEncode-- > 0 && in->read(queue + 3)) ? 1 : 0;

            queue[0] = (char) base64RevMap[queue[0]];
            queue[1] = (char) base64RevMap[queue[1]];
            queue[2] = (char) base64RevMap[queue[2]];
            queue[3] = (char) base64RevMap[queue[3]];

            queue[0] = (char) ((queue[0] << 2) | (queue[1] >> 4));// 00****** 00**0000 00000000 00000000
            queue[1] = (char) ((queue[1] << 4) | (queue[2] >> 2));// 00000000 0000**** 00****00 00000000
            queue[2] = (char) ((queue[2] << 6) | (queue[3]));// 00000000 00000000 000000** 00******

            queueReadIndex = 0;
        }
        *ch = queue[queueReadIndex++];
        return true;
    }

    void close() override {
        in->close();
    }

    int size() override {
        return sizeBeforeEncode;
    }

public:

    explicit Base64DecoderInputStream(shared_ptr<InputStream> in, int sizeBeforeEncode) {
        this->in = in;
        this->sizeBeforeEncode = sizeBeforeEncode;
        this->sizeAfterEncode = (sizeBeforeEncode + 2 / 3) * 4;
    }
};