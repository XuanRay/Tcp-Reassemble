/*   file    : main.h
 *   author  : rayss
 *   date    : 2022.03.15
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : main.h                  
 */

#ifndef __MAIN_H
#define __MAIN_H


#include <pcap.h>
#include "./lib/types.h"

// use 0D0A0D0A0D0A as delimiter
#define REQUEST_GAP_LEN 6
#define REQUEST_GAP     "\r\n\r\n\r\n"

#define PCAP_DIR        "pcaps"
#define REQS_DIR        "reqts"
#define HTTP_DIR        "files"

tCString pcap_path;
tCString reqs_path;
tCString http_path;

typedef struct {
    struct pcap_pkthdr header;
    tByte *packet;
} pcap_item;


#endif //__MAIN_H