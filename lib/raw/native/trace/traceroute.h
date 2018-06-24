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

#ifdef __cplusplus
extern "C"{
#endif

/**
 * 设置traceroute主机任务
 * @param dest 目的主机ip或域名
 * @param ttl 最大ttl
 * @param queries 每个节点发送的探测报文个数
 * @param time 最大等待时间
 */
void setTask(const char *dest, int ttl, int queries, int time);

/**
 * 开始任务
 * @param nodeInfoRecv 接受节点信息的函数
 */
void start(void (*nodeInfoRecv)(const char *dest, char *curAddres, int ttl, int time, char *errMsg));

#ifdef __cplusplus
}
#endif
#endif

