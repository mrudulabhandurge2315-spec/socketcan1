#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main()
{
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct canfd_frame frame;

    int enable_canfd = 1;

    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (sock < 0)
    {
        perror("Socket");
        return 1;
    }

    if (setsockopt(sock,
                   SOL_CAN_RAW,
                   CAN_RAW_FD_FRAMES,
                   &enable_canfd,
                   sizeof(enable_canfd)) < 0)
    {
        perror("CAN FD socket option");
        return 1;
    }

    strcpy(ifr.ifr_name, "vcan0");

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("Interface");
        return 1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock,
             (struct sockaddr *)&addr,
             sizeof(addr)) < 0)
    {
        perror("Bind");
        return 1;
    }

    memset(&frame, 0, sizeof(frame));

    frame.can_id = 0x300;
    frame.len = 20;

    for (int i = 0; i < frame.len; i++)
    {
        frame.data[i] = i + 1;
    }

    if (write(sock,
              &frame,
              CANFD_MTU) != CANFD_MTU)
    {
        perror("CAN FD transmission failed");
        return 1;
    }

    printf("CAN FD frame transmitted.\n");
    printf("CAN ID : 0x300\n");
    printf("Length : %d bytes\n", frame.len);

    close(sock);

    return 0;
}
