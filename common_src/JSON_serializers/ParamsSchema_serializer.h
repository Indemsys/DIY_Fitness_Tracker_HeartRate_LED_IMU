#ifndef PARAMSSCHEMA_SERIALIZER_H
  #define PARAMSSCHEMA_SERIALIZER_H

char*    ParamsSchema_serialize_to_buff(uint8_t ptype, uint32_t *sz_ptr);
uint32_t ParamsSchema_serialize_to_file(uint8_t ptype, const char *file_name);


#endif



