/*   file    : util.c
 *   author  : rayss
 *   date    : 2022.03.12
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 工具包的实现                   
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"


void myfree(void* p) {     /* 使用函数指针 指向myfree时， 不能用inline */
    if (p) {
        free(p);
    }
}


void *mymalloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        myerror("malloc failed");
    }
    return ptr;
}


void *mycalloc(size_t count, size_t size) {
    void* ptr = calloc(count, size);
    if (!ptr) {
        myerror("calloc failed");
    }
    return ptr;
}


void *mystrstr(const void* str1, size_t str1Len, const void* str2, size_t str2Len) {
    const char* ptr = (const char *)str1;
    const char* end = (const char *)((const char *)str1 + (str1Len - str2Len + 1));
    const char firstch = *(const char *)str2;
    size_t count;

    while (ptr < end) {
        ptr = (const char *)memchr((const void *)ptr, firstch, end - ptr);
        if (!ptr) {
            return NULL;
        }
        count = 0;
        while (count != str2Len && *(ptr + count) == *((const char *)str2 + count)) {
            count++;
        }
        if (count == str2Len) {
            return (void *)ptr;
        }
        //ptr += count;        //TODO
        ptr += 1;
    }

    return NULL;
}


void lowercase(char* str) {
    while (*str) {
        if ('A' <= *str && *str <= 'Z') {
            *str = *str + ('a' - 'A');
        }
        str++;
    }
}


char *strnchr(const char* s, char ch, size_t n) {
    while (n--) {
        if (*s == ch) {
            break;
        }
        s++;
    }
    if (n == 0 && *s != ch) {
        return NULL;
    }
    return (char *)s;  //TODO 这个n是前n个字符还是[0, n]的字符。
}


void replacechr(char* s, char old, char neww) {
    while (*s) {
        if (*s == old) {
            *s = neww;
        }
        s++;
    }
}


char *mystrdup(const char* s) {
    char *ptr = (char *)mymalloc(strlen(s) + 1);
    strcpy(ptr, s);
    return ptr;
}


char *mystrcat(int argc, const char* str1, ...) {
    va_list strs;
    va_start(strs, str1);
    char *ss = mystrdup(str1);
    unsigned int len = strlen(ss);

    for (int i = 0; i < argc - 1; ++i) {
        const char* s = va_arg(strs, const char *);
        len += strlen(s);
        if (!(ss = (char *)realloc((void *)ss, len + 1))) {
            myerror("alloc mem for mystrcat failed");
        }
        ss[len] = '\0';
        strcat(ss, s);
    }

    va_end(strs);
    return ss;
}


void myerror(const char* format, ...) {
    fprintf(stderr, ANSI_BG_RED);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, ANSI_RESET"\n");
    exit(EXIT_FAILURE);
}


void mywarning(const char* format, ...) {
    fprintf(stderr, ANSI_BG_YELLOW);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, ANSI_RESET"\n");
    exit(EXIT_FAILURE);
}