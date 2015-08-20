#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    /************************************************************/
    /* Windows 独自の設定 */
    WSADATA data;
    WSAStartup(MAKEWORD(2,0), &data);

    SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);

    // create addr
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    //addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = INADDR_ANY;
    //addr.sin_addr.s_addr = INADDR_LOOPBACK;

    // bind socket
    bind(_socket, (struct sockaddr *)&addr, sizeof(addr));

    /* 接続の許可 */
    //listen(_socket, 1);

    printf("befor getsockname()->%d\n", ntohs(addr.sin_port));
    int len = sizeof(addr);
    getsockname(_socket, (struct sockaddr *)&addr, &len); // if i comment this func, the last printf() will print 0; if not, it will print a real in use udp port(and it is correct!)
    printf("after getsockname()->%d\n", ntohs(addr.sin_port));

    closesocket(_socket);

    /* Windows 独自の設定 */
    WSACleanup();

    return 0;
}
