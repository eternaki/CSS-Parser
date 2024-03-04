#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include "Attribute.h"
#include "Selector.h"
#include "Section.h"
#include "Node.h"
#define N 8

enum { CSS, COMMANDS };

Attribute* createAttribute(char* key, char* value) {
    Attribute* attribute = (Attribute*)malloc(sizeof(Attribute));
    attribute->key = (char*)malloc(strlen(key) + 1);
    attribute->value = (char*)malloc(strlen(value) + 1);
    attribute->next = NULL;
    attribute->prev = NULL;
    memcpy(attribute->key, key, strlen(key) + 1);
    memcpy(attribute->value, value, strlen(value) + 1);
    return attribute;
}

// Function to add an Attribute to a Section
void pushBackAttribute(Section* section, Attribute* attribute) {
    if (section->attributes == NULL) {
        section->attributes = attribute;
    }
    else {
        Attribute* curr = section->attributes;
        Attribute* prev = NULL;
        while (curr != NULL && strcmp(curr->key, attribute->key) < 0) {
            prev = curr;
            curr = curr->next;
        }
        attribute->next = curr;
        if (prev != NULL) {
            prev->next = attribute;
            attribute->prev = prev;
        }
        else {
            section->attributes = attribute;
        }
        if (curr != NULL) {
            curr->prev = attribute;
        }
    }
    section->countAttributes++;
}


// Function to delete an Attribute from a Section
void deleteAttribute(Section* section, Attribute* attribute) {
    if (section->attributes == NULL || attribute == NULL) {
        return;
    }
    Attribute* curr = section->attributes;
    for (; curr != NULL; curr = curr->next) {
        if (curr == attribute) {
            if (attribute->prev == NULL) {
                section->attributes = attribute->next;
            }
            else {
                attribute->prev->next = attribute->next;
            }
            if (attribute->next != NULL) {
                attribute->next->prev = attribute->prev;
            }
            section->countAttributes--;
            free(attribute->key);
            free(attribute->value);
            free(attribute);
            break;
        }
    }
}


// Function to create a Selector
Selector* createSelector(char* name) {
    Selector* selector = (Selector*)malloc(sizeof(Selector));
    selector->name = (char*)malloc(strlen(name) + 1);
    selector->next = NULL;
    selector->prev = NULL;
    memcpy(selector->name, name, strlen(name) + 1);
    return selector;
}

// Function to add a Selector to a Section
void pushBackSelector(Section* section, Selector* selector) {
    if (section->selectors == NULL) {
        section->selectors = selector;
        selector->prev = NULL;
        selector->next = NULL;
    }
    else {
        Selector* last = section->selectors;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = selector;
        selector->prev = last;
        selector->next = NULL;
    }
    section->countSelectors++;
}


// Function to delete a Selector from a Section
void deleteSelector(Section* section, Selector* selector) {
    if (section->selectors == NULL || selector == NULL) {
        return;
    }

    Selector* curr = section->selectors;
    if (curr == selector) {
        section->selectors = selector->next;
        if (selector->next != NULL) {
            selector->next->prev = NULL;
        }
        free(selector->name);
        free(selector);
        section->countSelectors--;
        return;
    }

    while (curr != NULL && curr->next != selector) {
        curr = curr->next;
    }
    if (curr == NULL) {
        return;
    }

    curr->next = selector->next;
    if (selector->next != NULL) {
        selector->next->prev = curr;
    }
    free(selector->name);
    free(selector);
    section->countSelectors--;
}


// Function to create a Section
Section* createSection() {
    Section* section = (Section*)malloc(sizeof(Section));
    section->countAttributes = 0;
    section->countSelectors = 0;
    section->selectors = NULL;
    section->attributes = NULL;
    return section;
}


