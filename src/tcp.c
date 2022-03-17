/*   file    : tcp.c
 *   author  : rayss
 *   date    : 2022.03.14
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : tcp的实现                  
 */


#include "tcp.h"



inline tcp_hdr *get_tcp_header(tByte *ip_header)
{
    return (tcp_hdr *)get_transport_layer_header(ip_header);
}



size_t get_tcp_data_length(tByte *ip_header)
{
    size_t ip_len;
    size_t ip_header_len;

    int protocol = get_ip_protocol(ip_header);
    if (is_ip4(protocol))
    {
        ip_len = ntohs(IP4(ip_header)->ip_len);
        ip_header_len = IP4(ip_header)->ip_hl * 4;
    }
    else if (is_ip6(protocol))
    {
        ip_len = ntohs(IP6(ip_header)->ip6_plen);
        ip_header_len = 0;
    }
    else
        return 0;
    // `th_off` specifies the size of the TCP header in 32-bit words
    size_t tcp_header_len = th_off(get_tcp_header(ip_header)) * 4;
    return ip_len - (ip_header_len + tcp_header_len);
}



inline tByte *get_tcp_data(tcp_hdr *tcp_header)
{
    return (tByte *)((tString)(tcp_header) + th_off(tcp_header) * 4);
}