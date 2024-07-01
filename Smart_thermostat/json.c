#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "json.h"

int jsoneq(const char* json, jsmntok_t* tok, const char* s) {
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}


int json_parse(const char* json_string, jsmntok_t* tokens, unsigned int num_tokens) {
    jsmn_parser parser;
    jsmn_init(&parser);

    int ret = jsmn_parse(&parser, json_string, strlen(json_string), tokens, num_tokens);
    //printf("Parse result: %d\n", ret);  // Debug print
    if (ret < 0) {
        printf("Failed to parse JSON: %d\n", ret);
        return 1;
    }

    /* Assume the top-level element is an object */
    if (ret < 1 || tokens[0].type != JSMN_OBJECT) {
        printf("Object expected\n");
        return 1;
    }

    return 0;
}

char* json_get_value(const char* json_string, jsmntok_t* tokens, unsigned int num_tokens, const char* key) {
    char* value = NULL;
    for (unsigned int i = 1; i < num_tokens; i++) {
        if (jsoneq(json_string, &tokens[i], key) == 0) {
            // We may use strndup() to fetch string value
            size_t value_length = tokens[i + 1].end - tokens[i + 1].start;
            if (value != NULL) {
                free(value);
            }
            value = malloc(value_length + 1);
            if (value == NULL) {
                return NULL;
            }
            strncpy(value, json_string + tokens[i + 1].start, value_length);
            value[value_length] = '\0';
            // Remove the enclosing quotes
            if (value[0] == '\"' && value[value_length - 1] == '\"') {
                memmove(value, value + 1, value_length - 1);
                value[value_length - 2] = '\0';
            }
        }
    }
    return value;
}

