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
        perror("CAN FD option");
        return 1;
    }

    strcpy(ifr.ifr_name, "vcan0");

    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(sock,
         (struct sockaddr *)&addr,
         sizeof(addr));

    printf("CAN FD Receiver started.\n");

    while (1)
    {
        int nbytes =
            read(sock,
                 &frame,
                 sizeof(frame));

        if (nbytes < 0)
        {
            perror("Read");
            break;
        }

        printf("Received CAN FD frame\n");
        printf("CAN ID: 0x%03X\n",
               frame.can_id);

        printf("Length: %d bytes\n",
               frame.len);

        printf("Data: ");

        for (int i = 0; i < frame.len; i++)
        {
            printf("%02X ",
                   frame.data[i]);
        }

        printf("\n\n");
    }

    close(sock);

    return 0;
}
