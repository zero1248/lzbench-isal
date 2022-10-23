#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <igzip_lib.h>
// #include "common.h"
#include <stddef.h>
#include "compress-isal.h"

#define BUF_SIZE 4*1024*1024

#define LEVEL_DEFAULT    1
#define UNIX            3

int level_size_buf[10] = {
#ifdef ISAL_DEF_LVL0_DEFAULT
    ISAL_DEF_LVL0_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL1_DEFAULT
    ISAL_DEF_LVL1_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL2_DEFAULT
    ISAL_DEF_LVL2_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL3_DEFAULT
    ISAL_DEF_LVL3_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL4_DEFAULT
    ISAL_DEF_LVL4_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL5_DEFAULT
    ISAL_DEF_LVL5_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL6_DEFAULT
    ISAL_DEF_LVL6_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL7_DEFAULT
    ISAL_DEF_LVL7_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL8_DEFAULT
    ISAL_DEF_LVL8_DEFAULT,
#else
    0,
#endif
#ifdef ISAL_DEF_LVL9_DEFAULT
    ISAL_DEF_LVL9_DEFAULT,
#else
    0,
#endif
};

int64_t isal_compress(size_t cxlevel, size_t srcsize, char *src, size_t tgtsize, char *tgt) {


    struct isal_zstream stream;
    struct isal_gzip_header gz_hdr;
    unsigned char *level_buf = NULL;    //*inbuf = NULL,
    size_t level_buf_size;
    int err = -1;
    unsigned l_in_size, l_out_size;

    if (tgtsize <= 0) {
        return -1;
    }
    // gy modify
    if (srcsize > 0xffffffff || srcsize < 0) {
        return -1;
    }

    isal_gzip_header_init(&gz_hdr);
    gz_hdr.os = UNIX;
    
    level_buf_size = level_size_buf[LEVEL_DEFAULT];
    level_buf =  (uint8_t *)malloc (level_buf_size);
    

    isal_deflate_init(&stream);
    stream.avail_in = 0;
    stream.flush = NO_FLUSH;
	stream.level = cxlevel;
    stream.level_buf = level_buf;
    stream.level_buf_size = level_buf_size;
    stream.gzip_flag = IGZIP_GZIP_NO_HDR;
    stream.next_out = (uint8_t *)tgt;
    stream.avail_out = (uint32_t)tgtsize;
    
    /* Fill the file header with 10 bytes of compressed information */
    isal_write_gzip_header(&stream, &gz_hdr);
    
	if (stream.avail_in == 0) {
		stream.next_in = (uint8_t *)src;
    	stream.avail_in = (uint32_t)srcsize;
		stream.end_of_stream = 1;        // 1 yes  0 no
	}
    
    if (stream.next_out == NULL) {
        stream.next_out = (uint8_t *)tgt;
        stream.avail_out = (uint32_t)tgtsize;
    }

    err = isal_deflate(&stream);
    if (err != COMP_OK) {
		printf("igzip error encountered while compressing file with error %d\n", err);
		if(level_buf) {
			free(level_buf);
		}
        return -1;
    }

	if(level_buf) {
		free(level_buf);
	}
    return stream.total_out;
}


int64_t isal_decompress(size_t srcsize, char *src, size_t tgtsize, char *tgt) {
    
    struct inflate_state state;
    struct isal_gzip_header gz_hdr;

    int err = -1;

    isal_gzip_header_init(&gz_hdr);
    
    isal_inflate_init(&state);
    state.crc_flag = ISAL_GZIP_NO_HDR_VER;
    state.next_in = (uint8_t *)src;
    state.avail_in = (uint32_t)srcsize;


    /* Read and parse the first 10 bytes of compressed information in the file header */
    err = isal_read_gzip_header(&state, &gz_hdr);
    if (err != ISAL_DECOMP_OK) {
        printf("igzip: Error invalid gzip header found for file .\n");
        return -1;
    }
    
    state.next_out = (uint8_t *)tgt;
    state.avail_out = (uint32_t)tgtsize;
    
    err = isal_inflate(&state);
    if (err != ISAL_DECOMP_OK) {
        printf("igzip: Error encountered while decompressing file.\n");
        return -1;
    }
    return state.total_out;
}