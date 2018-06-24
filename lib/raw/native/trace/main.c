//
// Created by Albert Humbert on 18/6/24.
//

#include "traceroute.h"

void nodeInfoRecv(const char *dest, char *curAddress, int ttl, int time, char *errMsg) {
}

void main(void) {
    setTask("www.baidu.com", 30, 3, 3);
    start(recv);
}

