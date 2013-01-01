/*
 * This file is a part of RecursiveG's Online Judge System(RGOJ)
 * This file defined some consts and provide some platform-associated
 * information so that can use ptrace on both x86 & x86-64
 */
#define debug true  //use -Ddebug instead when compling

/*#ifdef __cplusplus
     #include <iostream>
     #include <fstream>
     #include <cstdlib>
     #include <climits>
     #include <ctime>
     #include <string>
     #include <csignal>
#include <cerrno>
extern c{
#else*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <limits.h>
#include <time.h>
//#endif

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
/*
#ifdef __cplusplus
}
#endif
*/
//TODO:Replace the including file with C++

#define AC 0		//Accept
#define CE 1		//Compile Error
#define RE 2		//Runtime Error
#define WA 3		//Wrong Answer
#define MLE 4		//Memory Limit Extended
#define TLE 5		//Time Limit Extended
#define OLE 6		//Output Limit Extended
#define RF 7		//Restricted Function
#define SE 8		//System Error

#define SIZE_OF_LONG sizeof(long)
#define max(a,b) ((a)>(b)?(a):(b))


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

#define SYSCALL_ENTER_REG_ADDR SIZE_PRE_REGISTER*SYSCALL_ENTER_REG
#define SYSCALL_write_STRING_POINTER_ADDR SIZE_PRE_REGISTER*SYSCALL_SECOND
#define SYSCALL_open_FILE_POINTER_ADDR SIZE_PRE_REGISTER*SYSCALL_FIRST

#ifdef debug
#define printregs printf("ORIGEAX:%lld EAX:%lld EBX:%llX RDX:%lld RSI:%llx RDI:%llx\n",regs.orig_eax,regs.r14,regs.r13,regs.rdx,regs.rsi,regs.rdi)

#endif


















