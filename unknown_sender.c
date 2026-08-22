#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>

int main()
{
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (sock < 0)
    {
        perror("socket");
        return 1;
    }

    strcpy(ifr.ifr_name, "vcan0");

    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    frame.can_id = 0x200;
    frame.can_dlc = 4;

    frame.data[0] = 0xAA;
    frame.data[1] = 0xBB;
    frame.data[2] = 0xCC;
    frame.data[3] = 0xDD;

    write(sock, &frame, sizeof(frame));

    printf("Unknown CAN message transmitted.\n");
    printf("ID: 0x200\n");

    close(sock);

    return 0;
}
