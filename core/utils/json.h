// json_parser.h
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

typedef struct JsonValue JsonValue;
typedef struct JsonMember JsonMember;

struct JsonValue {
    JsonType type;
    union {
        bool boolean;
        double number;
        char* string;
        struct {
            JsonValue* values;
            size_t count;
        } array;
        struct {
            JsonMember* members;
            size_t count;
        } object;
    } value;
};

struct JsonMember {
    char* key;
    JsonValue value;
};

typedef struct {
    const char* json;
    const char* pos;
    char* error;
} JsonParser;

// Основные функции
JsonValue json_parse_file(const char* filename);
void json_free(JsonValue* value);
void json_print(const JsonValue* value, int indent);
JsonValue* json_get_member(const JsonValue* obj, const char* key);

// Вспомогательные функции
static void json_skip_whitespace(JsonParser* parser);
static JsonValue json_parse_value(JsonParser* parser);
static JsonValue json_parse_object(JsonParser* parser);
static JsonValue json_parse_array(JsonParser* parser);
static JsonValue json_parse_string(JsonParser* parser);
static JsonValue json_parse_number(JsonParser* parser);
static JsonValue json_parse_literal(JsonParser* parser);
static char* json_read_file(const char* filename);

// Инициализация парсера
static JsonParser json_create_parser(const char* json) {
    JsonParser parser = {json, json, NULL};
    return parser;
}

// Пропуск пробельных символов
static void json_skip_whitespace(JsonParser* parser) {
    while (isspace((unsigned char)*parser->pos)) {
        parser->pos++;
    }
}

// Установка ошибки
static void json_set_error(JsonParser* parser, const char* message) {
    if (parser->error == NULL) {
        parser->error = malloc(256);
        snprintf(parser->error, 256, "Error at position %ld: %s", 
                 parser->pos - parser->json + 1, message);
    }
}

// Парсинг строки
static JsonValue json_parse_string(JsonParser* parser) {
    JsonValue value = {JSON_STRING, {.string = NULL}};
    
    if (*parser->pos != '"') {
        json_set_error(parser, "Expected string");
        return value;
    }
    
    parser->pos++;
    const char* start = parser->pos;
    size_t len = 0;
    
    while (*parser->pos != '"') {
        if (*parser->pos == '\0') {
            json_set_error(parser, "Unterminated string");
            return value;
        }
        if (*parser->pos == '\\') {
            parser->pos++;
        }
        parser->pos++;
        len++;
    }
    
    char* str = malloc(len + 1);
    const char* src = start;
    char* dst = str;
    
    while (src < parser->pos) {
        if (*src == '\\') {
            src++;
            switch (*src) {
                case '"': *dst++ = '"'; break;
                case '\\': *dst++ = '\\'; break;
                case '/': *dst++ = '/'; break;
                case 'b': *dst++ = '\b'; break;
                case 'f': *dst++ = '\f'; break;
                case 'n': *dst++ = '\n'; break;
                case 'r': *dst++ = '\r'; break;
                case 't': *dst++ = '\t'; break;
                default: *dst++ = *src; break;
            }
        } else {
            *dst++ = *src;
        }
        src++;
    }
    
    *dst = '\0';
    parser->pos++; // пропускаем закрывающую кавычку
    
    value.value.string = str;
    return value;
}

// Парсинг числа
static JsonValue json_parse_number(JsonParser* parser) {
    JsonValue value = {JSON_NUMBER, {.number = 0}};
    
    char* end;
    value.value.number = strtod(parser->pos, &end);
    
    if (end == parser->pos) {
        json_set_error(parser, "Invalid number");
        return value;
    }
    
    parser->pos = end;
    return value;
}

// Парсинг литералов (true, false, null)
static JsonValue json_parse_literal(JsonParser* parser) {
    JsonValue value;
    
    if (strncmp(parser->pos, "true", 4) == 0) {
        value.type = JSON_BOOL;
        value.value.boolean = true;
        parser->pos += 4;
    } else if (strncmp(parser->pos, "false", 5) == 0) {
        value.type = JSON_BOOL;
        value.value.boolean = false;
        parser->pos += 5;
    } else if (strncmp(parser->pos, "null", 4) == 0) {
        value.type = JSON_NULL;
        parser->pos += 4;
    } else {
        json_set_error(parser, "Invalid literal");
        value.type = JSON_NULL;
    }
    
    return value;
}

