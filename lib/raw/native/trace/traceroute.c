#include "traceroute.h"

#include <stdio.h>

void start(void (*nodeInfoRecv)(char *dest, char *curAddres, int ttl, int time)) {
    for (int i = 0; i < 10; i++) {
        nodeInfoRecv("111", "222", i, 222);
    };
}