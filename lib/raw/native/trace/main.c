/* main.c */
#include "traceroute.h"

//接收节点信息
void nodeInfoRecv(char *dest, char *curAddress, int ttl, int time,  char *msg) {
    if (msg!=NULL &&strcmp("finish", "finish") == 0) {
    }

}
int main(int argc, char *argv[]){
    start("www.baidu.com",16,5,nodeInfoRecv);
    return 0;
}