// Function to create a Node
Node* createNode() {
    Node* node = NULL;
    if ((node = (Node*)calloc(1, sizeof(Node))) == NULL) {
        printf("Nie udało się zaalokować pamięci dla węzła.\n");
        return NULL;
    }
    node->count = 0;
    node->deleted = 0;
    for (int i = 0; i < N; i++) {
        node->sections[i] = NULL;
    }
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void pushBack(Node** head, Node* new_node) {
    if (*head == NULL) {
        *head = new_node;
    }
    else {
        Node* current_node = *head;
        while (current_node->next != NULL) {
            current_node = current_node->next;
        }
        current_node->next = new_node;
    }
}

void insertAfter(Node* node, Node* new_node) {
    if (node == NULL || new_node == NULL) {
        return;
    }
    new_node->prev = node;
    new_node->next = node->next;
    if (node->next != NULL) {
        node->next->prev = new_node;
    }
    node->next = new_node;
}

void insertBefore(Node* node, Node* new_node) {
    if (node == NULL || new_node == NULL) {
        return;
    }
    new_node->next = node;
    new_node->prev = node->prev;
    if (node->prev != NULL) {
        node->prev->next = new_node;
    }
    node->prev = new_node;
}


// Function to add a Section to a Node
void pushBackSection(Node** node, Section* section) {
    if (*node == NULL) {
        *node = createNode();
        if (*node == NULL) {
            printf("Nie udało się utworzyć nowego węzła.\n");
            return;
        }
    }

    Node* curr_node = *node;
    while (curr_node->count == N) {
        if (curr_node->next == NULL) {
            Node* newNode = createNode();
            if (newNode == NULL) {
                printf("Nie udało się utworzyć nowego węzła.\n");
                return;
            }
            newNode->prev = curr_node;
            curr_node->next = newNode;
        }
        curr_node = curr_node->next;
    }

    curr_node->sections[curr_node->count] = section;
    curr_node->count++;
}


int countSections(Node* head) {
    int count = 0;
    Node* curr_node = head;
    while (curr_node != NULL) {
        count += (curr_node->count - curr_node->deleted);
        curr_node = curr_node->next;
    }
    return count;
}

void deleteNode(Node** head, Node* nodeToDelete) {
    if (*head == nullptr || nodeToDelete == nullptr) {
        return;
    }

    // Usuń węzeł z listy
    if (*head == nodeToDelete) {
        *head = nodeToDelete->next;
    }

    if (nodeToDelete->next != nullptr) {
        nodeToDelete->next->prev = nodeToDelete->prev;
    }

    if (nodeToDelete->prev != nullptr) {
        nodeToDelete->prev->next = nodeToDelete->next;
    }

    // Zwolnij pamięć zaalokowaną na węzeł
    Section* section;
    Selector* selector;
    Attribute* attribute;

    for (Section* section : nodeToDelete->sections) {
        if (section != nullptr) {
            Selector* selector = section->selectors;
            while (selector != nullptr) {
                Selector* nextSelector = selector->next;
                delete selector;
                selector = nextSelector;
            }

            Attribute* attribute = section->attributes;
            while (attribute != nullptr) {
                Attribute* nextAttribute = attribute->next;
                delete attribute;
                attribute = nextAttribute;
            }

            delete section;
        }
    }

    delete nodeToDelete;
    if (*head == nullptr) {
        *head = createNode();
    }
}


// Function to delete a Section from a Node
void deleteSection(Node** node, int index) {
    Node* curr_node = *node;
    if (index > countSections(curr_node)) return;
    int number = index - 1;
    while (curr_node->next != NULL) {
        if (number < (curr_node->count - curr_node->deleted)) {
            break;
        }
        number -= (curr_node->count - curr_node->deleted);
        curr_node = curr_node->next;
    }
    Section* section = curr_node->sections[number];
    if (section->countAttributes != 0) {
        Attribute* currAttribute = section->attributes;
        while (currAttribute != NULL && currAttribute->key != NULL && currAttribute->value == NULL) {
            Attribute* nextAttribute = currAttribute->next;
            deleteAttribute(section, currAttribute);
            currAttribute = nextAttribute;
        }
    }
    if (section->countSelectors != 0) {
        Selector* currSelector = section->selectors;
        while (currSelector != NULL) {
            Selector* nextSelector = currSelector->next;
            deleteSelector(section, currSelector);
            currSelector = nextSelector;
        }
    }
    free(section);
    for (int i = number; i < curr_node->count - 1; i++) {
        curr_node->sections[i] = curr_node->sections[i + 1];
    }
    curr_node->sections[curr_node->count - 1] = NULL;
    if (curr_node->next == NULL || curr_node->next->count == 0) {
        curr_node->count--;
    }
    else
        curr_node->deleted++;
    if (curr_node->count - curr_node->deleted == 0) {
        deleteNode(node, curr_node);
    }
}

bool checkInt(char* numArray) {
    int i = 0;
    if (numArray[0] == '-' || numArray[0] == '+')
        i++;
    for (; numArray[i] != '\0'; i++) {
        if (numArray[i] < '0' || numArray[i] > '9')
            return false;
    }
    return true;
}


int str_to_int(const char* numArray) {
    int num = 0;
    bool isNegative = false;

    if (*numArray == '-') {
        isNegative = true;
        numArray++;
    }

    while (*numArray >= '0' && *numArray <= '9') {
        num = (num * 10) + (*numArray - '0');
        numArray++;
    }

    return isNegative ? -num : num;
}


char* trim_whitespace(char* str) {
    int len = strlen(str);
    int start = 0, end = len - 1;
    while (end >= 0 && (isspace(str[end]) || str[end] < ' ')) {
        str[end--] = '\0';
    }
    while (start <= end && (isspace(str[start]) || str[start] < ' ')) {
        start++;
    }
    memmove(str, str + start, end - start + 2);

    return str;
}

void trim(char* str) {
    int len = strlen(str);
    int i = 0, j = len - 1;

    // Usuń białe znaki na początku napisu
    while (i < len && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r')) {
        i++;
    }

    // Usuń białe znaki na końcu napisu
    while (j >= i && (str[j] == ' ' || str[j] == '\t' || str[j] == '\n' || str[j] == '\r')) {
        j--;
    }

    // Przesuń napis w lewo
    if (i > 0) {
        for (int k = 0; k <= j - i; k++) {
            str[k] = str[i + k];
        }
        str[j - i + 1] = '\0';
    }
    else {
        str[j + 1] = '\0';
    }
}


// sprawdzamy czy istneje selector
bool selector_exist(Section* curr_section, char* name) {
    Selector* curr_selector = curr_section->selectors;
    while (curr_selector != NULL) {
        if (!strcmp(curr_selector->name, name)) {
            return true;
        }
        curr_selector = curr_selector->next;
    }
    return false;
}


char* replace(char* name) {
    for (int i = 0; i < strlen(name); i++) {
        name[i] = name[i + 1];
    }
    return name;
}

int my_atoi(const char* str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    if (str[0] == '-') {
        sign = -1;
        i++;
    }

    for (; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            // ignore non-numeric characters
            continue;
        }
        result = result * 10 + (str[i] - '0');
    }

    return result * sign;
}

