/********************************************************************\

  Name:         rb.h
  Created by:   Stefan Ritt

  Contents:     Function declarations and constants for ring buffer 
                routines

  $Id: rb.h 17217 2011-02-25 15:31:29Z ritt $

\********************************************************************/

#define RB_SUCCESS         1
#define RB_NO_MEMORY       2
#define RB_INVALID_PARAM   3
#define RB_INVALID_HANDLE  4
#define RB_TIMEOUT         5

#define POINTER_T unsigned int

int rb_set_nonblocking();
int rb_create(int size, int max_event_size, int *ring_buffer_handle);
int rb_delete(int ring_buffer_handle);
int rb_get_wp(int handle, void **p, int millisec);
int rb_increment_wp(int handle, int size);
int rb_get_rp(int handle, void **p, int millisec);
int rb_increment_rp(int handle, int size);
int rb_get_buffer_level(int handle, int * n_bytes);
