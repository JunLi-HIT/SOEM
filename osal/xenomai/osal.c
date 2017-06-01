/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */


#include <native/task.h>
#include <native/timer.h>

#include <unistd.h>
#include <osal.h>

typedef SRTIME NANO_TIME;

int osal_usleep (uint32 usec)
{
   RTIME ticks = usec * 1000LL;
   rt_task_sleep( rt_timer_ns2ticks(ticks) );
   return 0;
}

ec_timet osal_current_time (void)
{
   NANO_TIME current_time;
   ec_timet return_value;

   current_time = rt_timer_ticks2ns(rt_timer_read());

   return_value.sec = current_time / 1000000000LL;
   return_value.usec = (current_time % 1000000000LL) / 1000LL;
   return return_value;
}

uint64_t osal_current_time_ns (void)
{
   return rt_timer_ticks2ns(rt_timer_read());
}

void osal_time_diff(ec_timet *start, ec_timet *end, ec_timet *diff)
{
   if (end->usec < start->usec) {
      diff->sec = end->sec - start->sec - 1;
      diff->usec = end->usec + 1000000 - start->usec;
   }
   else {
      diff->sec = end->sec - start->sec;
      diff->usec = end->usec - start->usec;
   }
}

void osal_timer_start (osal_timert * self, uint32 timeout_usec)
{
   NANO_TIME start_time;
   NANO_TIME stop_time;

   start_time = rt_timer_ticks2ns(rt_timer_read());
   stop_time = start_time + (timeout_usec * 1000LL);

   self->stop_time.sec = stop_time / 1000000000LL;
   self->stop_time.usec = (stop_time % 1000000000LL) / 1000LL;
}

boolean osal_timer_is_expired (osal_timert * self)
{
   NANO_TIME current_time;
   NANO_TIME stop_time;

   current_time = rt_timer_ticks2ns(rt_timer_read());
   stop_time = self->stop_time.sec * 1000000000LL + self->stop_time.usec * 1000LL;

   return (current_time >= stop_time);
}


int osal_thread_create(void *thandle, int stacksize, void *func, void *param)
{
    return 0;
}


int osal_thread_create_rt(void *thandle, int stacksize, void *func, void *param)
{
    return 0;
}




