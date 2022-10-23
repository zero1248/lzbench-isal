#ifndef COMPRESS_ISAL_H
#define COMPRESS_ISAL_H

#ifdef __cplusplus
extern "C" {
#endif

extern int64_t isal_compress(size_t cxlevel, size_t srcsize, char *src, size_t tgtsize, char *tgt);
extern int64_t isal_decompress(size_t srcsize, char *src, size_t tgtsize, char *tgt);






#ifdef __cplusplus
}
#endif

#endif // COMPRESS_ISAL_H
