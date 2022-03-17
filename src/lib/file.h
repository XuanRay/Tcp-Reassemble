/*   file    : file.h
 *   author  : rayss
 *   date    : 2022.03.12
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 处理文件的.h                   
 */

#ifndef __FILE_H
#define __FILE_H


#ifdef _WIN32
    #inlcude <Windows.h>
    #define PATH_DELIMITER "\\"
#else
    #define PATH_DELIMITER "/"
#endif //_WIN32

#include <stdio.h>
#include <sys/stat.h>  //https://www.cnblogs.com/aboutblank/p/3617196.html
#include "types.h"


typedef struct Data_Block_
{
    tByte *data;
    size_t len;
} DATA_BLOCK;

//封装fopen
extern FILE *safe_fopen(const char *path, const char *mode);

//封装fclose
extern void safe_fclose(FILE *fp);

//以十六进制打印 返回值为多少字节
extern size_t hexprint(tByte *byte_ptr, size_t length);

//比较文件filename后缀名与suffix是否一致
extern tBool match_file_suffix(tCString filename, tCString suffix);

//获取文件大小
extern size_t getfilesize(FILE *fp);



/*  -----------------------------------------------------
 *  below functions will allocate memory for return value
 *  -----------------------------------------------------
 */

//路径合并
extern tCString pathcat(tCString dir, tCString filename);

//从url获取filename
extern tCString url2filename(tCString url);

//获取目录名
extern tCString getdirname(tCString filename);

//获取文件basename
extern tCString getbasename(tCString filename);

//获取文件后缀
extern tCString getfilesuffix(tCString filename);

//将文件读入内存
extern DATA_BLOCK read_file_into_memory(tCString path);




#endif //__FILE_H