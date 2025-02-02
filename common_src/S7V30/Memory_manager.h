#ifndef APP_MEM_MAN_H
#define APP_MEM_MAN_H


  #ifdef ENABLE_SDRAM
    #define JSON_MALLOC_PENDING  SDRAM_malloc_pending
    #define JSON_MALLOC          SDRAM_malloc
    #define JSON_FREE            SDRAM_free
  #else
    #define JSON_MALLOC_PENDING  App_malloc_pending
    #define JSON_MALLOC          App_malloc
    #define JSON_FREE            App_free
  #endif


uint32_t       RAM_pool_create(void);
void*          App_malloc_pending(ULONG size, ULONG wait_option);
void*          App_malloc(uint32_t size);
void*          App_calloc(size_t num, size_t size);
void*          App_malloc_named(char const*name, size_t size);
void*          App_calloc_named(char const*name, size_t num, size_t size);

void           App_free(void *block_ptr);
void           App_memory_pools_creation(void *first_unused_memory);

void           App_get_RAM_pool_statistic(uint32_t *avail_bytes, uint32_t *fragments);
uint32_t       App_get_pool_size(void);
#endif // APP_MEM_MAN_H



