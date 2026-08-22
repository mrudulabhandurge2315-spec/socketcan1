#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <net/if.h>
#include <linux/can.h>
#include <time.h>

#define CAN_INTERFACE "vcan0"

#define SPEED_ID 0x100
#define RPM_ID   0x101
#define TEMP_ID  0x102

#define TIMEOUT_SECONDS 2

int main()
{
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    int speed = 0;
    int rpm = 0;
    int temperature = 0;

    time_t last_speed_time = time(NULL);

    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (sock < 0)
    {
        perror("Socket");
        return 1;
    }

    strcpy(ifr.ifr_name, CAN_INTERFACE);

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

    printf("Dashboard ECU with diagnostics started.\n");

    while (1)
    {
        fd_set readfds;

        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        struct timeval timeout;

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int result = select(sock + 1,
                            &readfds,
                            NULL,
                            NULL,
                            &timeout);

        if (result > 0)
        {
            if (FD_ISSET(sock, &readfds))
            {
                int nbytes =
                    read(sock, &frame, sizeof(frame));

                if (nbytes == sizeof(frame))
                {
                    if (frame.can_id == SPEED_ID)
                    {
                        speed =
                            frame.data[0] |
                            (frame.data[1] << 8);

                        last_speed_time = time(NULL);
                    }

                    else if (frame.can_id == RPM_ID)
                    {
                        rpm =
                            frame.data[0] |
                            (frame.data[1] << 8);
                    }

                    else if (frame.can_id == TEMP_ID)
                    {
                        temperature =
                            frame.data[0] |
                            (frame.data[1] << 8);
                    }

                    else
                    {
                        printf("Unknown CAN ID: 0x%03X\n",
                               frame.can_id);
                    }
                }
            }
        }

        time_t current_time = time(NULL);

        int elapsed =
            (int)difftime(current_time,
                          last_speed_time);

        printf("\033[2J");
        printf("\033[H");

        printf("--------------------------------\n");
        printf("        Vehicle Dashboard       \n");
        printf("--------------------------------\n");

        printf("Speed       : %d km/h\n", speed);
        printf("Engine RPM  : %d rpm\n", rpm);
        printf("Temperature : %d C\n", temperature);

        printf("--------------------------------\n");

        if (elapsed >= TIMEOUT_SECONDS)
        {
            printf("WARNING: Vehicle ECU Offline\n");
        }
        else
        {
            printf("Vehicle ECU : ONLINE\n");
        }

        printf("--------------------------------\n");
    }

    close(sock);

    return 0;
}