// Парсинг массива
static JsonValue json_parse_array(JsonParser* parser) {
    JsonValue value = {JSON_ARRAY, {.array = {NULL, 0}}};
    
    if (*parser->pos != '[') {
        json_set_error(parser, "Expected array");
        return value;
    }
    
    parser->pos++;
    json_skip_whitespace(parser);
    
    if (*parser->pos == ']') {
        parser->pos++;
        return value;
    }
    
    size_t capacity = 4;
    JsonValue* items = malloc(capacity * sizeof(JsonValue));
    size_t count = 0;
    
    while (true) {
        if (count >= capacity) {
            capacity *= 2;
            items = realloc(items, capacity * sizeof(JsonValue));
        }
        
        items[count] = json_parse_value(parser);
        if (parser->error) {
            for (size_t i = 0; i < count; i++) {
                json_free(&items[i]);
            }
            free(items);
            value.type = JSON_NULL;
            return value;
        }
        count++;
        
        json_skip_whitespace(parser);
        
        if (*parser->pos == ',') {
            parser->pos++;
            json_skip_whitespace(parser);
        } else if (*parser->pos == ']') {
            parser->pos++;
            break;
        } else {
            json_set_error(parser, "Expected ',' or ']'");
            for (size_t i = 0; i < count; i++) {
                json_free(&items[i]);
            }
            free(items);
            value.type = JSON_NULL;
            return value;
        }
    }
    
    value.value.array.values = items;
    value.value.array.count = count;
    return value;
}

// Парсинг объекта
static JsonValue json_parse_object(JsonParser* parser) {
    JsonValue value = {JSON_OBJECT, {.object = {NULL, 0}}};
    
    if (*parser->pos != '{') {
        json_set_error(parser, "Expected object");
        return value;
    }
    
    parser->pos++;
    json_skip_whitespace(parser);
    
    if (*parser->pos == '}') {
        parser->pos++;
        return value;
    }
    
    size_t capacity = 4;
    JsonMember* members = malloc(capacity * sizeof(JsonMember));
    size_t count = 0;
    
    while (true) {
        if (count >= capacity) {
            capacity *= 2;
            members = realloc(members, capacity * sizeof(JsonMember));
        }
        
        // Парсим ключ
        json_skip_whitespace(parser);
        
        if (*parser->pos != '"') {
            json_set_error(parser, "Expected string key");
            for (size_t i = 0; i < count; i++) {
                free(members[i].key);
                json_free(&members[i].value);
            }
            free(members);
            value.type = JSON_NULL;
            return value;
        }
        
        JsonValue key_value = json_parse_string(parser);
        if (parser->error) {
            free(key_value.value.string);
            for (size_t i = 0; i < count; i++) {
                free(members[i].key);
                json_free(&members[i].value);
            }
            free(members);
            value.type = JSON_NULL;
            return value;
        }
        
        members[count].key = key_value.value.string;
        
        // Пропускаем двоеточие
        json_skip_whitespace(parser);
        if (*parser->pos != ':') {
            json_set_error(parser, "Expected ':'");
            for (size_t i = 0; i <= count; i++) {
                free(members[i].key);
                json_free(&members[i].value);
            }
            free(members);
            value.type = JSON_NULL;
            return value;
        }
        parser->pos++;
        json_skip_whitespace(parser);
        
        // Парсим значение
        members[count].value = json_parse_value(parser);
        if (parser->error) {
            for (size_t i = 0; i <= count; i++) {
                free(members[i].key);
                json_free(&members[i].value);
            }
            free(members);
            value.type = JSON_NULL;
            return value;
        }
        
        count++;
        
        json_skip_whitespace(parser);
        
        if (*parser->pos == ',') {
            parser->pos++;
            json_skip_whitespace(parser);
        } else if (*parser->pos == '}') {
            parser->pos++;
            break;
        } else {
            json_set_error(parser, "Expected ',' or '}'");
            for (size_t i = 0; i < count; i++) {
                free(members[i].key);
                json_free(&members[i].value);
            }
            free(members);
            value.type = JSON_NULL;
            return value;
        }
    }
    
    value.value.object.members = members;
    value.value.object.count = count;
    return value;
}

