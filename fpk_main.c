#include "fpk.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        puts("FPK - Free Pack File\nUsage:\n  fpk pack archive.fpk meta file1 file2 ...\n  fpk extract archive.fpk\n  fpk list archive.fpk");
        return 1;
    }
    if (!strcmp(argv[1], "pack")) {
        if (argc < 5) { puts("No files to pack."); return 1; }
        const char *meta = argv[3];
        const char **files = (const char**)&argv[4];
        int file_count = argc - 4;
        return fpk_pack(argv[2], files, file_count, meta);
    } else if (!strcmp(argv[1], "extract")) {
        return fpk_extract(argv[2]);
    } else if (!strcmp(argv[1], "list")) {
        return fpk_list(argv[2]);
    } else {
        puts("Unknown command.");
        return 1;
    }
}