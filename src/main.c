/*   file    : main.c
 *   author  : rayss
 *   date    : 2022.03.15
 *   ------------------------------------
 *   blog    : https://cnblogs.com/rayss
 *   github  : https://github.com/XuanRay
 *   mail    : xuanlei@seu.edu.cn
 *   ------------------------------------
 *   description : 主程序                   
 */


/*
 * TCP-Reassemble work
 *
 * A pcap file structure for tcp transaction is something like this:
 *     [pcap_file_header]
 *     for each packet
 *         [pcap_packet]  this contains packet len info
 *         [ip_header]    usually of size 20 or more
 *         [tcp_header]   usually of size 20 or more
 *         [tcp_data]     len stored in ip header
 */
#include <stdio.h>
#include <string.h>
#include "./lib/mydirent.h"
#include "./lib/file.h"
#include "./lib/util.h"
#include "./lib/hash.h"
#include "ip.h"
#include "tcp.h"
#include "main.h"


static inline tByte *get_ip_header_n(HASHNODE *node)
{
    return get_ip_header(((pcap_item *)node->data)->packet);
}

static inline tcp_hdr *get_tcp_header_p(tByte *pcap_packet)
{
    return get_tcp_header(get_ip_header(pcap_packet));
}

static inline tcp_hdr *get_tcp_header_n(HASHNODE *node)
{
    return get_tcp_header_p(((pcap_item *)node->data)->packet);
}

// return beginning memory address of tcp data
static inline tByte *get_tcp_data_n(HASHNODE *node)
{
    return (tByte *)((tString)(get_tcp_header_n(node)) + th_off(get_tcp_header_n(node)) * 4);
}

static inline size_t get_tcp_data_length_p(tByte *pcap_packet)
{
    return get_tcp_data_length(get_ip_header(pcap_packet));
}

static inline size_t get_tcp_data_length_n(HASHNODE *node)
{
    return get_tcp_data_length_p(((pcap_item *)node->data)->packet);
}

// hash operation
void free_pcap_item(tVar ptr)
{
    pcap_item *pcap = (pcap_item *)ptr;
    safe_free(pcap->packet);
    safe_free(pcap);
}

/*
 * a pcap item contains a pcap header and a pointer of beignning of packet
 */
pcap_item *create_pcap_item(tByte *pcap_packet, struct pcap_pkthdr *pcap_header)
{
    size_t pcap_len = pcap_header->caplen;
    u_char *tmp_packet = mymalloc(pcap_len);
    memcpy(tmp_packet, pcap_packet, pcap_len);

    pcap_item *pcap = mymalloc(sizeof(pcap_item));
    pcap->header = *pcap_header;
    pcap->packet = tmp_packet;

    return pcap;
}

/*
 * use (source ip:port, destination ip:port) as key, hash pcap_item
 */
tCString hash_pcap_item(tByte *pcap_packet, struct pcap_pkthdr *pcap_header)
{
    tByte *ip_header = get_ip_header(pcap_packet);
    tCString key = get_ip_port_pair(ip_header);
    pcap_item *pcap = create_pcap_item(pcap_packet, pcap_header);

    if ((size_t)(-1) == insert_hash_node(key, (tVar)pcap))
        myerror("insert to hash table failed");
    return key;
}

pcap_t *get_pcap_handle(tCString filename)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    if (!(handle = pcap_open_offline(filename, errbuf)))
        myerror("Couldn't open pcap file %s: %s", filename, errbuf);
    return handle;
}


int cmp_pcap_packet(pcap_item *p1, pcap_item *p2)
{
    const u_char *packet1 = p1->packet;
    const u_char *packet2 = p2->packet;
    tcp_hdr *t1 = get_tcp_header_p(packet1);
    tcp_hdr *t2 = get_tcp_header_p(packet2);

    int diff_seq = ntohl(t1->th_seq) - ntohl(t2->th_seq);
    int diff_ack = ntohl(t1->th_ack) - ntohl(t2->th_ack);

    return diff_seq ? diff_seq : diff_ack;
}

/*
 * sort pcap packets storging in hash table
 */