// Основная функция парсинга значения
static JsonValue json_parse_value(JsonParser* parser) {
    json_skip_whitespace(parser);
    
    switch (*parser->pos) {
        case '{': return json_parse_object(parser);
        case '[': return json_parse_array(parser);
        case '"': return json_parse_string(parser);
        case 't':
        case 'f':
        case 'n': return json_parse_literal(parser);
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': return json_parse_number(parser);
        default:
            json_set_error(parser, "Invalid JSON value");
            JsonValue value = {JSON_NULL, {0}};
            return value;
    }
}

// Чтение файла в строку
static char* json_read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = malloc(length + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    
    return buffer;
}

// Основная функция для парсинга файла
JsonValue json_parse_file(const char* filename) {
    char* json_str = json_read_file(filename);
    if (!json_str) {
        JsonValue error = {JSON_NULL, {0}};
        return error;
    }
    
    JsonParser parser = json_create_parser(json_str);
    JsonValue result = json_parse_value(&parser);
    
    if (parser.error) {
        fprintf(stderr, "JSON parse error: %s\n", parser.error);
        free(parser.error);
    }
    
    // Проверяем, что весь JSON разобран
    json_skip_whitespace(&parser);
    if (*parser.pos != '\0' && !parser.error) {
        fprintf(stderr, "JSON parse error: Extra data after JSON\n");
    }
    
    free(json_str);
    return result;
}

// Освобождение памяти
void json_free(JsonValue* value) {
    if (!value) return;
    
    switch (value->type) {
        case JSON_STRING:
            free(value->value.string);
            break;
        case JSON_ARRAY:
            for (size_t i = 0; i < value->value.array.count; i++) {
                json_free(&value->value.array.values[i]);
            }
            free(value->value.array.values);
            break;
        case JSON_OBJECT:
            for (size_t i = 0; i < value->value.object.count; i++) {
                free(value->value.object.members[i].key);
                json_free(&value->value.object.members[i].value);
            }
            free(value->value.object.members);
            break;
        default:
            break;
    }
    value->type = JSON_NULL;
}

// Получение члена объекта
JsonValue* json_get_member(const JsonValue* obj, const char* key) {
    if (obj->type != JSON_OBJECT) return NULL;
    
    for (size_t i = 0; i < obj->value.object.count; i++) {
        if (strcmp(obj->value.object.members[i].key, key) == 0) {
            return &obj->value.object.members[i].value;
        }
    }
    return NULL;
}

// Вывод JSON (для отладки)
void json_print(const JsonValue* value, int indent) {
    if (!value) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (value->type) {
        case JSON_NULL:
            printf("null\n");
            break;
        case JSON_BOOL:
            printf(value->value.boolean ? "true\n" : "false\n");
            break;
        case JSON_NUMBER:
            printf("%g\n", value->value.number);
            break;
        case JSON_STRING:
            printf("\"%s\"\n", value->value.string);
            break;
        case JSON_ARRAY:
            printf("[\n");
            for (size_t i = 0; i < value->value.array.count; i++) {
                json_print(&value->value.array.values[i], indent + 1);
            }
            for (int i = 0; i < indent; i++) printf("  ");
            printf("]\n");
            break;
        case JSON_OBJECT:
            printf("{\n");
            for (size_t i = 0; i < value->value.object.count; i++) {
                for (int j = 0; j < indent + 1; j++) printf("  ");
                printf("\"%s\": ", value->value.object.members[i].key);
                JsonValue* val = &value->value.object.members[i].value;
                if (val->type == JSON_OBJECT || val->type == JSON_ARRAY) {
                    printf("\n");
                    json_print(val, indent + 2);
                } else {
                    json_print(val, 0);
                }
            }
            for (int i = 0; i < indent; i++) printf("  ");
            printf("}\n");
            break;
    }
}

#endif // JSON_PARSER_H