#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <time.h>
#include <errno.h>
#include <linux/can/raw.h>

#define CAN_INTERFACE "vcan0"

#define SPEED_ID 0x100
#define RPM_ID   0x101
#define TEMP_ID  0x102

int main()
{
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    int speed = 0;
    int rpm = 0;
    int temperature = 0;

    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (sock < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    strcpy(ifr.ifr_name, CAN_INTERFACE);

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("Interface error");
        close(sock);
        return 1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind failed");
        close(sock);
        return 1;
    }
    struct can_filter filters[3];

    filters[0].can_id = SPEED_ID;
    filters[0].can_mask = CAN_SFF_MASK;

    filters[1].can_id = RPM_ID;
    filters[1].can_mask = CAN_SFF_MASK;

    filters[2].can_id = TEMP_ID;
    filters[2].can_mask = CAN_SFF_MASK;

    setsockopt(sock,
           SOL_CAN_RAW,
           CAN_RAW_FILTER,
           &filters,
           sizeof(filters));

    printf("Dashboard ECU started.\n");

    while (1)
    {
        int nbytes = read(sock, &frame, sizeof(frame));

        if (nbytes < 0)
        {
            perror("Read failed");
            break;
        }

        if (frame.can_id == SPEED_ID)
        {
            speed = frame.data[0] |
                    (frame.data[1] << 8);
        }

        else if (frame.can_id == RPM_ID)
        {
            rpm = frame.data[0] |
                  (frame.data[1] << 8);
        }

        else if (frame.can_id == TEMP_ID)
        {
            temperature = frame.data[0] |
                          (frame.data[1] << 8);
        }
        else
        {
             printf("Unknown CAN ID: 0x%03X\n",
                                 frame.can_id);
        }
        /*
         * Clear terminal and display dashboard.
         */
        printf("\033[2J");
        printf("\033[H");

        printf("--------------------------------\n");
        printf("        Vehicle Dashboard       \n");
        printf("--------------------------------\n");
        printf("Speed       : %d km/h\n", speed);
        printf("Engine RPM  : %d rpm\n", rpm);
        printf("Temperature : %d C\n", temperature);
        printf("--------------------------------\n");
    }

    close(sock);

    return 0;
}
