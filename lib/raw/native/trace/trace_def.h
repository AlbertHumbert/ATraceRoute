//
// Created by Albert Humbert on 18/6/24.
//

#ifndef TRACE_TRACE_DEF_H
#define TRACE_TRACE_DEF_H

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


/**
 * trace_options默认值
 * */
#define DEFAUTL_MAX_TTL             30
#define DEFAUTL_QUERIES             3
#define DEFAULT_WAIT_TIME           3


#define TRACE_RESULT_TIMEOUT        3
#define TRACE_RESULT_TIME_EXCEED    -2
#define TRACE_RESULT_UNREACH       -1

#define BUFSIZE     1500
#define ICMP_HLEN   8
#define FALSE       0
#define TRUE        1

struct trace_options {
    int m_max_TTL;         /* -m */
    int q_queries;         /* -q */
    int w_wait_time;       /* -w */
};

struct trace_options options;
struct sockaddr_in dest_addr;

uint16_t s_port, d_port;
const char *dest_host;
char send_buf[BUFSIZE], recv_buf[BUFSIZE];
int alarm_flag;                     //闹钟标记
#endif //TRACE_TRACE_DEF_H
