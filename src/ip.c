/*   file    : ip.c
 *   author  : rayss
 *   date    : 2022.03.15
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : ip layer                   
 */

#include <stdio.h>
#include <string.h>
#include "./lib/util.h"
#include "ip.h"

tByte *get_ip_header(tByte *pcap_packet)
{
    int ether_type = ((int)pcap_packet[12] << 8) | (int)pcap_packet[13];
    int offset;
    switch (ether_type) {
        case ETHER_TYPE_8021Q:
            offset = ETHER_OFFSET_8021Q;
            break;
        case ETHER_TYPE_IP4:
            offset = ETHER_OFFSET_IP4;
            break;
        case ETHER_TYPE_IP6:
            offset = ETHER_OFFSET_IP6;
            break;
        default:
            return NULL;
    }
    //skip past the Ethernet II header
    return pcap_packet + offset;
}

int get_ip_protocol(tByte *ip_header)
{
    char version = *ip_header;
    version = is_little_endian() ? ((version & 0xF0) >> 4) : (version & 0x0F);
    switch (version)
    {
        case 4: return PROTOCOL_IP4;
        case 6: return PROTOCOL_IP6;
        default: return 0;
    }
}

inline tBool is_ip4(int protocol)
{
    return protocol == PROTOCOL_IP4;
}

inline tBool is_ip6(int protocol)
{
    return protocol == PROTOCOL_IP6;
}

inline tBool is_ip(int protocol)
{
    return is_ip4(protocol) || is_ip6(protocol);
}

unsigned short get_ip_id(tByte *ip_header)
{
    int protocol = get_ip_protocol(ip_header);
    switch (protocol)
    {
        case PROTOCOL_IP4: return ntohs(IP4(ip_header)->ip_id);
        case PROTOCOL_IP6: return 0;
        default: return 0;
    }
}

tBool match_transport_protocol(tByte *ip_header, int protocol)
{
    int ip_protocol = get_ip_protocol(ip_header);
    switch (ip_protocol)
    {
        case PROTOCOL_IP4: return IP4(ip_header)->ip_p == protocol;
        case PROTOCOL_IP6: return IP6(ip_header)->ip6_nxt == protocol;
        default: return FALSE;
    }
}

inline tBool is_tcp(tByte *ip_header)
{
    return match_transport_protocol(ip_header, PROTOCOL_TCP);
}

inline tBool is_udp(tByte *ip_header)
{
    return match_transport_protocol(ip_header, PROTOCOL_UDP);
}

tByte *get_transport_layer_header(tByte *ip_header)
{
    int protocol = get_ip_protocol(ip_header);
    size_t header_len;
    switch (protocol)
    {
        case PROTOCOL_IP4:
            header_len = IP4(ip_header)->ip_hl * 4;
            break;
        case PROTOCOL_IP6:
            header_len = 40;
            break;
        default:
            return NULL;
    }
    return ip_header + header_len;
}

/*
 * return something like "192.168.137.1.80--192.168.137.233.8888"
 */
tCString get_ip_port_pair(tByte *ip_header)
{
    int addr_str_len;
    tVar ip_src;
    tVar ip_dst;
    int protocol = get_ip_protocol(ip_header);

    if (is_ip4(protocol))
    {
        addr_str_len = INET_ADDRSTRLEN;
        ip_src = &IP4(ip_header)->ip_src;
        ip_dst = &IP4(ip_header)->ip_dst;
    }
    else
    {
        addr_str_len = INET6_ADDRSTRLEN;
        ip_src = &IP6(ip_header)->ip6_src;
        ip_dst = &IP6(ip_header)->ip6_dst;
    }

    char buf_src[INET6_ADDRSTRLEN];
    char buf_dst[INET6_ADDRSTRLEN];
    _inet_ntop(protocol, ip_src, buf_src, addr_str_len);
    _inet_ntop(protocol, ip_dst, buf_dst, addr_str_len);

    tByte *upper_layer = get_transport_layer_header(ip_header);
    int port_src = ntohs(SPORT(upper_layer));
    int port_dst = ntohs(DPORT(upper_layer));

    // max port number in string takes 5 bytes
    tString str = mymalloc((addr_str_len + 5) * 2 + 5);
    sprintf(str, "%s" IP_PORT_DELIMITER_S "%d"
                      IP_PAIR_DELIMITER_S
                 "%s" IP_PORT_DELIMITER_S "%d",
            buf_src, port_src, buf_dst, port_dst);

    // replace all ':' to '.'
    for (int i = 0; * (str + i); i++)
        if (*(str + i) == ':')
            *(str + i) = '.';

    return (tCString)str;
}

tCString reverse_ip_port_pair(tCString ip_port_pair)
{
    tString pair2 = strchr(ip_port_pair, IP_PAIR_DELIMITER_C);
    tString pair1 = strndup(ip_port_pair, pair2 - ip_port_pair);
    return mystrcat(3, pair2 + 1, IP_PAIR_DELIMITER_S, pair1);
}

tBool is_same_ip_port(tByte *ip_header1, tByte *ip_header2)
{
    int protocol1 = get_ip_protocol(ip_header1);
    int protocol2 = get_ip_protocol(ip_header2);
    if (protocol1 != protocol2)
        return FALSE;

    size_t addr_len;
    if (is_ip4(protocol1))
    {
        addr_len = sizeof(IP4(ip_header1)->ip_src);
        if (memcmp(&IP4(ip_header1)->ip_src, &IP4(ip_header2)->ip_src, addr_len))
            return FALSE;
        if (memcmp(&IP4(ip_header1)->ip_dst, &IP4(ip_header2)->ip_dst, addr_len))
            return FALSE;
    }
    else
    {
        addr_len = sizeof(IP6(ip_header1)->ip6_src);
        if (memcmp(&IP6(ip_header1)->ip6_src, &IP6(ip_header2)->ip6_src, addr_len))
            return FALSE;
        if (memcmp(&IP6(ip_header1)->ip6_dst, &IP6(ip_header2)->ip6_dst, addr_len))
            return FALSE;
    }

    tByte *upper_layer1 = get_transport_layer_header(ip_header1);
    tByte *upper_layer2 = get_transport_layer_header(ip_header2);

    return (SPORT(upper_layer1) == SPORT(upper_layer2))
        && (DPORT(upper_layer1) == DPORT(upper_layer2));
}
