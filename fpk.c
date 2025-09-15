#include "fpk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COPY_BUF_SIZE 4096
#define FPK_MAGIC "FPK1"

FPK_API int fpk_pack(const char *archive, const char **files, int file_count, const char *meta) {
    FILE *out = fopen(archive, "wb");
    if (!out) return 1;

    fwrite(FPK_MAGIC, 1, 4, out);
    fwrite(&file_count, 4, 1, out);

    char meta_buf[FPK_MAX_META] = {0};
    if (meta) strncpy(meta_buf, meta, FPK_MAX_META-1);
    fwrite(meta_buf, 1, FPK_MAX_META, out);

    // Reserve space for entry headers
    long entry_table_pos = ftell(out);
    for (int i = 0; i < file_count; ++i) {
        uint32_t nlen = 0;
        fwrite(&nlen, 4, 1, out); // name length
        char zero[FPK_MAX_NAME] = {0};
        fwrite(zero, 1, FPK_MAX_NAME, out); // name
        uint32_t zero32 = 0;
        fwrite(&zero32, 4, 1, out); // size
        fwrite(&zero32, 4, 1, out); // offset
    }

    // Write file data and collect entry info
    uint32_t *sizes = malloc(file_count * sizeof(uint32_t));
    uint32_t *offsets = malloc(file_count * sizeof(uint32_t));
    char names[FPK_MAX_FILES][FPK_MAX_NAME];

    for (int i = 0; i < file_count; ++i) {
        FILE *in = fopen(files[i], "rb");
        if (!in) { fclose(out); free(sizes); free(offsets); return 2; }
        strncpy(names[i], files[i], FPK_MAX_NAME-1);
        names[i][FPK_MAX_NAME-1] = 0;

        fseek(in, 0, SEEK_END);
        sizes[i] = (uint32_t)ftell(in);
        fseek(in, 0, SEEK_SET);
        offsets[i] = (uint32_t)ftell(out);

        char buf[COPY_BUF_SIZE];
        uint32_t left = sizes[i];
        while (left) {
            size_t chunk = left > COPY_BUF_SIZE ? COPY_BUF_SIZE : left;
            size_t r = fread(buf, 1, chunk, in);
            fwrite(buf, 1, r, out);
            left -= (uint32_t)r;
        }
        fclose(in);
    }

    // Write entry headers
    fseek(out, entry_table_pos, SEEK_SET);
    for (int i = 0; i < file_count; ++i) {
        uint32_t nlen = (uint32_t)strlen(names[i]) + 1;
        fwrite(&nlen, 4, 1, out);
        fwrite(names[i], 1, nlen, out);
        if (nlen < FPK_MAX_NAME)
            fwrite("\0", 1, FPK_MAX_NAME - nlen, out); // pad name
        fwrite(&sizes[i], 4, 1, out);
        fwrite(&offsets[i], 4, 1, out);
    }

    free(sizes);
    free(offsets);
    fclose(out);
    return 0;
}

FPK_API int fpk_extract(const char *archive) {
    FILE *in = fopen(archive, "rb");
    if (!in) return 1;
    char magic[5] = {0};
    fread(magic, 1, 4, in);
    if (strncmp(magic, FPK_MAGIC, 4)) { fclose(in); return 2; }
    uint32_t count;
    fread(&count, 4, 1, in);
    char meta[FPK_MAX_META];
    fread(meta, 1, FPK_MAX_META, in);

    struct {
        char name[FPK_MAX_NAME];
        uint32_t size, offset;
    } entry;

    for (uint32_t i = 0; i < count; ++i) {
        uint32_t nlen;
        fread(&nlen, 4, 1, in);
        fread(entry.name, 1, FPK_MAX_NAME, in);
        fread(&entry.size, 4, 1, in);
        fread(&entry.offset, 4, 1, in);

        FILE *out = fopen(entry.name, "wb");
        if (!out) continue;
        fseek(in, entry.offset, SEEK_SET);
        uint32_t left = entry.size;
        char buf[COPY_BUF_SIZE];
        while (left) {
            size_t chunk = left > COPY_BUF_SIZE ? COPY_BUF_SIZE : left;
            size_t r = fread(buf, 1, chunk, in);
            fwrite(buf, 1, r, out);
            left -= (uint32_t)r;
        }
        fclose(out);
        fseek(in, (4 + FPK_MAX_NAME + 4 + 4) * (i + 1) + 4 + 4 + FPK_MAX_META, SEEK_SET);
    }
    fclose(in);
    return 0;
}

FPK_API int fpk_list(const char *archive) {
    FILE *in = fopen(archive, "rb");
    if (!in) return 1;
    char magic[5] = {0};
    fread(magic, 1, 4, in);
    if (strncmp(magic, FPK_MAGIC, 4)) { fclose(in); return 2; }
    uint32_t count;
    fread(&count, 4, 1, in);
    char meta[FPK_MAX_META];
    fread(meta, 1, FPK_MAX_META, in);

    printf("Meta: %s\nFiles:\n", meta);
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t nlen;
        char name[FPK_MAX_NAME];
        uint32_t size, offset;
        fread(&nlen, 4, 1, in);
        fread(name, 1, FPK_MAX_NAME, in);
        fread(&size, 4, 1, in);
        fread(&offset, 4, 1, in);
        printf("  %s (%u bytes)\n", name, size);
    }
    fclose(in);
    return 0;
}