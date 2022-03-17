/*   file    : ip.h
 *   author  : rayss
 *   date    : 2022.03.15
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : xxx                   
 */

#ifndef __IP_H
#define __IP_H

#include "network.h"

typedef struct {
// little-endian
    _u_int32_t ip_hl:4;             /* header length */
    _u_int32_t ip_v:4;              /* version */
    _u_int8_t ip_tos;               /* type of service */
    _u_int16_t ip_len;              /* total length */
    _u_int16_t ip_id;               /* identification */
    _u_int16_t ip_off;              /* fragment offset field */
#define IP_RF 0x8000                /* reserved fragment flag */
#define IP_DF 0x4000                /* dont fragment flag */
#define IP_MF 0x2000                /* more fragments flag */
#define IP_OFFMASK 0x1fff           /* mask for fragmenting bits */
    _u_int8_t ip_ttl;               /* time to live */
    _u_int8_t ip_p;                 /* protocol */
    _u_int16_t ip_sum;              /* checksum */
    struct _in_addr ip_src, ip_dst;  /* source and dest address */
} ip4_hdr;

typedef struct {
    union
      {
    struct ip6_hdrctl
      {
        _u_int32_t ip6_un1_flow;      /* 4 bits version, 8 bits TC, 20 bits flow-ID */
        _u_int16_t ip6_un1_plen;      /* payload length */
        _u_int8_t  ip6_un1_nxt;       /* next header */
        _u_int8_t  ip6_un1_hlim;      /* hop limit */
      } ip6_un1;
    _u_int8_t ip6_un2_vfc;            /* 4 bits version, top 4 bits tclass */
      } ip6_ctlun;
    struct _in6_addr ip6_src;        /* source address */
    struct _in6_addr ip6_dst;        /* destination address */
} ip6_hdr;

#define ip6_vfc   ip6_ctlun.ip6_un2_vfc
#define ip6_flow  ip6_ctlun.ip6_un1.ip6_un1_flow
#define ip6_plen  ip6_ctlun.ip6_un1.ip6_un1_plen
#define ip6_nxt   ip6_ctlun.ip6_un1.ip6_un1_nxt
#define ip6_hlim  ip6_ctlun.ip6_un1.ip6_un1_hlim
#define ip6_hops  ip6_ctlun.ip6_un1.ip6_un1_hlim
#define ip4_v(x) (is_little_endian() ? (x)->ip_v : (x)->ip_hl)
#define ip4_hl(x) (is_little_endian() ? (x)->ip_hl : (x)->ip_v)
#define SPORT(upper_layer) (*(unsigned short *)(upper_layer))
#define DPORT(upper_layer) (*(unsigned short *)((upper_layer) + 2))
#define IP4(x) ((ip4_hdr *)(x))
#define IP6(x) ((ip6_hdr *)(x))

#define IP_PORT_DELIMITER_S "."
#define IP_PORT_DELIMITER_C (*IP_PORT_DELIMITER_S)
#define IP_PAIR_DELIMITER_S "-"
#define IP_PAIR_DELIMITER_C (*IP_PAIR_DELIMITER_S)

extern tBool is_ip4(int protocol);
extern tBool is_ip6(int protocol);
extern tBool is_ip(int protocol);
extern tBool is_tcp(tByte *ip_header);
extern tBool is_udp(tByte *ip_header);
extern tBool is_same_ip_port(tByte *ip_header1, tByte *ip_header2);
extern tBool match_transport_protocol(tByte *ip_header, int protocol);
extern int get_ip_protocol(tByte *ip_header);
extern unsigned short get_ip_id(tByte *ip_header);
extern tByte *get_ip_header(tByte *pcap_packet);
extern tByte *get_transport_layer_header(tByte *ip_header);
extern tCString get_ip_port_pair(tByte *ip_header);
extern tCString reverse_ip_port_pair(tCString ip_port_pair);




#endif //__IP_H