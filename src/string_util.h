#ifndef STRING_UTIL_H_SFSVXNAL
#define STRING_UTIL_H_SFSVXNAL

int numberOfStringMatches(const char *s, const char *search);

//return a pointer to the nth occurance of term in s, or NULL
const char* findNthString(const char *s, const char *term, int n);

#endif //STRING_UTIL_H_SFSVXNAL