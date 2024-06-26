#ifndef JSON_H
#define JSON_H

#include "jsmn.h"

int json_parse(const char* json_string, jsmntok_t* tokens, unsigned int num_tokens);
char* json_get_value(const char* json_string, jsmntok_t* tokens, unsigned int num_tokens, const char* key);

#endif // JSON_H

