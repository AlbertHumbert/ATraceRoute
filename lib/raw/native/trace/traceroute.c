
/* traceroute.c */
#include <sys/time.h>
#include "traceroute.h"


struct trace_options trace_options;

struct sockaddr_in dest_addr;
char *dest_host_name;

char send_buf[BUFSIZE], recv_buf[BUFSIZE];
struct hostent *hostent;

//in_addr 32位无符号整形表示的ip地址
in_addr_t saddr;

static int alarm_flag;

char *get_dest_host() {
    return dest_host_name;
}

void start(char *host, int max_ttl, int wait_time,
           void (*node_info_recv)(char *dest, char *cur_address, int ttl, int time, char *msg)) {


    trace_options.m_max_ttl = max_ttl;
    trace_options.q_queries = DFLOPT_NQUERIES;
    trace_options.w_wait_time = wait_time;


    dest_host_name = host;
    printf("trace %s", host);

    if ((saddr = inet_addr(dest_host_name)) == INADDR_NONE) {
        if ((hostent = gethostbyname(dest_host_name)) == NULL) {
            //printf("unknown host %s \n", host);
            node_info_recv(dest_host_name, NULL, 0, 0, "unknown host ");
            return;

        }
        memmove(&saddr, hostent->h_addr, hostent->h_length);
    }
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = saddr;


    trace_icmp(node_info_recv);


}

char *sock_2_host(const struct sockaddr *addr, socklen_t addr_len) {
    static char str[64];

    switch (addr->sa_family) {
        case AF_INET: {
            struct sockaddr_in *sin = (struct sockaddr_in *) addr;
            if (inet_ntop(AF_INET, &sin->sin_addr,
                          str, sizeof(str)) == NULL)
                return NULL;
            break;
        }

        case AF_INET6: {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) addr;
            if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
                return NULL;
            break;
        }
    }

    return str;
}


int sock_addr_cmp(const struct sockaddr *sa1, const struct sockaddr *sa2) {
    if (sa1->sa_family != sa2->sa_family)
        return -1;

    switch (sa1->sa_family) {
        case AF_INET:
            return memcmp(&((struct sockaddr_in *) sa1)->sin_addr,
                          &((struct sockaddr_in *) sa2)->sin_addr,
                          sizeof(struct in_addr));

        case AF_INET6:
            return memcmp(&((struct sockaddr_in6 *) sa1)->sin6_addr,
                          &((struct sockaddr_in6 *) sa2)->sin6_addr,
                          sizeof(struct in6_addr));
    }

    return -1;
}


/*
 *  计算检验和
 */

