/***
 *
 *  examples/xenomai/posix/raw-ethernet.c
 *
 *  SOCK_RAW sender - sends out Ethernet frames via a SOCK_RAW packet socket
 *
 *  Copyright (C) 2006 Jan Kiszka <jan.kiszka@web.de>
 *
 *  RTnet - real-time networking example
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h>

char buffer[1514];
int sock;


void catch_signal(int sig)
{
    close(sock);
}


int main(int argc, char *argv[])
{
    struct sched_param param = { .sched_priority = 1 };
    ssize_t len;
    struct sockaddr_ll addr;
    struct ifreq ifr;
    struct timespec delay = { 1, 0 };
    struct ether_header *eth = (struct ether_header *)buffer;


    signal(SIGTERM, catch_signal);
    signal(SIGINT, catch_signal);
    signal(SIGHUP, catch_signal);
    mlockall(MCL_CURRENT|MCL_FUTURE);

    if (argc < 2) {
        printf("usage: %s <interface>\n", argv[0]);
        return 0;
    }

    if ((sock = socket(PF_PACKET, SOCK_RAW, htons(0x1234))) < 0) {
        perror("socket cannot be created");
        return 1;
    }

    strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("cannot get interface index");
        close(sock);
        return 1;
    }

    addr.sll_family   = AF_PACKET;
    addr.sll_protocol = htons(0x1234);
    addr.sll_ifindex  = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("cannot bind to local ip/port");
        close(sock);
        return 1;
    }

    memset(eth->ether_dhost, 0xFF, ETH_HLEN);
    eth->ether_type = htons(0x1234);

    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    while (1) {
        buffer[0] = 0xFF; buffer[1] = 0xFF; buffer[2] = 0xFF;
        buffer[3] = 0xFF; buffer[4] = 0xFF; buffer[5] = 0xFF;
        buffer[6] = 0x68; buffer[7] = 0x05; buffer[8] = 0xCA;
        buffer[9] = 0x3A; buffer[10] = 0x4A; buffer[11] = 0x25;
        buffer[12] = 0x88; buffer[13] = 0x84;

        buffer[14] = 0x11; buffer[15] = 0x22; buffer[42] = 0x48;

        // len = send(sock, buffer, sizeof(buffer), 0);
        len = send(sock, buffer, 30, 0);
        if (len < 0)
            break;

        printf("Sent frame of %zd bytes\n", len);

        nanosleep(&delay, NULL);
    }

    /* This call also leaves primary mode, required for socket cleanup. */
    printf("shutting down\n");

    /* Note: The following loop is no longer required since Xenomai 2.4. */
    while ((close(sock) < 0) && (errno == EAGAIN)) {
        printf("socket busy - waiting...\n");
        sleep(1);
    }

    return 0;
}
