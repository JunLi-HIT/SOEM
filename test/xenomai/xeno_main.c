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
#include "ethercat.h"


char buffer[1514];
int sock;


void catch_signal(int sig)
{
    printf("signal catched");
    close(ecx_context.port->sockhandle);
}

int main(int argc, char *argv[])
{
    struct sched_param param = { .sched_priority = 1 };
    ssize_t len;
    struct sockaddr_ll addr;
    struct ifreq ifr;
    struct timespec delay = { 1, 0 };
    struct ether_header *eth = (struct ether_header *)buffer;
    pthread_attr_t       attr;
    int stacksize = 1024;
    int i = 0;
    struct timespec tstart, tend;
    double tdiff_us;
    ec_timet ec_tstart, ec_tend, ec_tdiff;

    printf("sock handle = %d\n", ecx_context.port->sockhandle);

#if defined(__KERNEL__) || defined(__XENO_SIM__)
    printf("test some defined ");
#endif


    signal(SIGTERM, catch_signal);
    signal(SIGINT, catch_signal);
    signal(SIGHUP, catch_signal);
    mlockall(MCL_CURRENT|MCL_FUTURE);

    ec_init("rteth0");

    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stacksize);

    /* while (1) { */
    /*     buffer[0] = 0xFF; buffer[1] = 0xFF; buffer[2] = 0xFF; */
    /*     buffer[3] = 0xFF; buffer[4] = 0xFF; buffer[5] = 0xFF; */
    /*     buffer[6] = 0x68; buffer[7] = 0x05; buffer[8] = 0xCA; */
    /*     buffer[9] = 0x3A; buffer[10] = 0x4A; buffer[11] = 0x25; */
    /*     buffer[12] = 0x88; buffer[13] = 0x84; */

    /*     buffer[14] = 0x11; buffer[15] = 0x22; buffer[42] = 0x48; */

    /*     // len = send(sock, buffer, sizeof(buffer), 0); */
    /*     len = send(ecx_context.port->sockhandle, buffer, 30, 0); */
    /*     if (len < 0) */
    /*         break; */

    /*     // printf("Sent frame of %zd bytes\n", len); */


    /*     nanosleep(&delay, NULL); */
    /* } */
    

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    ec_tstart = osal_current_time();

    for (i = 0; i < 1000; i++)
    {
       buffer[0] = 0xFF; buffer[1] = 0xFF; buffer[2] = 0xFF;
       buffer[3] = 0xFF; buffer[4] = 0xFF; buffer[5] = 0xFF;
       buffer[6] = 0x68; buffer[7] = 0x05; buffer[8] = 0xCA;
       buffer[9] = 0x3A; buffer[10] = 0x4A; buffer[11] = 0x25;
       buffer[12] = 0x88; buffer[13] = 0x84;

       buffer[14] = (i%256); buffer[15] = 0x11; buffer[16] = 0x22;
       buffer[42] = 0x48;

       len = send(ecx_context.port->sockhandle, buffer, 42, 0);
    }

    clock_gettime(CLOCK_MONOTONIC, &tend);
    ec_tend = osal_current_time();


    // print time difference
    printf("tstsrt sec  = %d\n", (int)tstart.tv_sec);
    printf("tstsrt nsec = %d\n", (int)tstart.tv_nsec);
    printf("tend   sec  = %d\n", (int)tend.tv_sec);
    printf("tend   nsec = %d\n", (int)tend.tv_nsec);

    tdiff_us = 1.0 * (double)(tend.tv_sec - tstart.tv_sec) * 1000000.0 + (tend.tv_nsec - tstart.tv_nsec) * 0.001;
    printf("tdiff  usec = %f\n", 1.0 * (double)(tend.tv_sec - tstart.tv_sec));


    osal_time_diff(&ec_tstart, &ec_tend, &ec_tdiff);
    printf("tstsrt sec  = %d\n", (int)ec_tstart.sec);
    printf("tstsrt usec = %d\n", (int)ec_tstart.usec);
    printf("tend   sec  = %d\n", (int)ec_tend.sec);
    printf("tend   usec = %d\n", (int)ec_tend.usec);
    printf("tdiff  sec  = %d\n", (int)ec_tdiff.sec);
    printf("tdiff  usec = %d\n", (int)ec_tdiff.usec);

    /* This call also leaves primary mode, required for socket cleanup. */
    printf("shutting down\n");
    
    close(ecx_context.port->sockhandle);

    return 0;
}
