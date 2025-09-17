#include "fpk.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage:\n");
        printf("  fpk pack <archive.fpk> <files...>\n");
        printf("  fpk list <archive.fpk>\n");
        printf("  fpk extract <archive.fpk>\n");
        return 1;
    }

    if (!strcmp(argv[1],"pack")) {
        return fpk_pack(argv[2], (const char**)&argv[3], argc-3, "meta");
    } else if (!strcmp(argv[1],"list")) {
        return fpk_list(argv[2]);
    } else if (!strcmp(argv[1],"extract")) {
        return fpk_extract(argv[2]);
    }

    return 0;
}
