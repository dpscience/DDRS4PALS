/********************************************************************\

  Name:         rb.c
  Created by:   Stefan Ritt

  $Id: rb.cpp 21437 2014-07-30 14:13:29Z ritt $

\********************************************************************/

#include <stdio.h>
#ifdef OS_DARWIN
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <string.h>
#include <assert.h>

#include "rb.h"

/********************************************************************\
*                                                                    *
*                 Ring buffer functions                              *
*                                                                    *
* Provide an inter-thread buffer scheme for handling front-end       *
* events. This code allows concurrent data acquisition, calibration  *
* and network transfer on a multi-CPU machine. One thread reads      *
* out the data, passes it vis the ring buffer functions              *
* to another thread running on the other CPU, which can then         *
* calibrate and/or send the data over the network.                   *
*                                                                    *
\********************************************************************/

typedef struct {
   unsigned char *buffer;
   unsigned int size;
   unsigned int max_event_size;
   unsigned char *rp;
   unsigned char *wp;
   unsigned char *ep;
} RING_BUFFER;

#define MAX_RING_BUFFER 100
RING_BUFFER rb[MAX_RING_BUFFER];

volatile int _rb_nonblocking = 0;

extern void ss_sleep(int ms);

int rb_set_nonblocking()
/********************************************************************\

  Routine: rb_set_nonblocking

  Purpose: Set all rb_get_xx to nonblocking. Needed in multi-thread
           environments for stopping all theads without deadlock

  Input:
    NONE

  Output:
    NONE

  Function value:
    RB_SUCCESS       Successful completion

\********************************************************************/
{
   _rb_nonblocking = 1;

   return RB_SUCCESS;
}

int rb_create(int size, int max_event_size, int *handle)
/********************************************************************\

  Routine: rb_create

  Purpose: Create a ring buffer with a given size

  Input:
    int size             Size of ring buffer, must be larger than
                         2*max_event_size
    int max_event_size   Maximum event size to be placed into
                         ring buffer
  Output:
    int *handle          Handle to ring buffer

  Function value:
    DB_SUCCESS           Successful completion
    DB_NO_MEMORY         Maximum number of ring buffers exceeded
    DB_INVALID_PARAM     Invalid event size specified

\********************************************************************/
{
   int i;

   for (i = 0; i < MAX_RING_BUFFER; i++)
      if (rb[i].buffer == NULL)
         break;

   if (i == MAX_RING_BUFFER)
      return RB_NO_MEMORY;

   if (size < max_event_size * 2)
      return RB_INVALID_PARAM;

   memset(&rb[i], 0, sizeof(RING_BUFFER));
   rb[i].buffer = (unsigned char *) malloc(size);
   assert(rb[i].buffer);
   rb[i].size = size;
   rb[i].max_event_size = max_event_size;
   rb[i].rp = rb[i].buffer;
   rb[i].wp = rb[i].buffer;
   rb[i].ep = rb[i].buffer;

   *handle = i + 1;

   return RB_SUCCESS;
}

int rb_delete(int handle)
/********************************************************************\

  Routine: rb_delete

  Purpose: Delete a ring buffer

  Input:
    none
  Output:
    int handle       Handle to ring buffer

  Function value:
    DB_SUCCESS       Successful completion

\********************************************************************/
{
   if (handle < 0 || handle >= MAX_RING_BUFFER || rb[handle - 1].buffer == NULL)
      return RB_INVALID_HANDLE;

   free(rb[handle - 1].buffer);
   memset(&rb[handle - 1], 0, sizeof(RING_BUFFER));

   return RB_SUCCESS;
}

int rb_get_wp(int handle, void **p, int millisec)
/********************************************************************\

Routine: rb_get_wp

  Purpose: Retrieve write pointer where new data can be written

  Input:
     int handle               Ring buffer handle
     int millisec             Optional timeout in milliseconds if
                              buffer is full. Zero to not wait at
                              all (non-blocking)

  Output:
    char **p                  Write pointer

  Function value:
    DB_SUCCESS       Successful completion

\********************************************************************/
{
   int h, i;
   unsigned char *rp;

   if (handle < 1 || handle > MAX_RING_BUFFER || rb[handle - 1].buffer == NULL)
      return RB_INVALID_HANDLE;

   h = handle - 1;

   for (i = 0; i <= millisec / 10; i++) {

      rp = rb[h].rp;            // keep local copy, rb[h].rp might be changed by other thread

      /* check if enough size for wp >= rp without wrap-around */
      if (rb[h].wp >= rp
          && rb[h].wp + rb[h].max_event_size <= rb[h].buffer + rb[h].size - rb[h].max_event_size) {
         *p = rb[h].wp;
         return RB_SUCCESS;
      }

      /* check if enough size for wp >= rp with wrap-around */
      if (rb[h].wp >= rp && rb[h].wp + rb[h].max_event_size > rb[h].buffer + rb[h].size - rb[h].max_event_size && rb[h].rp > rb[h].buffer) {    // next increment of wp wraps around, so need space at beginning
         *p = rb[h].wp;
         return RB_SUCCESS;
      }

      /* check if enough size for wp < rp */
      if (rb[h].wp < rp && rb[h].wp + rb[h].max_event_size < rp) {
         *p = rb[h].wp;
         return RB_SUCCESS;
      }

      if (millisec == 0)
         return RB_TIMEOUT;

      if (_rb_nonblocking)
         return RB_TIMEOUT;

      /* wait one time slice */
      ss_sleep(10);
   }

   return RB_TIMEOUT;
}

