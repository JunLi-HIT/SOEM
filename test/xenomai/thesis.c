/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test.
 *
 * (c)Arthur Ketels 2010 - 2011
 */

#include <signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "ethercat.h"

#define EC_TIMEOUTMON 500
#define numOfCycles 100000

char IOmap[4096];
OSAL_THREAD_HANDLE thread1;
int expectedWKC;
boolean needlf;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;
struct timespec tstart, tend;
double tdiff_ns;
struct timespec tr0, tr1;
double tr_sum_us = 0.0;
double trd, tr_min = 1000, tr_max = 0;
FILE *fid;
double trall[numOfCycles];


void set_output_int32 (uint16 slave_no, uint8 module_index, int32 value)
{
   uint8 *data_ptr;
   /* Get the IO map pointer from the ec_slave struct */
   data_ptr = ec_slave[slave_no].outputs;
   /* Move pointer to correct module index */
   data_ptr += module_index * 4;
   /* Read value byte by byte since all targets can handle misaligned
    * addresses
    */
   *data_ptr++ = (value >> 0) & 0xFF;
   *data_ptr++ = (value >> 8) & 0xFF;
   *data_ptr++ = (value >> 16) & 0xFF;
   *data_ptr++ = (value >> 24) & 0xFF;
}

void set_output_int8 (uint16 slave_no, uint8 module_index, int8 value)
{
   uint8 *data_ptr;
   /* Get the IO map pointer from the ec_slave struct */
   data_ptr = ec_slave[slave_no].outputs;
   /* Move pointer to correct module index */
   data_ptr += module_index;
   /* Read value byte by byte since all targets can handle misaligned
    * addresses
    */
   *data_ptr++ = (value >> 0) & 0xFF;
}

void simpletest(char *ifname)
{
   int i, j, s, oloop, iloop, chk;
   int myled = 0;
   
   needlf = FALSE;
   inOP = FALSE;

   printf("Starting simple test\n");

   /* initialise SOEM, bind socket to ifname */
   if (ec_init(ifname))
   {
      printf("ec_init on %s succeeded.\n",ifname);
      /* find and auto-config slaves */

      if ( ec_config_init(FALSE) > 0 )
      {
         printf("%d slaves found and configured.\n",ec_slavecount);

         ec_config_map(&IOmap);

         ec_configdc();

         printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
         if (oloop > 8) oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
         if (iloop > 8) iloop = 8;

         printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments,
                ec_group[0].IOsegment[0],
                ec_group[0].IOsegment[1],
                ec_group[0].IOsegment[2],
                ec_group[0].IOsegment[3]);

         printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_OPERATIONAL;

         // block LWR
         ec_group[0].blockLRW = 1;

         
         /* send one valid process data to make outputs in slaves happy*/
         ec_send_processdata();
         ec_receive_processdata(EC_TIMEOUTRET);
         /* request OP state for all slaves */
         ec_writestate(0);
         chk = 40;
         /* wait for all slaves to reach OP state */
         do
         {
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
         }
         while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
         if (ec_slave[0].state == EC_STATE_OPERATIONAL )
         {
            printf("Operational state reached for all slaves.\n");
            inOP = TRUE;
            
            clock_gettime(CLOCK_MONOTONIC, &tstart);
            
            /* cyclic loop */
            for(i = 1; i <= numOfCycles; i++)
            {
               ec_send_processdata();

               clock_gettime(CLOCK_MONOTONIC, &tr0);
               wkc = ec_receive_processdata(EC_TIMEOUTRET);
               clock_gettime(CLOCK_MONOTONIC, &tr1);

               trd = (tr1.tv_sec - tr0.tv_sec) * 1.0 * 1e6 + (tr1.tv_nsec - tr0.tv_nsec) * 0.001;
               tr_sum_us += trd;
               if (trd > tr_max) {tr_max = trd;}
               if (trd < tr_min) {tr_min = trd;}
               trall[i-1] = trd;

               /* if(wkc >= expectedWKC) */
               /* { */
               /*    printf("Processdata cycle %4d, WKC %d , O:", i, wkc); */

               /*    for(j = 0 ; j < oloop; j++) */
               /*    { */
               /*       printf(" %2.2x", *(ec_slave[0].outputs + j)); */
               /*    } */

               /*    printf(" I:"); */
               /*    for(j = 0 ; j < iloop; j++) */
               /*    { */
               /*       printf(" %2.2x", *(ec_slave[0].inputs + j)); */
               /*    } */
               /*    printf(" T:%"PRId64"\r",ec_DCtime); */
               /*    needlf = TRUE; */
               /* } */

               for (s = 1; s <= ec_slavecount; s++) {
                  set_output_int32(s, 0, myled);
                  set_output_int32(s, 1, myled);
                  set_output_int32(s, 2, myled);
                  set_output_int32(s, 3, myled);
               }

               /* set_output_int8(1, 0, myled); */
                 

               if (i%1000 == 0) {myled = !myled;}
               
               if (wkc < 0) {
                  printf("Error: wkc is smaller than 0");
               }
               // osal_usleep(5000);
            }
            inOP = FALSE;


            clock_gettime(CLOCK_MONOTONIC, &tend);

            printf("tstsrt sec  = %d\n", (int)tstart.tv_sec);
            printf("tstsrt nsec = %d\n", (int)tstart.tv_nsec);
            printf("tend   sec  = %d\n", (int)tend.tv_sec);
            printf("tend   nsec = %d\n", (int)tend.tv_nsec);
            
            tdiff_ns = (tend.tv_sec - tstart.tv_sec) * 1.0 * 1e9 + (tend.tv_nsec - tstart.tv_nsec);
            printf("tdiff  usec = %f\n", tdiff_ns/1000.0/numOfCycles);
            printf("tsend  usec = %f\n", tdiff_ns/1000.0/numOfCycles - tr_sum_us/numOfCycles);
            printf("trall  usec = %f\n", tr_sum_us/numOfCycles);
            printf("trmin  usec = %f\n", tr_min);
            printf("trmax  usec = %f\n", tr_max);
         }
         else
         {
            printf("Not all slaves reached operational state.\n");
            ec_readstate();
            for(i = 1; i<=ec_slavecount ; i++)
            {

               if(ec_slave[i].state != EC_STATE_OPERATIONAL)
               {
                  printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                         i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
               }
            }
         }
         printf("\nRequest init state for all slaves\n");
         ec_slave[0].state = EC_STATE_INIT;
         /* request INIT state for all slaves */
         ec_writestate(0);
      }
      else
      {
         printf("No slaves found!\n");
      }
      printf("End simple test, close socket\n");
      /* stop SOEM, close socket */
      ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n",ifname);
    }
}

