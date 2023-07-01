#ifndef __RAN_H__
#define __RAN_H__

#include "defines.h"

#ifdef __MINGW32__  /* these functions work using winddows libraries */

u64 ran_rand(void);
u64 ran_rand_range(u64 min, u64 max);
bool ran_rand_arr(u64 *arr, usize size, bool unique);
bool ran_rand_range_arr(u64 *result, usize size, u64 min, u64 max, bool unique);

#endif

#endif
