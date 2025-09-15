#pragma once
#include <stdint.h>

#if defined(_WIN32) && defined(FPK_DLL_EXPORTS)
  #define FPK_API __declspec(dllexport)
#else
  #define FPK_API
#endif

#define FPK_MAGIC "FPK2"
#define FPK_MAX_FILES 1024
#define FPK_MAX_NAME  256
#define FPK_MAX_META  256

typedef struct {
    char name[FPK_MAX_NAME];
    uint32_t size;             // original size             // original size
    uint32_t compressed_size;  // compressed size (if compressed)sed_size;  // compressed size (if compressed)
    uint32_t offset;t offset;
    uint8_t  compressed;       // 1 = compressed, 0 = not    uint8_t  compressed;       // 1 = compressed, 0 = not
} FPKEntry;

FPK_API int fpk_pack(const char *archive, const char **files, int file_count, const char *meta);const char **files, int file_count, const char *meta);


FPK_API int fpk_list(const char *archive);FPK_API int fpk_extract(const char *archive);FPK_API int fpk_extract(const char *archive);
FPK_API int fpk_list(const char *archive);