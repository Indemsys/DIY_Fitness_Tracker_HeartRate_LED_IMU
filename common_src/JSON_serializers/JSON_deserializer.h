#ifndef JSON_DESERIALIZER_H
  #define JSON_DESERIALIZER_H

uint32_t JSON_Deser_tables(uint8_t ptype, json_t *root);
uint32_t JSON_Deser_settings(uint8_t ptype, char *text);
uint32_t JSON_Deser_and_Exec_command(char *text, uint32_t f_long_stream);

#endif



