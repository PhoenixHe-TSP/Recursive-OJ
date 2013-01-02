#define debug

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <asm/resource.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <wait.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#ifdef __x86_64__
	#include "oj_system_x64.h"
	#define CALL_ID_ORIG_EAX 8*ORIG_RAX
#else
	#define CALL_ID_ORIG_EAX 4*ORIG_EAX
#endif

//Exit Codes
#define AC 0
#define CE 1
#define RE 2
#define WA 3
#define MLE 4
#define TLE 5
#define OLE 6
#define RF 7

//Macros

#define SIZE_OF_LONG sizeof(long)
#define max(a,b) ((a)>(b)?(a):(b))
#define CLEARSTR(str) memset(str,0,sizeof(str));

/*System Calls List
 *Meanings in unistd_32.h
 *0 means UnAccept Call
 *1 means Accept Call
 *-n means Accept for n times
 */

int CallList[400]=
  {1,1,0,1,1,0,1,0,1,1,
   1,-2,1,1,1,0,1,1,1,1,
   1,0,0,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,0,1,
   0,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,
   1,0,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,0,1,
   1,1,1,1,1,1,1,1,0,1,
   1,1,1,1,1,1,1,1,1,1,//line 10
   1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,
   0,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

/*Valid File Check
 *Return 1 when Accept
 *return 0 when Unaccept
 */

int ValidFiles(char name[],char fname[])
{
  return 1;
  /*char tname[255];
  sprintf(tname,"%s.in",name);if (!strcmp(tname,fname)) return 1;
  sprintf(tname,"%s.out",name);if (!strcmp(tname,fname)) return 1;
  sprintf(tname,"%s.err",name);if (!strcmp(tname,fname)) return 1;
  return 0;*/
}

