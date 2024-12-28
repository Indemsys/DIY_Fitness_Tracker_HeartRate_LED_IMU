#ifndef __COMPRESS_IO_H
  #define __COMPRESS_IO_H

  #ifdef ENABLE_SDRAM
    #define COMPRESSOR_MALLOC_PENDING  SDRAM_malloc_pending
    #define COMPRESSOR_MEM_FREE        SDRAM_free
  #else
    #define COMPRESSOR_MALLOC_PENDING  App_malloc_pending
    #define COMPRESSOR_MEM_FREE        App_free
  #endif

void             cio_Put_char(compress_data_t *strm, int data);
int              cio_Get_char(compress_data_t *strm);
void             cio_Rewind(compress_data_t *strm);
T_compress_size  cio_Get_pos(compress_data_t *strm);
T_compress_size  cio_Get_len(compress_data_t *strm);
void* cio_malloc(unsigned int size);
void             cio_free(void *ptr);
#endif
