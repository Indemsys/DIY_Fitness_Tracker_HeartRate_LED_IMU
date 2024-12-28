#ifndef _APP_COMPRESSED_WEB_FILES_H
  #define _APP_COMPRESSED_WEB_FILES_H

typedef struct {
    const char* name;
    const uint8_t* data;
    uint32_t size;
} T_app_compressed_file_rec ;

#define APP_COMPRESSED_WEB_FILES_NUM 5

extern const T_app_compressed_file_rec app_compressed_web_files[APP_COMPRESSED_WEB_FILES_NUM];

#endif
