/*
 * dirent.h - dirent API for Microsoft Visual Studio
 *
 * Copyright (C) 2006-2012 Toni Ronkko
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * ``Software''), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL TONI RONKKO BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * $Id: dirent.h,v 1.20 2014/03/19 17:52:23 tronkko Exp $
 */
#ifndef MY_DIRENT_H
#define MY_DIRENT_H


#include <string.h>
#include <stdlib.h>
int removedir(const char *path);

#ifndef _WIN32

#include <dirent.h>
#define makedir(path) mkdir(path, 0754)

#else /* _WIN32 */
/*
 * Define architecture flags so we don't need to include windows.h.
 * Avoiding windows.h makes it simpler to use windows sockets in conjunction
 * with dirent.h.
 */
#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_IX86)
# define _X86_
#endif
#if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_AMD64)
# define _AMD64_
#endif

#include <stdio.h>
#include <stdarg.h>
#include <windef.h>
#include <winbase.h>
#include <wchar.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <direct.h>

/* Indicates that d_type field is available in dirent structure */
#define _DIRENT_HAVE_D_TYPE

/* Indicates that d_namlen field is available in dirent structure */
#define _DIRENT_HAVE_D_NAMLEN

/* Entries missing from MSVC 6.0 */
#if !defined(FILE_ATTRIBUTE_DEVICE)
# define FILE_ATTRIBUTE_DEVICE 0x40
#endif

/* File type and permission flags for stat() */
#if !defined(S_IFMT)
# define S_IFMT   _S_IFMT                     /* File type mask */
#endif
#if !defined(S_IFDIR)
# define S_IFDIR  _S_IFDIR                    /* Directory */
#endif
#if !defined(S_IFCHR)
# define S_IFCHR  _S_IFCHR                    /* Character device */
#endif
#if !defined(S_IFFIFO)
# define S_IFFIFO _S_IFFIFO                   /* Pipe */
#endif
#if !defined(S_IFREG)
# define S_IFREG  _S_IFREG                    /* Regular file */
#endif
#if !defined(S_IREAD)
# define S_IREAD  _S_IREAD                    /* Read permission */
#endif
#if !defined(S_IWRITE)
# define S_IWRITE _S_IWRITE                   /* Write permission */
#endif
#if !defined(S_IEXEC)
# define S_IEXEC  _S_IEXEC                    /* Execute permission */
#endif
#if !defined(S_IFIFO)
# define S_IFIFO _S_IFIFO                     /* Pipe */
#endif
#if !defined(S_IFBLK)
# define S_IFBLK   0                          /* Block device */
#endif
#if !defined(S_IFLNK)
# define S_IFLNK   0                          /* Link */
#endif
#if !defined(S_IFSOCK)
# define S_IFSOCK  0                          /* Socket */
#endif

#if defined(_MSC_VER)
# define S_IRUSR  S_IREAD                     /* Read user */
# define S_IWUSR  S_IWRITE                    /* Write user */
# define S_IXUSR  0                           /* Execute user */
# define S_IRGRP  0                           /* Read group */
# define S_IWGRP  0                           /* Write group */
# define S_IXGRP  0                           /* Execute group */
# define S_IROTH  0                           /* Read others */
# define S_IWOTH  0                           /* Write others */
# define S_IXOTH  0                           /* Execute others */
#endif

/* Maximum length of file name */
#if !defined(PATH_MAX)
# define PATH_MAX MAX_PATH
#endif
#if !defined(FILENAME_MAX)
# define FILENAME_MAX MAX_PATH
#endif
#if !defined(NAME_MAX)
# define NAME_MAX FILENAME_MAX
#endif

/* File type flags for d_type */
#define DT_UNKNOWN  0
#define DT_REG      S_IFREG
#define DT_DIR      S_IFDIR
#define DT_FIFO     S_IFIFO
#define DT_SOCK     S_IFSOCK
#define DT_CHR      S_IFCHR
#define DT_BLK      S_IFBLK
#define DT_LNK      S_IFLNK

