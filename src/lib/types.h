/*   file    : types.h
 *   author  : rayss
 *   date    : 2022.03.12
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 类型统一typedef                   
 */

#ifndef __TYPES_H
#define __TYPES_H


#ifndef _INT8_T
#define _INT8_T
typedef signed char             _int8_t;
#endif /* _INT8_T */

#ifndef _INT16_T
#define _INT16_T
typedef signed short            _int16_t;
#endif /* _INT16_T */

#ifndef _INT32_T
#define _INT32_T
typedef signed int              _int32_t;
#endif /* _INT32_T */

#ifndef _INT64_T
#define _INT64_T
typedef signed long long        _int64_t;
#endif /* _INT64_T */

#ifndef _UINT8_T
#define _UINT8_T
typedef unsigned char           _u_int8_t;
#endif /* _UINT8_T */

#ifndef _UINT16_T
#define _UINT16_T
typedef unsigned short          _u_int16_t;
#endif /* _UINT16_T */

#ifndef _UINT32_T
#define _UINT32_T
typedef unsigned int            _u_int32_t;
#endif /* _UINT32_T */

#ifndef _UINT64_T
#define _UINT64_T
typedef unsigned long long      _u_int64_t;
#endif /* _UINT64_T */


typedef void *                  tVar;
typedef char *                  tString;
typedef const char *            tCString;
typedef int                     tBool;
typedef const unsigned char     tByte;

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif



#endif //__TYPES_H