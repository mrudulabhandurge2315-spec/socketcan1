#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <sys/time.h>

#define CAN_INTERFACE "vcan0"

int main()
{
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    FILE *logfile;

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

    logfile = fopen("can_log.csv", "a");

    if (logfile == NULL)
    {
        perror("Log file error");
        close(sock);
        return 1;
    }

    fprintf(logfile, "Timestamp,CAN_ID,DLC,Data\n");
    fflush(logfile);

    printf("Logger ECU started.\n");
    printf("Logging to can_log.csv\n");

    while (1)
    {
        int nbytes = read(sock, &frame, sizeof(frame));

        if (nbytes < 0)
        {
            perror("Read failed");
            break;
        }

        struct timeval tv;

        gettimeofday(&tv, NULL);

        printf("[%ld.%06ld] ID=0x%03X DLC=%d Data=",
               tv.tv_sec,
               tv.tv_usec,
               frame.can_id & CAN_SFF_MASK,
               frame.can_dlc);

        fprintf(logfile,
                "%ld.%06ld,0x%03X,%d,",
                tv.tv_sec,
                tv.tv_usec,
                frame.can_id & CAN_SFF_MASK,
                frame.can_dlc);

        for (int i = 0; i < frame.can_dlc; i++)
        {
            printf("%02X ", frame.data[i]);
            fprintf(logfile, "%02X", frame.data[i]);

            if (i < frame.can_dlc - 1)
                fprintf(logfile, " ");
        }

        printf("\n");

        fprintf(logfile, "\n");

        fflush(logfile);
    }

    fclose(logfile);
    close(sock);

    return 0;
}