int rb_increment_wp(int handle, int size)
/********************************************************************\

  Routine: rb_increment_wp

  Purpose: Increment current write pointer, making the data at
           the write pointer available to the receiving thread

  Input:
     int handle               Ring buffer handle
     int size                 Number of bytes placed at the WP

  Output:
    NONE

  Function value:
    RB_SUCCESS                Successful completion
    RB_INVALID_PARAM          Event size too large or invalid handle
\********************************************************************/
{
   int h;
   unsigned char *new_wp;

   if (handle < 1 || handle > MAX_RING_BUFFER || rb[handle - 1].buffer == NULL)
      return RB_INVALID_HANDLE;

   h = handle - 1;

   if ((unsigned int) size > rb[h].max_event_size)
      return RB_INVALID_PARAM;

   new_wp = rb[h].wp + size;

   /* wrap around wp if not enough space */
   if (new_wp > rb[h].buffer + rb[h].size - rb[h].max_event_size) {
      rb[h].ep = new_wp;
      new_wp = rb[h].buffer;
      assert(rb[h].rp != rb[h].buffer);
   }

   rb[h].wp = new_wp;

   return RB_SUCCESS;
}

int rb_get_rp(int handle, void **p, int millisec)
/********************************************************************\

  Routine: rb_get_rp

  Purpose: Obtain the current read pointer at which new data is
           available with optional timeout

  Input:
    int handle               Ring buffer handle
    int millisec             Optional timeout in milliseconds if
                             buffer is full. Zero to not wait at
                             all (non-blocking)

  Output:
    char **p                 Address of pointer pointing to newly
                             available data. If p == NULL, only
                             return status.

  Function value:
    RB_SUCCESS       Successful completion

\********************************************************************/
{
   int i, h;

   if (handle < 1 || handle > MAX_RING_BUFFER || rb[handle - 1].buffer == NULL)
      return RB_INVALID_HANDLE;

   h = handle - 1;

   for (i = 0; i <= millisec / 10; i++) {

      if (rb[h].wp != rb[h].rp) {
         if (p != NULL)
            *p = rb[handle - 1].rp;
         return RB_SUCCESS;
      }

      if (millisec == 0)
         return RB_TIMEOUT;

      if (_rb_nonblocking)
         return RB_TIMEOUT;

      /* wait one time slice */
      ss_sleep(10);
   }

   return RB_TIMEOUT;
}

int rb_increment_rp(int handle, int size)
/********************************************************************\

  Routine: rb_increment_rp

  Purpose: Increment current read pointer, freeing up space for
           the writing thread.

  Input:
     int handle               Ring buffer handle
     int size                 Number of bytes to free up at current
                              read pointer

  Output:
    NONE

  Function value:
    RB_SUCCESS                Successful completion
    RB_INVALID_PARAM          Event size too large or invalid handle

\********************************************************************/
{
   int h;

   unsigned char *new_rp;

   if (handle < 1 || handle > MAX_RING_BUFFER || rb[handle - 1].buffer == NULL)
      return RB_INVALID_HANDLE;

   h = handle - 1;

   if ((unsigned int) size > rb[h].max_event_size)
      return RB_INVALID_PARAM;

   new_rp = rb[h].rp + size;

   /* wrap around if not enough space left */
   if (new_rp + rb[h].max_event_size > rb[h].buffer + rb[h].size)
      new_rp = rb[h].buffer;

   rb[handle - 1].rp = new_rp;

   return RB_SUCCESS;
}

int rb_get_buffer_level(int handle, int *n_bytes)
/********************************************************************\

  Routine: rb_get_buffer_level

  Purpose: Return number of bytes in a ring buffer

  Input:
    int handle              Handle of the buffer to get the info

  Output:
    int *n_bytes            Number of bytes in buffer

  Function value:
    RB_SUCCESS              Successful completion
    RB_INVALID_HANDLE       Buffer handle is invalid

\********************************************************************/
{
   int h;

   if (handle < 1 || handle > MAX_RING_BUFFER || rb[handle - 1].buffer == NULL)
      return RB_INVALID_HANDLE;

   h = handle - 1;

   if (rb[h].wp >= rb[h].rp)
      *n_bytes = rb[h].wp - rb[h].rp;
   else
      *n_bytes = rb[h].ep - rb[h].rp + rb[h].wp - rb[h].buffer;

   return RB_SUCCESS;
}