OSAL_THREAD_FUNC ecatcheck( void *ptr )
{
    int slave;

    while(1)
    {
        if( inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
        {
            if (needlf)
            {
               needlf = FALSE;
               printf("\n");
            }
            /* one ore more slaves are not responding */
            ec_group[currentgroup].docheckstate = FALSE;
            ec_readstate();
            for (slave = 1; slave <= ec_slavecount; slave++)
            {
               if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
               {
                  ec_group[currentgroup].docheckstate = TRUE;
                  if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
                  {
                     printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                     ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state == EC_STATE_SAFE_OP)
                  {
                     printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                     ec_slave[slave].state = EC_STATE_OPERATIONAL;
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state > EC_STATE_NONE)
                  {
                     if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d reconfigured\n",slave);
                     }
                  }
                  else if(!ec_slave[slave].islost)
                  {
                     /* re-check state */
                     ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                     if (ec_slave[slave].state == EC_STATE_NONE)
                     {
                        ec_slave[slave].islost = TRUE;
                        printf("ERROR : slave %d lost\n",slave);
                     }
                  }
               }
               if (ec_slave[slave].islost)
               {
                  if(ec_slave[slave].state == EC_STATE_NONE)
                  {
                     if (ec_recover_slave(slave, EC_TIMEOUTMON))
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d recovered\n",slave);
                     }
                  }
                  else
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d found\n",slave);
                  }
               }
            }
            if(!ec_group[currentgroup].docheckstate)
               printf("OK : all slaves resumed OPERATIONAL.\n");
        }
        osal_usleep(10000);
    }
}

int main(int argc, char *argv[])
{
   size_t i = 0;
   struct sched_param param = { .sched_priority = 40 };
   fid = fopen("tmp.csv", "w");
    
   printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

   mlockall(MCL_CURRENT|MCL_FUTURE);

   pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

   if (argc > 1)
   {
      /* create thread to handle slave error handling in OP */
      // osal_thread_create(&thread1, 128000, &ecatcheck, (void*) &ctime);
      
      /* start cyclic part */
      simpletest(argv[1]);
   }
   else
   {
      printf("Usage: simple_test ifname1\nifname = eth0 for example\n");
   }

   // log data to file in csv format
   for (i = 0; i < numOfCycles; i++) {
      fprintf(fid, "%f\n", trall[i]);
   }

   fclose(fid);
   printf("End program\n");
   return (0);
}
