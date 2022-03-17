/*   file    : tcp.h
 *   author  : rayss
 *   date    : 2022.03.14
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : tcp.h                   
 */


#ifndef __TCP_H
#define __TCP_H


#include "ip.h"

typedef _u_int32_t tcp_seq;

typedef struct {
    _u_int16_t th_sport;     /* source port */
    _u_int16_t th_dport;     /* destination port */
    tcp_seq th_seq;          /* sequence number */
    tcp_seq th_ack;          /* acknowledgement number */
// #if __BYTE_ORDER == __LITTLE_ENDIAN
    _u_int16_t th_x2:4,       /* (unused) */
              th_off:4,      /* data offset */
              flag_fin:1,    /* flags */
              flag_syn:1,
              flag_rst:1,
              flag_psh:1,
              flag_ack:1,
              flag_urg:1,
              flag_ece:1,
              flag_cwr:1;
    _u_int16_t th_win;       /* window */
    _u_int16_t th_sum;       /* checksum */
    _u_int16_t th_urp;       /* urgent pointer */
} tcp_hdr;

#define th_off(th)  (is_little_endian() ? (th)->th_off : (th)->th_x2)
#define tcp_fin(th)  (is_little_endian() ? (th)->flag_fin : (th)->flag_cwr)
#define tcp_syn(th)  (is_little_endian() ? (th)->flag_syn : (th)->flag_ece)
#define tcp_rst(th)  (is_little_endian() ? (th)->flag_rst : (th)->flag_urg)
#define tcp_psh(th)  (is_little_endian() ? (th)->flag_psh : (th)->flag_ack)
#define tcp_ack(th)  (is_little_endian() ? (th)->flag_ack : (th)->flag_psh)
#define tcp_urg(th)  (is_little_endian() ? (th)->flag_urg : (th)->flag_rst)
#define tcp_ece(th)  (is_little_endian() ? (th)->flag_ece : (th)->flag_syn)
#define tcp_cwr(th)  (is_little_endian() ? (th)->flag_cwr : (th)->flag_fin)

extern tcp_hdr *get_tcp_header(tByte *ip_header);
extern size_t get_tcp_data_length(tByte *ip_header);
extern tByte *get_tcp_data(tcp_hdr *tcp_header);


#endif //__TCP_H

