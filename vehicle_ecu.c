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
#include <math.h>

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

    /* Create CAN RAW socket */
    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (sock < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    /* Get interface index */
    strcpy(ifr.ifr_name, CAN_INTERFACE);

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("SIOCGIFINDEX failed");
        close(sock);
        return 1;
    }

    /* Bind socket to vcan0 */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind failed");
        close(sock);
        return 1;
    }

    printf("Vehicle ECU started on %s\n", CAN_INTERFACE);

    double speed = 0;
    double temperature = 70;

    int direction = 1;

    while (1)
    {
        /*
         * Simulate realistic vehicle speed.
         */
        speed += direction * 2;

        if (speed >= 120)
        {
            speed = 120;
            direction = -1;
        }

        if (speed <= 0)
        {
            speed = 0;
            direction = 1;
        }

        /*
         * Approximate engine RPM based on speed.
         */
        int rpm = 800 + (int)(speed * 35);

        if (rpm > 5000)
            rpm = 5000;

        /*
         * Simulate coolant temperature.
         */
        if (speed > 30)
            temperature += 0.1;
        else
            temperature -= 0.05;

        if (temperature > 100)
            temperature = 100;

        if (temperature < 20)
            temperature = 20;

        /*
         * -----------------------------
         * SPEED MESSAGE
         * CAN ID = 0x100
         * -----------------------------
         */

        memset(&frame, 0, sizeof(frame));

        frame.can_id = SPEED_ID;
        frame.can_dlc = 2;

        uint16_t speed_value = (uint16_t)speed;

        frame.data[0] = speed_value & 0xFF;
        frame.data[1] = (speed_value >> 8) & 0xFF;

        if (write(sock, &frame, sizeof(frame)) != sizeof(frame))
        {
            perror("Speed transmission failed");
        }

        /*
         * -----------------------------
         * RPM MESSAGE
         * CAN ID = 0x101
         * -----------------------------
         */

        memset(&frame, 0, sizeof(frame));

        frame.can_id = RPM_ID;
        frame.can_dlc = 2;

        uint16_t rpm_value = (uint16_t)rpm;

        frame.data[0] = rpm_value & 0xFF;
        frame.data[1] = (rpm_value >> 8) & 0xFF;

        if (write(sock, &frame, sizeof(frame)) != sizeof(frame))
        {
            perror("RPM transmission failed");
        }

        /*
         * -----------------------------
         * TEMPERATURE MESSAGE
         * CAN ID = 0x102
         * -----------------------------
         */

        memset(&frame, 0, sizeof(frame));

        frame.can_id = TEMP_ID;
        frame.can_dlc = 2;

        uint16_t temp_value = (uint16_t)temperature;

        frame.data[0] = temp_value & 0xFF;
        frame.data[1] = (temp_value >> 8) & 0xFF;

        if (write(sock, &frame, sizeof(frame)) != sizeof(frame))
        {
            perror("Temperature transmission failed");
        }

        printf("TX -> Speed: %.0f km/h | RPM: %d | Temperature: %.1f C\n",
               speed, rpm, temperature);

        /*
         * Wait 500 ms.
         */
        usleep(100000);
    }

    close(sock);

    return 0;
}
