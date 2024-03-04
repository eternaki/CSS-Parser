typedef struct Selector {
    char* name;
    struct Selector* next;
    struct Selector* prev;
} Selector;