/* Macros for converting between st_mode and d_type */
#define IFTODT(mode) ((mode) & S_IFMT)
#define DTTOIF(type) (type)

/*
 * File type macros.  Note that block devices, sockets and links cannot be
 * distinguished on Windows and the macros S_ISBLK, S_ISSOCK and S_ISLNK are
 * only defined for compatibility.  These macros should always return false
 * on Windows.
 */
#ifndef S_ISFIFO
#define	S_ISFIFO(mode) (((mode) & S_IFMT) == S_IFIFO)
#endif
#ifndef S_ISDIR
#define	S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define	S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISLNK
#define	S_ISLNK(mode)  (((mode) & S_IFMT) == S_IFLNK)
#endif
#ifndef S_ISSOCK
#define	S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)
#endif
#ifndef S_ISCHR
#define	S_ISCHR(mode)  (((mode) & S_IFMT) == S_IFCHR)
#endif
#ifndef S_ISBLK
#define	S_ISBLK(mode)  (((mode) & S_IFMT) == S_IFBLK)
#endif
/* Return the exact length of d_namlen without zero terminator */
#define _D_EXACT_NAMLEN(p) ((p)->d_namlen)

/* Return number of bytes needed to store d_namlen */
#define _D_ALLOC_NAMLEN(p) (PATH_MAX)


#ifdef __cplusplus
extern "C" {
#endif

/* Wide-character version */
struct _wdirent {
    long d_ino;                                 /* Always zero */
    unsigned short d_reclen;                    /* Structure size */
    size_t d_namlen;                            /* Length of name without \0 */
    int d_type;                                 /* File type */
    wchar_t d_name[PATH_MAX];                   /* File name */
};
typedef struct _wdirent _wdirent;

struct _WDIR {
    struct _wdirent ent;                        /* Current directory entry */
    WIN32_FIND_DATAW data;                      /* Private file data */
    int cached;                                 /* True if data is valid */
    HANDLE handle;                              /* Win32 search handle */
    wchar_t *patt;                              /* Initial directory name */
};
typedef struct _WDIR _WDIR;

_WDIR *_wopendir (const wchar_t *dirname);
struct _wdirent *_wreaddir (_WDIR *dirp);
int _wclosedir (_WDIR *dirp);
void _wrewinddir (_WDIR* dirp);

/* For compatibility with Symbian */
#define wdirent _wdirent
#define WDIR _WDIR
#define wopendir _wopendir
#define wreaddir _wreaddir
#define wclosedir _wclosedir
#define wrewinddir _wrewinddir

/* Multi-byte character versions */
struct dirent {
    long d_ino;                                 /* Always zero */
    unsigned short d_reclen;                    /* Structure size */
    size_t d_namlen;                            /* Length of name without \0 */
    int d_type;                                 /* File type */
    char d_name[PATH_MAX];                      /* File name */
};
typedef struct dirent dirent;

struct DIR {
    struct dirent ent;
    struct _WDIR *wdirp;
};
typedef struct DIR DIR;


#define makedir(path) mkdir(path)

DIR *opendir (const char *dirname);
struct dirent *readdir (DIR *dirp);
int closedir (DIR *dirp);
void rewinddir (DIR* dirp);

/* Internal utility functions */
WIN32_FIND_DATAW *dirent_first (_WDIR *dirp);
WIN32_FIND_DATAW *dirent_next (_WDIR *dirp);

int dirent_mbstowcs_s(
    size_t *pReturnValue,
    wchar_t *wcstr,
    size_t sizeInWords,
    const char *mbstr,
    size_t count);

int dirent_wcstombs_s(
    size_t *pReturnValue,
    char *mbstr,
    size_t sizeInBytes,
    const wchar_t *wcstr,
    size_t count);

void dirent_set_errno (int error);

#ifdef __cplusplus
}
#endif
#endif /* _WIN32 */
#endif /* MY_DIRENT_H */
