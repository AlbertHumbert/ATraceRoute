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

void start(void (*nodeInfoRecv)(char *dest, char *curAddres, int ttl, int time));
#ifdef __cplusplus
}
#endif
#endif