void readCSS(Node** head) {
    Node* curr_node = *head;
    int state = CSS;

    int curr_index = 0;
    char line[1000];
    char backup[1000] = "start";
    while (fgets(line, sizeof(line) / sizeof(char), stdin)) {
        if (line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\t')
            line[strlen(line) - 1] = '\0';
        if (!strcmp(line, "????")) {
            state = COMMANDS;
        }
        else if (strstr(line, "****") != NULL) {
            state = CSS;
            if ((*head)->count == 0) {
                curr_node = *head;
                curr_index = 0;
                continue;
            }
            while (curr_node->next != NULL) {
                curr_node = curr_node->next;
            }
            if (curr_node->count == N)
                curr_index = 0;
            else
                curr_index = curr_node->count;
        }
        else if (state == CSS) {
            size_t len = strlen(line);
            for (int i = strlen(line) - 1; i >= 0; i--) {
                if (line[i] == '\r' || line[i] == '\n' || line[i] == ' ' || line[i] == '\t') {
                    line[i] = '\0';
                }
                else {
                    break;
                }
            }
            if (strchr(line, '}') == NULL || (strchr(line, '}') != NULL && strlen(line) > 1)) {
                if (strcmp(backup, "start")) {
                    strcat(backup, line);
                }
                else {
                    strcpy(backup, line);
                }
                if (!(strchr(line, '}') != NULL && strlen(line) > 1)) {
                    continue;
                }
            }
            pushBackSection(head, createSection());
            if (curr_node->count == N && curr_node->sections[curr_index]->countSelectors != 0) {
                curr_node = curr_node->next;
            }
            // Usuń klamrę otwierającą
            char* backup_attributes;
            char* section_name = strtok(backup, "{");
            backup_attributes = strtok(NULL, "{");
            if (backup_attributes != NULL) {
                char* selector_names = strtok(section_name, ",");
                while (selector_names != NULL) {
                    // Usuń białe znaki przed selektorem
                    while (isspace(*selector_names)) {
                        selector_names++;
                    }
                    if (!selector_exist(curr_node->sections[curr_index], selector_names)) {
                        trim_whitespace(selector_names);
                        pushBackSelector(curr_node->sections[curr_index], createSelector(selector_names));
                    }

                    // Przetwórz następny selektor
                    selector_names = strtok(NULL, ",");
                }
            }
            if (backup_attributes == NULL) {
                backup_attributes = _strdup(section_name);
            }
            // Dodaj atrybut do bieżącego selektora
            while (true) {
                char* attribute_key = strtok(backup_attributes, ":");
                char new_key[10000] = "";
                while (true) {
                    char* temp = strtok(NULL, ":");
                    if (temp == NULL) break;
                    strcat(new_key, ":");
                    strcat(new_key, temp);
                }
                char* attribute_value = strtok(new_key, ";");
                char new_value[10000] = "";
                while (true) {
                    char* temp = strtok(NULL, ";");
                    if (temp == NULL) break;
                    strcat(new_value, temp);
                    strcat(new_value, ";");
                }
                if (attribute_key != NULL && attribute_value != NULL) {
                    // Usuń białe znaki przed kluczem atrybutu
                    attribute_key = trim_whitespace(attribute_key);
                    // Usuń białe znaki przed wartością atrybutu
                    attribute_value = trim_whitespace(attribute_value);

                    if (curr_node->sections[curr_index] != NULL) {
                        attribute_value = strtok(attribute_value, ":");
                        if (attribute_value[strlen(attribute_value) - 1] == '}') {
                            attribute_value[strlen(attribute_value) - 1] = '\0';
                        }
                        if (attribute_value[0] == ':' || attribute_value[0] == ' ') {
                            strcpy(attribute_value, replace(attribute_value));
                        }
                        Attribute* curr_attribute = curr_node->sections[curr_index]->attributes;
                        bool attribute_updated = false;
                        for (int i = 0; i < curr_node->sections[curr_index]->countAttributes; i++) {
                            if (!strcmp(curr_attribute->key, attribute_key)) {
                                strcpy(curr_attribute->value, attribute_value);
                                attribute_updated = true;
                            }
                            curr_attribute = curr_attribute->next;
                        }
                        if (!attribute_updated) {
                            pushBackAttribute(curr_node->sections[curr_index], createAttribute(attribute_key, attribute_value));
                        }
                    }
                }
                else {
                    break;
                }
                strcpy(backup_attributes, new_value);
            }

            memset(&backup[0], 0, sizeof(backup));
            strcpy(backup, "start");
            curr_index++;
            curr_index %= N;
        }
        else if (state == COMMANDS) {
            if (strstr(line, "****")) {
                state = CSS;
                line[strlen(line) - 4] = '\0';
            }
            else if (strncmp(line, "****", 4) == 0) {
                state = CSS;
                memset(line, line[4], strlen(line)); // memset wypełnia tablicę znakiem
                strcpy(backup, line);
                continue;
            }
            else if (strcmp(line, "") == 0 || strcmp(line, "\n") == 0 || strcmp(line, "\t") == 0) {
                continue;
            }
            else if (strcmp(line, "?") == 0) {
                printf("? == %d\n", countSections(*head));
            }
            else {
                char* first_parammetr = strtok(line, ",");
                char* second_parammetr = strtok(NULL, ",");
                char* third_parammetr = strtok(NULL, ",");
                if (first_parammetr == NULL || second_parammetr == NULL || third_parammetr == NULL) {
                    printf("%s\n", line);
                    continue;
                }
                Node* curr_node = *head;
                Selector* curr_selector = NULL;
                Attribute* curr_attribute = NULL;
                if (strcmp(second_parammetr, "S") == 0 && checkInt(first_parammetr) && strcmp(third_parammetr, "?") == 0) {
                    int input_number = my_atoi(first_parammetr);
                    int total_sections = countSections(*head);
                    if (input_number > total_sections) {
                        continue;
                    }
                    int section_index = input_number - 1;
                    Node* curr_node = *head;
                    while (curr_node->next != NULL) {
                        if (section_index < (curr_node->count - curr_node->deleted)) {
                            break;
                        }
                        section_index -= (curr_node->count - curr_node->deleted);
                        curr_node = curr_node->next;
                    }
                    printf("%s,S,? == %d\n", first_parammetr, curr_node->sections[section_index]->countSelectors);
                }
                if (strcmp(second_parammetr, "S") == 0 && checkInt(first_parammetr) && checkInt(third_parammetr)) {
                    int first_param_int = my_atoi(first_parammetr);
                    int third_param_int = my_atoi(third_parammetr);
                    if (first_param_int > countSections(*head)) {
                        continue;
                    }
                    int number = first_param_int - 1;
                    while (curr_node->next != NULL) {
                        if (number < (curr_node->count - curr_node->deleted)) {
                            break;
                        }
                        number -= (curr_node->count - curr_node->deleted);
                        curr_node = curr_node->next;
                    }
                    Selector* curr_selector = curr_node->sections[number]->selectors;
                    for (int i = 1; i <= curr_node->sections[number]->countSelectors; i++) {
                        if (i == third_param_int) {
                            printf("%d,S,%d == %s\n", first_param_int, third_param_int, curr_selector->name);
                            break;
                        }
                        curr_selector = curr_selector->next;
                    }
                }
                if (!strcmp(second_parammetr, "S") && !checkInt(first_parammetr) && !strcmp(third_parammetr, "?")) {
                    int count = 0;
                    while (curr_node != NULL) {
                        for (int i = 0; i < (curr_node->count - curr_node->deleted); i++) {
                            curr_selector = curr_node->sections[i]->selectors;
                            for (int j = 1; j <= curr_node->sections[i]->countSelectors; j++) {
                                if (!strcmp(curr_selector->name, first_parammetr)) {
                                    count++;
                                    //break;
                                }
                                curr_selector = curr_selector->next;
                            }
                        }
                        curr_node = curr_node->next;
                    }
                    printf("%s,S,? == %d\n", first_parammetr, count);
                }
                if (!strcmp(second_parammetr, "A") && checkInt(first_parammetr) && !strcmp(third_parammetr, "?")) {
                    if (my_atoi(first_parammetr) > countSections(*head)) {
                        continue;
                    }
                    int number = my_atoi(first_parammetr) - 1;
                    while (curr_node->next != NULL) {
                        if (number < (curr_node->count - curr_node->deleted)) {
                            break;
                        }
                        number -= (curr_node->count - curr_node->deleted);
                        curr_node = curr_node->next;
                    }
                    printf("%s,A,? == %d\n", first_parammetr, curr_node->sections[number]->countAttributes);
                }
                if (!strcmp(second_parammetr, "A") && !checkInt(first_parammetr) && !strcmp(third_parammetr, "?")) {
                    int count = 0;
                    while (curr_node != NULL) {
                        for (int i = 0; i < (curr_node->count - curr_node->deleted); i++) {
                            curr_attribute = curr_node->sections[i]->attributes;
                            for (int j = 1; j <= curr_node->sections[i]->countAttributes; j++) {
                                if (!strcmp(curr_attribute->key, first_parammetr)) {
                                    count++;
                                    //break;
                                }
                                curr_attribute = curr_attribute->next;
                            }
                        }
                        curr_node = curr_node->next;
                    }
                    printf("%s,A,? == %d\n", first_parammetr, count);
                }
                if (!strcmp(second_parammetr, "A") && checkInt(first_parammetr) && strcmp(third_parammetr, "?")) {
                    int section_num = my_atoi(first_parammetr) - 1;
                    if (section_num >= countSections(*head)) {
                        continue;
                    }
                    Node* curr_node = *head;
                    while (section_num >= (curr_node->count - curr_node->deleted)) {
                        section_num -= (curr_node->count - curr_node->deleted);
                        curr_node = curr_node->next;
                    }
                    curr_attribute = curr_node->sections[section_num]->attributes;
                    for (int i = 1; i <= curr_node->sections[section_num]->countAttributes; i++) {
                        if (!strcmp(curr_attribute->key, third_parammetr)) {
                            printf("%s,A,%s == %s\n", first_parammetr, third_parammetr, curr_attribute->value);
                            break;
                        }
                        curr_attribute = curr_attribute->next;
                    }
                }
                if (!strcmp(second_parammetr, "E")) {
                    curr_node = *head;
                    while (curr_node->next != NULL) {
                        curr_node = curr_node->next;
                    }
                    bool sel = false, atr = false;
                    while (curr_node != NULL) {
                        for (int i = (curr_node->count - curr_node->deleted) - 1; i >= 0; i--) {
                            curr_selector = curr_node->sections[i]->selectors;
                            curr_attribute = curr_node->sections[i]->attributes;
                            for (int j = 1; j <= curr_node->sections[i]->countSelectors; j++) {
                                if (!strcmp(curr_selector->name, first_parammetr)) {
                                    sel = true;
                                    break;
                                }
                                curr_selector = curr_selector->next;
                            }
                            if (!sel) {
                                continue;
                            }
                            for (int j = 1; j <= curr_node->sections[i]->countAttributes; j++) {
                                if (!strcmp(curr_attribute->key, third_parammetr)) {
                                    atr = true;
                                    break;
                                }
                                curr_attribute = curr_attribute->next;
                            }
                            if (sel && atr) {
                                break;
                            }
                        }
                        if (sel && atr) {
                            break;
                        }
                        curr_node = curr_node->prev;
                    }
                    if (sel && atr && curr_attribute != NULL) {
                        printf("%s,E,%s == %s\n", first_parammetr, third_parammetr, curr_attribute->value);
                    }
                }
                if (!strcmp(second_parammetr, "D") && !strcmp(third_parammetr, "*")) {
                    int count = countSections(*head);
                    deleteSection(head, str_to_int(first_parammetr));
                    if (countSections(*head) < count) printf("%s,D,* == deleted\n", first_parammetr);
                }
                if (!strcmp(second_parammetr, "D") && strcmp(third_parammetr, "*")) {
                    if (str_to_int(first_parammetr) > countSections(*head)) {
                        continue;
                    }
                    int number = str_to_int(first_parammetr) - 1;
                    while (curr_node->next != NULL) {
                        if (number < (curr_node->count - curr_node->deleted)) {
                            break;
                        }
                        number -= (curr_node->count - curr_node->deleted);
                        curr_node = curr_node->next;
                    }
                    curr_attribute = curr_node->sections[number]->attributes;
                    for (int i = 1; i <= curr_node->sections[number]->countAttributes; i++) {
                        if (!strcmp(curr_attribute->key, third_parammetr)) {
                            deleteAttribute(curr_node->sections[number], curr_attribute);
                            printf("%s,D,%s == deleted\n", first_parammetr, third_parammetr);
                            break;
                        }
                        curr_attribute = curr_attribute->next;
                    }
                    if (curr_node->sections[number]->countAttributes == 0) {
                        deleteSection(head, str_to_int(first_parammetr));
                    }
                }
            }
        }
    }
}