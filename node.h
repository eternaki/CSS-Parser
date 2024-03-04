#define N 8

typedef struct Node {
    Section* sections[N];
    int count;
    int deleted;
    struct Node* next;
    struct Node* prev;
} Node;