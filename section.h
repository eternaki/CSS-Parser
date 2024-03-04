typedef struct Section {
    int countSelectors;
    int countAttributes;
    Selector* selectors;
    Attribute* attributes;
} Section;