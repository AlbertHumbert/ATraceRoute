#ifndef TRACE_LIBRARY_H
#define TRACE_LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>

/* trace_recv_xxx函数返回值 */
#define TRACE_RESULT_TIMEOUT            -3
#define TRACE_RESULT_TIMEEXCEED         -2
#define TRACE_RESULT_UNREACH            -1

/* 探测报文类型 */
#define TRACE_TYPE_UDP      1
#define TRACE_TYPE_TCP      2
#define TRACE_TYPE_ICMP     3


#define EXIT_ERR    1

#define BUFSIZE     1500
#define ICMP_HLEN   8
#define UDP_HLEN    8
#define TRUE        1
#define FALSE       0


/* troptions默认值 */
#define DFLOPT_MAXTTL       30
#define DFLOPT_NQUERIES     3
#define DFLOPT_WAITTIME     10
#define DFLOPT_TYPE TRACE_TYPE_UDP

#define USAGE_NEWLINE   "\n"
#ifdef __cplusplus
extern "C" {
#endif


struct trace_options {
    int m_max_ttl;         /* -m */
    int q_queries;         /* -q */
    int w_wait_time;       /* -w */
};



void start( char *host, int max_ttl, int wait_time,
           void (*node_info_recv)( char *dest, char *cur_address, int ttl, int time,  char *msg));

void trace_icmp(void (*node_info_recv)( char *dest, char *cur_Address, int ttl, int time,  char *msg));

int trace_recv_icmp(int sockfd, int seq, struct timeval *tv, struct sockaddr *addr, socklen_t *addr_length);

uint16_t check_sum(uint16_t *addr, int len);

char * get_dest_host();

char *sock_2_host(const struct sockaddr *addr, socklen_t addr_len);

int sock_addr_cmp(const struct sockaddr *sa1, const struct sockaddr *sa2);

#ifdef __cplusplus
}
#endif
#endif

