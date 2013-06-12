/*
 * This file is a part of RecursiveG's Online Judge System(RGOJ)
 * This file defined some consts and provide some platform-associated
 * information so that can use ptrace on both x86 & x86-64
 */
#define debug //use -Ddebug with g++

#include <iostream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <climits>
#include <ctime>
#include <string>
#include <cmath>
using std::string;
#include <csignal>
#include <set>
extern "C"{
#include <unistd.h>
#include <sys/ptrace.h>   //for ptrace
#include <sys/resource.h> //for getrlimit&setrlimit
#include <sys/reg.h>      //EAX,EBX...||RDI,RSI...
#include <sys/user.h>     //struct user_regs_struct
#include <sys/stat.h>     //
#include <sys/types.h>    //for wait4()
#include <sys/time.h>     //
#include <wait.h>
#include <syscall.h>
}

/*===============Registers Instruction==================
 * We use ptrace to get registers of the traced process
 * The registers' struction definition is in user.h called user_regs_struct
 * and there order are in reg.h. We multiplies a register's
 * order with length per register to get its offset in memory
 * space and pass it to ptrace.
 * =================System Call Parameters==============
 * When made a system call, system push the call number
 * (can be find in asm/unistd.h) into ORIG_EAX(in x64 its ORIG_RAX). 
 * and the following parameters into other registers in a special order.
 * For x86, it's EBX,ECX,EDX......
 * And in x64, it's RDI,RSI,RDX......
 * For example, the prototype of __NR_write is this
 * ssize_t write(int fd, const void *buf, size_t count);
 * So in 64-bit platform, fd will be in RDI, RSI is a pointer
 * pointed to the string to write, and RDX is it's length.
 */ 
 #ifdef __x86_64__
	//#include "oj_system_x64.h"
	#define CALL_ID_ORIG_EAX 8*ORIG_RAX
#else
	#define CALL_ID_ORIG_EAX 4*ORIG_EAX
#endif

//Exit Codes
const int AC=10;	//Accept
const int CE=11;	//Compile Error - will never return
const int RE=12;	//Runtime Error	
const int WA=13;	//Wrong Answer - will never return
const int MLE=14;	//Memory Limit Exceed
const int TLE=15;	//Time Limit Exceed
const int OLE=16;	//Output Limit Exceed
const int RF=17;	//Restricted Function
const int CRASH=18;//System Crash
const int LE=19;	//Logical Error - will never return
//Macros

#define SIZE_OF_LONG sizeof(long)
#define max(a,b) ((a)>(b)?(a):(b))
#define CLEARSTR(str) memset(str,0,sizeof(str));

class CONFIG
{
private:
	std::ofstream LogFile;
	std::set<string> AcceptedFiles;
	/*System Calls List
 *Meanings in unistd_32.h
 *0 means UnAccept Call
 *1 means Accept Call
 *-n means Accept for n times
 */

	int CallList[400]={
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,-2,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,        //Call No.80
1,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,        //Call No.160
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,        //Call No.240
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,        //Call No.320
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1      	//Call No.400
};
public:
	bool InitConfigure(int,char**);
	void LogHeader();
	void Sumarize(struct timeval *,struct timeval *,struct rusage *);
	void Log(string);
	void Log_Debug(string);
	bool CallIllegal(int);
	bool CheckOpen(string);
	long long int maxusage,MemoryLimit,TimeLimit;
	pid_t childpid;
	string EXENAME,infileName,outfileName,errfileName;
	struct rlimit TimeL,FileL;
	int RTCode;
	string RTInfo;
	
};

bool InitArguments(int,char**);
bool MemoryOverflow(struct rusage*);
bool CheckSpecialSYSCALL(struct user_regs_struct *);
string getOpenFile(struct user_regs_struct *);
bool CheckExited(int);
string CCString(long long);

#define PTRACE_PEEKDATA_RETURN_SIZE SIZE_OF_LONG  

#ifdef __x86_64__   						//platform-associated defines start from here
        #define SIZE_PRE_REGISTER 8			//details in reg.h
	#define SYSCALL_ENTER_REG ORIG_RAX
	#define SYSCALL_FIRST RDI
	#define SYSCALL_SECOND RSI
	#define SYSCALL_THIRD RDX
#else                                   	//defines for 32-bit platform,haven't finished
	#define SIZE_PRE_REGISTER 4
	#define CALL_ID_ORIG_EAX 4*ORIG_EAX
#endif











