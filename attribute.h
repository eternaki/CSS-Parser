typedef struct Attribute {
    char* key;
    char* value;
    struct Attribute* next;
    struct Attribute* prev;
} Attribute;