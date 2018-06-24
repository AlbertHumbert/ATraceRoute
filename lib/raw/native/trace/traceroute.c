#include <sys/time.h>
#include "traceroute.h"
#include "trace_def.h"

char *sock_ntop_host(const struct sockaddr *addr, socklen_t addrlen);

int sock_addr_cmp(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen);

uint16_t check_sum(uint16_t *addr, int len);

int icmp_recv(int sockfd, int seq, struct timeval *tv, struct sockaddr *addr, socklen_t *addrlen);

void sig_alrm(int signo);


void setTask(const char *dest, int ttl, int queries, int time) {

    in_addr_t addr;
    struct hostent *hostent;

    //设置选项值
    if (ttl <= 0) {
        options.m_max_TTL = DEFAUTL_MAX_TTL;
    } else {
        options.m_max_TTL = ttl;
    }

    if (queries <= 0) {
        options.q_queries = DEFAUTL_QUERIES;
    } else {
        options.q_queries = queries;
    }

    if (time <= 0) {
        options.w_wait_time = DEFAULT_WAIT_TIME;
    } else {
        options.w_wait_time = time;
    }

    // 如果输入的是点分十进制的ip地址，转换为长整数型
    if ((addr = inet_addr(dest)) == INADDR_NONE) {
        //否则将域名转换为ip地址
        if ((hostent = gethostbyname(dest)) == NULL) {
            fprintf(stderr, "unknown host %s \n", dest);
            exit(1);
        }
        memmove(&addr, hostent->h_addr, hostent->h_length);
    }

    // 缓存和构造目的主机信息
    dest_host = dest;
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = addr;
}

void start(void (*nodeInfoRecv)(const char *dest, char *curAddres, int ttl, int time, char *errMsg)) {
    int sockfd;
    struct sockaddr addr, last_addr;
    socklen_t addrlen;

    double rtt;
    int icmpdatalen, seq, ttl, query, code, finish;
    struct timeval tvsend, tvrecv;
    struct icmp *icmp;
    size_t len;

    /* 创建ICMP的套接字 */
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("create socket failed");
        exit(1);
    }

    //在 *nix 下创建 icmp 似乎需要root权限
    setuid(getuid());

    printf("traceroute to %s (%s), %d hops max (ICMP) \n", dest_host, inet_ntoa(dest_addr.sin_addr), options.m_max_TTL);

    icmpdatalen = 56;
    seq = 0;
    finish = 0;
    //以max_ttl为最大上限访问节点
    for (ttl = 1; ttl <= options.m_max_TTL && finish == 0; ttl++) {
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
        printf("%3d", ttl);
        fflush(stdout);
        bzero(&last_addr, sizeof(last_addr));
        //以queries为最大上限发送和接收icmp报文
        for (query = 0; query < options.q_queries; query++) {
            seq++;
            gettimeofday(&tvsend, NULL);

            //创建echo报文,数据部分为时间戳，空白部分以0xa5填充
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

     /*       printf("bsendto");
            fflush(stdout);*/
            //发送报文
            if (sendto(sockfd, send_buf, len, 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
                perror("sendto error");
                exit(1);
            }
     /*       printf("sendto");
            fflush(stdout);*/
            code = icmp_recv(sockfd, seq, &tvrecv, &addr, &addrlen);
/*            printf("recv");*/
            if (code == TRACE_RESULT_TIMEOUT) {
                printf("\t*");
            } else {
                char str[NI_MAXHOST];
                if (sock_addr_cmp(&last_addr, &addr, addrlen) != 0) {
                    if (getnameinfo(&addr, addrlen, str, sizeof(str), NULL, 0, 0) == 0) {
                        printf("\t%s (%s)",
                               str,
                               sock_ntop_host(&addr, addrlen));
                    } else {
                        printf("\t%s",
                               sock_ntop_host(&addr, addrlen));

                    }

                    memcpy(&last_addr, &addr, addrlen);
                }

                if ((tvrecv.tv_usec -= tvsend.tv_usec) < 0) {
                    --tvrecv.tv_sec;
                    tvrecv.tv_usec += 1000000;
                }
                tvrecv.tv_sec -= tvsend.tv_sec;

                rtt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec / 1000.0;

                printf("\t%.3f ms", rtt);

                nodeInfoRecv(dest_host, sock_ntop_host(&addr, addrlen), ttl, rtt, NULL);

                if (code == TRACE_RESULT_UNREACH)
                    ++finish;
            }
        }

        printf("\n");
    }
}

int icmp_recv(int sockfd, int seq, struct timeval *tv, struct sockaddr *addr, socklen_t *addrlen) {
    struct ip *ip1, *ip2;
    struct icmp *icmp, *icmp2;
    int iphlen1, iphlen2, icmplen, ret, n;
    struct sigaction act;

    /* 设置SIGALRM信号处理函数 */
    sigemptyset(&act.sa_mask);
    act.sa_handler = sig_alrm;
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);

    alarm(options.w_wait_time);

    alarm_flag = FALSE;
    for (;;) {
        if (alarm_flag) {
            ret = TRACE_RESULT_TIMEOUT;
            break;
        }

        n = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, addr, addrlen);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            else {
                perror("recv error");
                exit(1);
            }
        }


        /* 处理接收到的数据 */
        ip1 = (struct ip *) recv_buf;
        iphlen1 = ip1->ip_hl << 2;

        icmp = (struct icmp *) (recv_buf + iphlen1);
        if ((icmplen = n - iphlen1) < ICMP_HLEN)
            continue;

        if (icmp->icmp_type == ICMP_TIMXCEED
            && icmp->icmp_code == ICMP_TIMXCEED_INTRANS) {

            if (icmplen < ICMP_HLEN + sizeof(struct ip))
                continue;

            ip2 = (struct ip *) (recv_buf + iphlen1 + ICMP_HLEN);
            iphlen2 = ip2->ip_hl << 2;

            if (icmplen < ICMP_HLEN + iphlen2 + ICMP_HLEN)
                continue;

            icmp2 = (struct icmp *) (recv_buf + iphlen1 + ICMP_HLEN + iphlen2);
            if (icmp2->icmp_type == ICMP_ECHO
                && icmp2->icmp_code == 0
                && icmp2->icmp_id == htons(getpid())
                && icmp2->icmp_seq == htons(seq)) {
                ret = TRACE_RESULT_TIME_EXCEED;
                break;
            }

        } else if (icmp->icmp_type == ICMP_ECHOREPLY) {

            if (icmp->icmp_id == htons(getpid())
                && icmp->icmp_seq == htons(seq)) {
                ret = TRACE_RESULT_UNREACH;
                break;
            }
        }

    }
    alarm(0);
    gettimeofday(tv, NULL);

    return ret;

}


/*
 *  SIGALRM信号处理函数
 */
void sig_alrm(int signo) {
    alarm_flag = TRUE;
    return;
}

/*
 *  计算检验和
 */
uint16_t check_sum(uint16_t *addr, int len) {
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *(unsigned char *) (&answer) = *(unsigned char *) w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

/*
 *  比较两个套接字地址结构是否相同
 *      若相同，返回0
 */
int sock_addr_cmp(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen) {
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
 *  套接字地址结构地址部分转字符串
 *  若成功，返回转换后的字符串，若出错，返回NULL
 */
char *sock_ntop_host(const struct sockaddr *addr, socklen_t addrlen) {
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
