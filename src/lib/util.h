/*   file    : util.h
 *   author  : rayss
 *   date    : 2022.03.14
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 工具包                   
 */

#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <stdarg.h>


#ifdef DEBUG
    #define logging(...)          myerror(__VA_ARGS__)
#else
    #define logging(...)          mywarning(__VA_ARGS__)
#endif //DEBUG


// 终端显示颜色的变量
#define ANSI_RESET          "\x1b[0m"
#define ANSI_BOLD_ON        "\x1b[1m"
#define ANSI_INVERSE_ON     "\x1b[7m"
#define ANSI_BOLD_OFF       "\x1b[22m"
#define ANSI_FG_BLACK       "\x1b[30m"
#define ANSI_FG_RED         "\x1b[31m"
#define ANSI_FG_GREEN       "\x1b[32m"
#define ANSI_FG_YELLOW      "\x1b[33m"
#define ANSI_FG_BLUE        "\x1b[34m"
#define ANSI_FG_MAGENTA     "\x1b[35m"
#define ANSI_FG_CYAN        "\x1b[36m"
#define ANSI_FG_WHITE       "\x1b[37m"
#define ANSI_BG_RED         "\x1b[41m"
#define ANSI_BG_GREEN       "\x1b[42m"
#define ANSI_BG_YELLOW      "\x1b[43m"
#define ANSI_BG_BLUE        "\x1b[44m"
#define ANSI_BG_MAGENTA     "\x1b[45m"
#define ANSI_BG_CYAN        "\x1b[46m"
#define ANSI_BG_WHITE       "\x1b[47m"



#ifndef MAX
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define safe_free(x) myfree((void *)x)


//封装free
extern void myfree(void* p);

//封装malloc
extern void *mymalloc(size_t size);

//封装calloc
extern void *mycalloc(size_t count, size_t size);

//返回一个字符串str2在str1中的起始位置 字符串匹配算法
extern void *mystrstr(const void* str1, size_t str1Len, const void* str2, size_t str2Len);

//大写转小写
extern void lowercase(char* str);

//定位一个字符在字符串s中的前n个字符中所在的位置
extern char *strnchr(const char* s, char ch, size_t n);

//字符替换
extern void replacechr(char* s, char old, char neww);

//字符串深拷贝 封装了申请内存的操作
extern char *mystrdup(const char* s);

//多个字符串连接，strcat升级版 封装了申请内存的操作
extern char *mystrcat(int argc, const char* str1, ...);

//错误输出
extern void myerror(const char* format, ...);

//警告输出
extern void mywarning(const char* format, ...);

#endif //__UTIL_H