HASHNODE *sort_pcap_packets(HASHNODE *list)
{
    HASHNODE *p, *q, *e, *tail;
    int insize, nmerges, psize, qsize, i;

    if (!list)
        return NULL;

    insize = 1;
    while (1)
    {
        p = list;
        list = NULL;
        tail = NULL;
        /* count number of merges we do in this pass */
        nmerges = 0;

        while (p)
        {
            /* there exists a merge to be done */
            nmerges++;
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++)
            {
                psize++;
                if (!(q = q->next))
                    break;
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while (psize > 0 || (qsize > 0 && q))
            {
                /* decide whether next element of merge comes from p or q */
                if (psize == 0)
                {
                    /* p is empty; e must come from q. */
                    e = q; q = q->next; qsize--;
                }
                else if (qsize == 0 || !q)
                {
                    /* q is empty; e must come from p. */
                    e = p; p = p->next; psize--;
                }
                else if (cmp_pcap_packet((pcap_item *)p->data, (pcap_item *)q->data) <= 0)
                {
                    /* First element of p is lower (or same);
                     * e must come from p. */
                    e = p; p = p->next; psize--;
                }
                else
                {
                    /* First element of q is lower; e must come from q. */
                    e = q; q = q->next; qsize--;
                }

                /* add the next element to the merged list */
                if (tail)
                    tail->next = e;
                else
                    list = e;

                tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }

        tail->next = NULL;

        /* If we have done only one merge, we're finished. */
        if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
            return list;
        /* Otherwise repeat, merging lists twice the size */
        insize *= 2;
    }
}

void classify_pcap_packets_in_hashtbl()
{
    HASHTBL *hashtbl = get_hash_table();
    HASHITR hashitr = hashtbl_iterator(hashtbl);
    while (hashtbl_next(&hashitr)) {
        size_t i = hashitr.cindex;
        hashtbl->nodes[i] = sort_pcap_packets(hashtbl->nodes[i]);
    }
}

void write_pcap_to_file(pcap_t *handle, HASHNODE *node, tCString dirpath)
{
    tCString filename = pathcat(dirpath, mystrcat(2, node->key, ".pcap"));
    pcap_dumper_t *pd;
    if (!(pd = pcap_dump_open(handle, filename)))
        myerror("create pcap file '%s' failed", filename);

    for (; node; node = node->next)
    {
        pcap_item *pcap = (pcap_item *)node->data;
        pcap_dump((u_char *)pd, &pcap->header, pcap->packet);
    }

    pcap_dump_close(pd);
    safe_free(filename);
}

void write_pcaps_to_files(pcap_t *handle, tCString dirpath)
{
    HASHITR hashitr = hashtbl_iterator(get_hash_table());
    HASHNODE *node;
    while (node = hashtbl_next(&hashitr))
        write_pcap_to_file(handle, node, dirpath);
}


size_t combine_tcp_nodes(tCString key1, tCString key2)
{
    HASHTBL *hashtbl = get_hash_table();
    size_t hash1 = get_hash_index(key1);
    size_t hash2 = get_hash_index(key2);
    HASHNODE *node1;
    HASHNODE *node2;

    // if one nodes is empty, then return another one
    if (hash1 == (size_t)(-1) || !(node1 = hashtbl->nodes[hash1]))
        return hash2;
    if (hash2 == (size_t)(-1) || !(node2 = hashtbl->nodes[hash2]))
        return hash1;

    // make sure of node1 being requester and node2 being responser
    tcp_hdr *t = get_tcp_header_n(node1);
    if (tcp_syn(t) && tcp_ack(t))
    {
        size_t tmp = hash1;
        hash1 = hash2;
        hash2 = tmp;
    }

    node2 = hashtbl->nodes[hash2];
    // exchange seq and ack, and append node2 to node1
    while (node2)
    {
        tcp_hdr *tcp_packet2 = get_tcp_header_n(node2);
        tcp_seq seq2 = tcp_packet2->th_seq;
        tcp_seq ack2 = tcp_packet2->th_ack;
        tcp_packet2->th_seq = ack2;
        tcp_packet2->th_ack = seq2;

        HASHNODE *next2 = node2->next;
        node2->next = hashtbl->nodes[hash1];
        hashtbl->nodes[hash1] = node2;
        node2 = next2;
    }

    hashtbl->nodes[hash1] = node1 = sort_pcap_packets(hashtbl->nodes[hash1]);
    // recovery seq and ack in original node2
    while (node1)
    {
        tByte *ip_header2 = get_ip_header_n(node1);
        tCString tmp = get_ip_port_pair(ip_header2);
        if (!strcmp(key2, tmp))
        {
            tcp_hdr *tcp_packet2 = get_tcp_header(ip_header2);
            tcp_seq seq2 = tcp_packet2->th_seq;
            tcp_seq ack2 = tcp_packet2->th_ack;
            tcp_packet2->th_seq = ack2;
            tcp_packet2->th_ack = seq2;
        }
        node1 = node1->next;
        safe_free(tmp);
    }

    node1 = hashtbl->nodes[hash1];
    hashtbl->nodes[hash1] = NULL;
    hashtbl->nodes[hash2] = NULL;
    hash1 = hashtbl->hashfunc(node1->key) % hashtbl->size;
    hashtbl->nodes[hash1] = node1;
    return hash1;
}

void tidy_tcp_packet_in_hashtbl()
{
    HASHITR hashitr = hashtbl_iterator(get_hash_table());
    HASHNODE *node;
    while (node = hashtbl_next(&hashitr)) {
        tCString key1 = node->key;
        tCString key2 = reverse_ip_port_pair(key1);
        combine_tcp_nodes(key1, key2);
    }
}


/*
 * check three handshake and return last handshake node
 */
HASHNODE *last_of_three_handshake(HASHNODE *head)
{
    HASHNODE *secnd = head->next;
    HASHNODE *three;
    if (!secnd || !(three = secnd->next))
        return NULL;

    tcp_hdr *t1 = get_tcp_header_n(head);
    tcp_hdr *t2 = get_tcp_header_n(secnd);
    tcp_hdr *t3 = get_tcp_header_n(three);

    // check flags
    if (!tcp_syn(t1))
        return NULL;
    if (!(tcp_syn(t2) && tcp_ack(t2)))
        return NULL;
    if (!tcp_ack(t3))
        return NULL;

    // check seq and ack
    if (!(ntohl(t2->th_ack) == ntohl(t1->th_seq) + 1))
        return NULL;
    if (!(t2->th_ack == t3->th_seq && ntohl(t3->th_ack) == ntohl(t2->th_seq) + 1))
        return NULL;

    return three;
}

void write_tcp_data_to_file(tCString filename, HASHNODE *head)
{
    HASHNODE *node = last_of_three_handshake(head);
    if (!node)
        return;

    FILE *fp = NULL;

    // ip header of tcp segment packet (one direction)
    tByte *ip_hdr1 = get_ip_header_n(node);
    // ip header of tcp segment packet (another direction)
    tByte *ip_hdr2 = NULL;
    // previous ip header of tcp segment packet
    tByte *pip_hdr = NULL;
    // next ip header to handle
    tByte *nip_hdr = NULL;
    // tcp header
    tcp_hdr *ptcp_pkt = NULL;
    tcp_hdr *ntcp_pkt = NULL;
    // last tcp header having tcp data
    tcp_hdr *ltcp_pkt = NULL;
    tcp_seq pack = 0;
    tcp_seq nack = 0;
    size_t plen  = 0;
    size_t nlen  = 0;
    // offset of tcp data
    size_t offset = 0;
    for (HASHNODE *next = node->next; node; next = node->next) {
        tByte *data     = get_tcp_data_n(node);
        size_t data_len = get_tcp_data_length_n(node);

        size_t writelen = data_len - offset;
        if (writelen > 0) {
            // delay openning
            if (!fp)
                fp = safe_fopen(filename, "w");
            // write data into file
            if (writelen != fwrite(data + offset, 1, writelen, fp))
                logging("Can't write all tcp bytes to '%s'", filename);
            // remember last tcp header having data
            ltcp_pkt = get_tcp_header_n(node);
        }
        // if no more tcp data to write
        if (!next)
            break;

        nip_hdr = get_ip_header_n(next);
        // which direction should we do judge
        if (is_same_ip_port(nip_hdr, ip_hdr1))
            pip_hdr = ip_hdr1;
        // may be start of a new tcp segment, so we record the ip header for next time judging
        else if (!ip_hdr2)
            ip_hdr2 = nip_hdr;
        else if (is_same_ip_port(nip_hdr, ip_hdr2))
            pip_hdr = ip_hdr2;

        if (pip_hdr) {
            ptcp_pkt = get_tcp_header(pip_hdr);
            pack = ptcp_pkt->th_ack;
            plen = get_tcp_data_length(pip_hdr);
        }
        ntcp_pkt = get_tcp_header(nip_hdr);
        nack = ntcp_pkt->th_ack;
        nlen = get_tcp_data_length(nip_hdr);
        // if next node is a tcp segment node
        if (nip_hdr != pip_hdr && nack == pack) {
            size_t pseq = ntohl(ptcp_pkt->th_seq);
            size_t nseq = ntohl(ntcp_pkt->th_seq);
            offset = pseq + plen;

            /*
             *       plen
             *   ------------
             *   | tcp data |
             *   ------------
             * pseq    ----------------
             *         |   tcp data   |
             *         ----------------
             *       nseq
             */
            if (offset >= nseq + nlen)
                offset = nlen;
            else if (offset > nseq)
                offset -= nseq;
            else
                offset = 0;

            // record the last ip header containing a tcp segment
            if (pip_hdr == ip_hdr1)
                ip_hdr1 = nip_hdr;
            else
                ip_hdr2 = nip_hdr;
        } else {
            offset = 0;
        }

        // if next tcp packet isn't a new tcp segment of last tcp packet,
        // then write a delimiter for distinguish different parts
        if (fp && nlen && ltcp_pkt->th_ack != nack)
            fwrite(REQUEST_GAP, 1, REQUEST_GAP_LEN, fp);

        node = next;
    }

    safe_fclose(fp);
}

void write_tcp_data_to_files(tCString dirpath)
{
    HASHITR hashitr = hashtbl_iterator(get_hash_table());
    HASHNODE *head;
    while (head = hashtbl_next(&hashitr)) {
        tCString basename = mystrcat(2, head->key, ".txt");
        tCString filename = pathcat(dirpath, basename);

        write_tcp_data_to_file(filename, head);

        safe_free(filename);
        safe_free(basename);
    }
}




pcap_t *init_environment(int argc, char **argv)
{
    tCString path;
    tCString dir;
#ifdef DEBUG
    path = "test.pcap";
#else
    if (argc < 2)
        myerror("usage: %s [file]", argv[0]);
    path = argv[1];
#endif
    dir = getdirname(path);
    pcap_path = pathcat(dir, PCAP_DIR);
    reqs_path = pathcat(dir, REQS_DIR);
    //http_path = pathcat(dir, HTTP_DIR);
    removedir(pcap_path);
    removedir(reqs_path);
    //removedir(http_path);
    makedir(pcap_path);
    makedir(reqs_path);
    makedir(http_path);
    set_hash_freefunc(free_pcap_item);
    return get_pcap_handle(path);
}

void reset_environment(pcap_t *handle)
{
    pcap_close(handle);
    safe_free(pcap_path);
    safe_free(reqs_path);
    safe_free(http_path);
}

void hash_all_pcap_item(pcap_t *handle)
{
    struct pcap_pkthdr header;
    tByte *pk;
    for (pk = pcap_next(handle, &header); pk != NULL; pk = pcap_next(handle, &header))
    {
        tByte *ip_header = get_ip_header(pk);
        // skip if neither IPv4 nor IPv6
        if (NULL == ip_header)
            continue;
        if (is_tcp(ip_header))
            hash_pcap_item(pk, &header);
    }
}

int main(int argc, char **argv)
{
    pcap_t *handle = init_environment(argc, argv);
    hash_all_pcap_item(handle);

    classify_pcap_packets_in_hashtbl();
    write_pcaps_to_files(handle, pcap_path);

    tidy_tcp_packet_in_hashtbl();
    write_tcp_data_to_files(reqs_path);
    empty_hash_table();

    //write_http_data_to_dirs(reqs_path, http_path);
    // No data to be free
    set_hash_freefunc(NULL);
    destroy_hash_table();

    reset_environment(handle);
    return 0;
}