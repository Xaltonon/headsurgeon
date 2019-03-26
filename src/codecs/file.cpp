#include "codecs/file.h"

FileHandle::FileHandle(const char *fname, const char *mode) {
    file = fopen(fname, mode);
    if (!file)
        throw "error opening file for reading";
}

FileHandle::~FileHandle() { fclose(file); }
