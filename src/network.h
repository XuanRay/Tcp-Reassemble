/*   file    : network.h
 *   author  : rayss
 *   date    : 2022.03.14
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : network.h                   
 */


#ifndef __NETWORK_H
#define __NETWORK_H


#include "./lib/types.h"
#include <stdio.h>

#undef CHUNK
#define CHUNK 16384

// the type code of the pcap_packet in an ETHERNET header
#define ETHER_TYPE_IP4     0x0800
#define ETHER_TYPE_IP6     0x86DD
#define ETHER_TYPE_8021Q   0x8100

// the offset value of the pcap_packet (in byte number)
#define ETHER_OFFSET_IP4   14
#define ETHER_OFFSET_IP6   14
#define ETHER_OFFSET_8021Q 18

// protocol code
#define PROTOCOL_IP4       AF_INET
#define PROTOCOL_IP6       AF_INET6
#define PROTOCOL_TCP       0x06
#define PROTOCOL_UDP       0x11

#define INET_ADDRSTRLEN    16     /* 255.255.255.255\0  4*4=16 */
#define INET6_ADDRSTRLEN   46     /* <netinet/in.h>  https://www.cnblogs.com/shenlinken/p/10546269.html */

#ifndef _SOCKLEN_T
#define _SOCKLEN_T
typedef unsigned int socklen_t;
#endif /* _SOCKLEN_T */

typedef _u_int32_t in_addr_t;
struct _in_addr {
    in_addr_t s_addr;
};

struct _in6_addr {
    union
      {
    _u_int8_t __u6_addr8[16];
    _u_int16_t __u6_addr16[8];
    _u_int32_t __u6_addr32[4];
      } __in6_u;
#define s6_addr        __in6_u.__u6_addr8
#define s6_addr16      __in6_u.__u6_addr16
#define s6_addr32      __in6_u.__u6_addr32
};

#define AF_INET     2
#define AF_INET6    10

extern tBool is_little_endian();

#undef htonl
extern _u_int32_t htonl(_u_int32_t hostlong);

#undef htons
extern _u_int16_t htons(_u_int16_t hostshort);

#undef ntohl
extern _u_int32_t ntohl(_u_int32_t netlong);

#undef ntohs
extern _u_int16_t ntohs(_u_int16_t netshort);


/* n:numeric  p:presentation 将数值转换从xxx.xxx.xxx.xxx形式 */
extern const char *_inet_ntop(int af, const void *src, char *dst, size_t size);


#endif //__NETWORK_H