uint16_t check_sum(uint16_t *addr, int len) {
    int left = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    while (left > 1) {
        sum += *w++;
        left -= 2;
    }

    if (left == 1) {
        *(unsigned char *) (&answer) = *(unsigned char *) w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}


void handle_alrm(int sign) {
    printf("\ttime-out");
    fflush(stdout);
    alarm_flag = TRUE;
    return;
}


void trace_icmp(void (*node_info_recv)(char *dest, char *curAddres, int ttl, int time, char *errMsg)) {
    int sockfd;
    struct sockaddr addr, lastaddr;
    socklen_t addrlen;

    int rtt;
    int has_name_info;
    int icmpdatalen, seq, ttl, query, code, ndone;
    struct timeval tvsend, tvrecv;
    struct icmp *icmp;
    size_t len;

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("scoket error");
        exit(EXIT_ERR);
    }
    setuid(getuid());


    printf("traceroute to %s (%s), %d hops max (ICMP) \n",
           dest_host_name,
           inet_ntoa(dest_addr.sin_addr),
           trace_options.m_max_ttl);

    has_name_info = FALSE;
    icmpdatalen = 56;
    seq = 0;
    ndone = 0;

    //外部用信号经常没办法停下recvfrom函数，设一个秒级别的超时可以解决
    //todo 也就是说只设这个超时就能解决了？
    struct timeval begin;
    begin.tv_sec = 0;
    begin.tv_usec = 10000;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &begin, sizeof(begin));

    for (ttl = 1; ttl <= trace_options.m_max_ttl && ndone == 0; ttl++) {
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

        printf("%3d", ttl);
        fflush(stdout);

        bzero(&lastaddr, sizeof(lastaddr));
        for (query = 0; query < trace_options.q_queries; query++) {
            ++seq;

            gettimeofday(&tvsend, NULL);

            icmp = (struct icmp *) send_buf;
            icmp->icmp_type = ICMP_ECHO;
            icmp->icmp_code = 0;
            icmp->icmp_id = htons(getpid());
            icmp->icmp_seq = htons(seq);
            memset(icmp->icmp_data, 0xa5, icmpdatalen);
            memcpy(icmp->icmp_data, &tvsend, sizeof(struct timeval));


            len = ICMP_HLEN + icmpdatalen;
            icmp->icmp_cksum = 0;
            icmp->icmp_cksum = check_sum((u_short *) icmp, len);

            if (sendto(sockfd, send_buf, len, 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
                perror("sendto error");
                exit(EXIT_ERR);
            }

            code = trace_recv_icmp(sockfd, seq, &tvrecv, &addr, &addrlen);
            if (code == TRACE_RESULT_TIMEOUT) {
                printf("\t*");
                node_info_recv(dest_host_name, NULL, ttl, -1, "connection time-out");
            } else {
                char str[NI_MAXHOST];
                memset(str, 0, sizeof(str));

                if (sock_addr_cmp(&lastaddr, &addr) != 0) {
                    if (getnameinfo(&addr, addrlen, str, sizeof(str), NULL, 0, 0) == 0) {
                        printf("\t%s (%s)",
                               str,
                               sock_2_host(&addr, addrlen));
                       has_name_info = TRUE;
                    } else {
                        printf("\t%s",
                               sock_2_host(&addr, addrlen));
                        has_name_info = FALSE;
                    }

                    memcpy(&lastaddr, &addr, addrlen);
                }

                if ((tvrecv.tv_usec -= tvsend.tv_usec) < 0) {
                    --tvrecv.tv_sec;
                    tvrecv.tv_usec += 1000000;
                }
                tvrecv.tv_sec -= tvsend.tv_sec;

                rtt = tvrecv.tv_usec;

                printf("\t%.3f ms", rtt / 1000.0);

                char *msg = (ttl == trace_options.m_max_ttl && query == trace_options.q_queries - 1) ? "finish" : NULL;
                if (strlen(str) == 0) {
                    node_info_recv(dest_host_name, NULL, ttl, rtt, msg);
                } else {
                    if (has_name_info) {
                        strcat(str, "(");
                        strcat(str, sock_2_host(&addr, addrlen));
                        strcat(str, ")");
                    }
                    node_info_recv(dest_host_name, str, ttl, rtt, msg);
                }

                if (code == TRACE_RESULT_UNREACH)
                    ++ndone;
            }
        }


        printf("\n");
    }
}


int trace_recv_icmp(int sockfd, int seq, struct timeval *tv, struct sockaddr *addr, socklen_t *addr_length) {

    struct ip *ip1, *ip2;
    struct icmp *icmp, *icmp2;
    int ip_header_length1, ip_header_length2, icmp_length;
    int return_val, n;
    struct sigaction act;


    act.sa_handler = handle_alrm;
    act.sa_flags = SA_NODEFER;
    sigaction(SIGALRM, &act, NULL);

    alarm(trace_options.w_wait_time);

    alarm_flag = FALSE;
    for (;;) {
        if (alarm_flag) {
            return_val = TRACE_RESULT_TIMEOUT;
            break;
        }

        n = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, addr, addr_length);

        if (n < 0 && n != -1) {
            if (errno == EINTR)
                continue;
            else {
                perror("recv error");
                exit(EXIT_ERR);
            }
        }


        ip1 = (struct ip *) recv_buf;
        ip_header_length1 = ip1->ip_hl << 2;

        icmp = (struct icmp *) (recv_buf + ip_header_length1);
        if ((icmp_length = n - ip_header_length1) < ICMP_HLEN)
            continue;

        if (icmp->icmp_type == ICMP_TIMXCEED
            && icmp->icmp_code == ICMP_TIMXCEED_INTRANS) {

            if (icmp_length < ICMP_HLEN + sizeof(struct ip))
                continue;

            ip2 = (struct ip *) (recv_buf + ip_header_length1 + ICMP_HLEN);
            ip_header_length2 = ip2->ip_hl << 2;

            if (icmp_length < ICMP_HLEN + ip_header_length2 + ICMP_HLEN)
                continue;

            icmp2 = (struct icmp *) (recv_buf + ip_header_length1 + ICMP_HLEN + ip_header_length2);
            if (icmp2->icmp_type == ICMP_ECHO
                && icmp2->icmp_code == 0
                && icmp2->icmp_id == htons(getpid())
                && icmp2->icmp_seq == htons(seq)) {
                return_val = TRACE_RESULT_TIMEEXCEED;
                break;
            }

        } else if (icmp->icmp_type == ICMP_ECHOREPLY) {

            if (icmp->icmp_id == htons(getpid())
                && icmp->icmp_seq == htons(seq)) {
                return_val = TRACE_RESULT_UNREACH;
                break;
            }
        }

    }
    alarm(0);
    gettimeofday(tv, NULL);
    return return_val;
}