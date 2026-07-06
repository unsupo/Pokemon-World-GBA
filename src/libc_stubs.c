// Minimal C library stubs for Homebrew arm-none-eabi-gcc (no newlib).
// Each function uses O0 to prevent GCC from re-emitting a call to itself.
#include <stddef.h>

__attribute__((optimize("O0")))
void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    while (n--)
        *d++ = *s++;
    return dest;
}

__attribute__((optimize("O0")))
void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    if (d < s || d >= s + n) {
        while (n--)
            *d++ = *s++;
    } else {
        d += n;
        s += n;
        while (n--)
            *--d = *--s;
    }
    return dest;
}

__attribute__((optimize("O0")))
void *memset(void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char *)s;
    unsigned char v = (unsigned char)c;
    while (n--)
        *p++ = v;
    return s;
}

__attribute__((optimize("O0")))
int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *a = (const unsigned char *)s1;
    const unsigned char *b = (const unsigned char *)s2;
    while (n--) {
        if (*a != *b)
            return (int)*a - (int)*b;
        a++; b++;
    }
    return 0;
}

__attribute__((optimize("O0")))
void *memchr(const void *s, int c, size_t n)
{
    const unsigned char *p = (const unsigned char *)s;
    unsigned char v = (unsigned char)c;
    while (n--) {
        if (*p == v)
            return (void *)p;
        p++;
    }
    return NULL;
}

__attribute__((optimize("O0")))
size_t strlen(const char *s)
{
    const char *p = s;
    while (*p)
        p++;
    return (size_t)(p - s);
}

__attribute__((optimize("O0")))
char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

__attribute__((optimize("O0")))
char *strncpy(char *dest, const char *src, size_t n)
{
    char *d = dest;
    while (n && *src) {
        *d++ = *src++;
        n--;
    }
    while (n--)
        *d++ = '\0';
    return dest;
}

__attribute__((optimize("O0")))
char *strcat(char *dest, const char *src)
{
    char *d = dest;
    while (*d)
        d++;
    while ((*d++ = *src++))
        ;
    return dest;
}

__attribute__((optimize("O0")))
char *strncat(char *dest, const char *src, size_t n)
{
    char *d = dest;
    while (*d)
        d++;
    while (n-- && *src)
        *d++ = *src++;
    *d = '\0';
    return dest;
}

__attribute__((optimize("O0")))
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

__attribute__((optimize("O0")))
int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && *s1 == *s2) {
        s1++; s2++; n--;
    }
    if (!n) return 0;
    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

__attribute__((optimize("O0")))
int strcoll(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

__attribute__((optimize("O0")))
char *strchr(const char *s, int c)
{
    char ch = (char)c;
    while (*s) {
        if (*s == ch)
            return (char *)s;
        s++;
    }
    return ch == '\0' ? (char *)s : NULL;
}

__attribute__((optimize("O0")))
char *strrchr(const char *s, int c)
{
    char ch = (char)c;
    const char *last = NULL;
    while (*s) {
        if (*s == ch)
            last = s;
        s++;
    }
    if (ch == '\0')
        return (char *)s;
    return (char *)last;
}

__attribute__((optimize("O0")))
char *strstr(const char *haystack, const char *needle)
{
    if (!*needle)
        return (char *)haystack;
    while (*haystack) {
        const char *h = haystack, *n = needle;
        while (*h && *n && *h == *n) { h++; n++; }
        if (!*n)
            return (char *)haystack;
        haystack++;
    }
    return NULL;
}

__attribute__((optimize("O0")))
size_t strspn(const char *s, const char *accept)
{
    size_t n = 0;
    while (*s) {
        const char *a = accept;
        while (*a && *a != *s) a++;
        if (!*a) break;
        n++; s++;
    }
    return n;
}

__attribute__((optimize("O0")))
size_t strcspn(const char *s, const char *reject)
{
    size_t n = 0;
    while (*s) {
        const char *r = reject;
        while (*r && *r != *s) r++;
        if (*r) break;
        n++; s++;
    }
    return n;
}

__attribute__((optimize("O0")))
char *strpbrk(const char *s, const char *accept)
{
    while (*s) {
        const char *a = accept;
        while (*a) {
            if (*a == *s)
                return (char *)s;
            a++;
        }
        s++;
    }
    return NULL;
}

__attribute__((optimize("O0")))
size_t strxfrm(char *dest, const char *src, size_t n)
{
    size_t len = strlen(src);
    if (n > 0) {
        strncpy(dest, src, n - 1);
        dest[n - 1] = '\0';
    }
    return len;
}

__attribute__((optimize("O0")))
char *strtok(char *str, const char *delim)
{
    static char *saved;
    if (str)
        saved = str;
    if (!saved || !*saved)
        return NULL;
    while (*saved && strchr(delim, *saved))
        saved++;
    if (!*saved)
        return NULL;
    char *start = saved;
    while (*saved && !strchr(delim, *saved))
        saved++;
    if (*saved)
        *saved++ = '\0';
    return start;
}

__attribute__((optimize("O0")))
char *strerror(int errnum)
{
    (void)errnum;
    return (char *)"";
}
