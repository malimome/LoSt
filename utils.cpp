
#include<ctime>
#include<string.h>
#include "utils.h"

#define E109 1000000000
uint64_t getTimeElapsed(bool pr)
{
  static bool first = true;
  static timespec ts;
  if (first)
  {
	  clock_gettime(CLOCK_REALTIME, &ts);
    first = false;
    return 0;
  }

  uint64_t elapsed;
  timespec ts_aft;
	clock_gettime(CLOCK_REALTIME, &ts_aft); //CLOCK_PROCESS_CPUTIME_ID

	elapsed = (ts_aft.tv_sec - ts.tv_sec)*E109+ (ts_aft.tv_nsec - ts.tv_nsec);
/*  long tmp1, tmp2;
  tmp1 = ts_aft.tv_sec - ts.tv_sec;
  tmp2 = ts_aft.tv_nsec - ts.tv_nsec;
  if (pr)
    cout << tmp1 << "," << tmp2 << endl;
*/
  memcpy(&ts, &ts_aft, sizeof(timespec));
  
  return elapsed;
} 


