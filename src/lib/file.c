/*   file    : file-descriptor
 *   author  : rayss
 *   date    : 2022.03.12
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 处理文件                   
 */

#include <string.h>

#include "file.h"
#include "util.h"


FILE *safe_fopen(const char *path, const char *mode) {
    FILE *fp;
    if ((fp = fopen(path, mode)) == NULL) {
        myerror("open file %s by mode %s failed", path, mode);
    }
    return fp;
}


void safe_fclose(FILE *fp) {
    if (fp) {
        fclose(fp);
    }
}


size_t hexprint(tByte *byte_ptr, size_t length) {
    size_t count = 0;

    while (length--) {
        printf("%02X", *byte_ptr);
        byte_ptr++;

        count++;
        
        if (count % 16 == 0) { //16个字节一行
            printf("\n");
        }
        else {
            printf(" ");       //中间以空格分开
        }
    }

    printf("\n");
    return count;
}


tBool match_file_suffix(tCString filename, tCString suffix) {
    tCString dot = strchr(filename, '.');
    return dot && !strcmp(dot + 1, suffix);     //strcmp 两str相等返回0
}


size_t getfilesize(FILE *fp) {
    struct stat s;

    fstat(fileno(fp), &s);
    if (s.st_size == -1) {
        myerror("call %s failed", __func__);
        //__FILE__,__func__,__LINE__常用于logout，打trace，debug调试。
    }

    return s.st_size;
}


tCString pathcat(tCString dir, tCString filename) {
    if (!dir) {
        return mystrdup(filename);
    }

    size_t len = strlen(dir);
    char last = *(dir + len - 1);

    if (last == '/') {
        return (tCString)mystrcat(2, dir, filename);
    }
    else {
        return (tCString)mystrcat(3, dir, PATH_DELIMITER, filename);
    }
}


tCString url2filename(tCString url) {
    //TODO
}


tCString getdirname(tCString filename) {  /*  '/home/ray/download/a.pcap'  */
    tCString last_loc = strrchr(filename, '/');  /*  strrchr & strchr  */
    if (last_loc) {
        return strndup(filename, last_loc - filename);
    }
    return mystrdup("");
}


tCString getbasename(tCString filename) {  /* 获取末尾的文件名或路径名 */
    tCString ptr = strrchr(filename, '/');
    return ptr == NULL ? NULL : mystrdup(ptr + 1);  /* if-else replace by 三目 */
}


tCString getfilesuffix(tCString filename) {
    tCString dot = strrchr(filename, '.');
    return dot ? mystrdup(dot + 1) : NULL;
}


DATA_BLOCK read_file_into_memory(tCString path) {   /* DATA_BLOCK = {*data, len} */
    DATA_BLOCK datablock = {
        .data = NULL,
        .len  = 0
    };

    FILE* fp = safe_fopen(path, "rb");            /* 'rb'写死了，可以提供个参数 */

    size_t filesize = getfilesize(fp);
    if (!filesize) {          /* empty file? */
        safe_fclose(fp);
        return datablock;
    }

    tByte *data = mymalloc(filesize);

    if (fread((void *)data, 1, filesize, fp) != filesize) {
        safe_fclose(fp);
        logging("Can't read entire file into memory: %s", path);
        return datablock;
    }

    safe_fclose(fp);
    datablock.data = data;
    datablock.len = filesize;

    return datablock;
}