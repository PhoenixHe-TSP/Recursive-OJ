#include "oj_system.h"
#define setexit(x) exitcode=x
#define WRITE_TO_LOG(str) fprintf(LOGFILE,"%s",str)

#ifdef debug
  #define DP(x) printf("DebugPoint No.%d\n",x);
  #define LOGTMPSTR fprintf(LOGFILE,"%s",tmpstr) 
  #define CLEARTMPSTR memset(tmpstr,0,sizeof(tmpstr))  //It seems sprintf will clear the str,it may not be necessary
  #define printregs printf("ORIGEAX:%lld EAX:%lld EBX:%llX ECX:%lld EDX:%lld RDI:%lld\n",regs.orig_eax,regs.eax,regs.ebx,regs.ecx,regs.edx,regs.rdi)
#endif

//Public Variables
int exitcode=0;
pid_t CHILD;
FILE *LOGFILE;

/*To find out which file SYS_open opened. Copied from Web*/	     
void Pick_open(long addr ,char fname[])  
{
  union u{
	  unsigned int val;
	  char chars[SIZE_OF_LONG];
	  } data;
  unsigned long i=0,j=0,k=0;
  char filename[300];
  while(1)
    {
      data.val = ptrace(PTRACE_PEEKUSER, CHILD, addr +i,  NULL);//printf("%d:%c:",data.val,data.chars[0]);
      i += SIZE_OF_LONG;
      for (j = 0; j < SIZE_OF_LONG && data.chars[j] > 0 && k < 256; j++) 
	    filename[k++] = data.chars[j];
      if (j < SIZE_OF_LONG && data.chars[j] == 0)break;
    }
  filename[k] = 0;
  strcpy(fname,filename);
  //fname[0]=0;
  //printf("/n");
}

void New_Pick_open(long long addr)  
{
  union u{
	  unsigned long val;
	  char chars[256];
	  } data;
	  data.val=ptrace(PTRACE_PEEKUSER, CHILD, addr,  NULL);
	  int i;for(i=0;i<10;i++)printf("%d,",data.chars[i]);
  //printf("%s\n",(char *) &addr);
  printf("%#lx\n",addr);
  //  if(j=ptrace(PTRACE_PEEKDATA,CHILD,i,NULL),(j>=32)&&(j<=123))
    //  printf("%d",j);
	    
}

void Alarm_In(int sig)
{
  if(sig==SIGALRM)
    {
      ptrace(PTRACE_KILL,CHILD,0,0);
      WRITE_TO_LOG("SIGALRM Received\nTLE\n");
      setexit(TLE);
    }
}


int main(int args,char *argv[])
{
  if(args==1){printf("Usage: NAME EXE TLs MLm FLm\n");return 0;}
  //======Read arguments from commandLine===//
  /* In further Cross-plantform 
   * arg NAME and EXE may different
   */
  char PROBNAME[255],EXENAME[255],INFILENAME[255],
        OUTFILENAME[255],ERRFILENAME[255],LOGFILENAME[255];
  long TL,ML,FL;
  sprintf(PROBNAME,"%s",argv[1]);
  strcpy(EXENAME,PROBNAME);
  sprintf(INFILENAME,"%s.in",PROBNAME);  //Get Informations form CommandLine
  sprintf(OUTFILENAME,"%s.out",PROBNAME);//Time Unit:Seconds
  sprintf(ERRFILENAME,"%s.err",PROBNAME);//Memory Unit:KiloByte
  sprintf(LOGFILENAME,"%s.log",PROBNAME);//The Units are all used IN the prog
  TL=atoi(argv[3]);
  ML=atoi(argv[4])*1024;
  FL=atoi(argv[5])*1024*1024;
  //=======Variables Define========//
  LOGFILE=fopen(LOGFILENAME,"w");
  signal(SIGALRM,Alarm_In);
  struct rlimit TimeL,FileL;
  struct rusage ru;
  struct timeval StartT,EndT;  
  struct user_regs_struct regs;
  int sta,init=1;              //The first call is 120,skip!
  long Call_ID,maxusage=-1024;
  getrlimit(RLIMIT_CPU,&TimeL);
  getrlimit(RLIMIT_FSIZE,&FileL);
  TimeL.rlim_cur=TL;
  FileL.rlim_cur=FL;
  alarm(TL+1);               //One second longer than usual.
  //=============Log File==== 
  WRITE_TO_LOG("Ready to run program\n");
  fprintf(LOGFILE,"Problem name:     %s\n",PROBNAME);
  fprintf(LOGFILE,"Executable File:  %s\n",EXENAME);
  fprintf(LOGFILE,"Time Limit:       %ld\n",TL);
  fprintf(LOGFILE,"Memory Limit:     %s MB\n",argv[4]);
  fprintf(LOGFILE,"Open File Limit:  %s MB\n",argv[5]);
  WRITE_TO_LOG("Executing.....\n");
  //==========RUN================================
  if((CHILD=fork())==0)
    {//Here is the child process
      freopen(INFILENAME,"r",stdin);
      freopen(OUTFILENAME,"w",stdout);
      freopen(ERRFILENAME,"w",stderr);
      setrlimit(RLIMIT_CPU,&TimeL);
      setrlimit(RLIMIT_FSIZE,&FileL);
      ptrace(PTRACE_TRACEME,0,0,0);
      execl(EXENAME, "", NULL);
    }
  else
    {
      gettimeofday(&StartT,NULL);
      
      /*Main Loop:stop by break*/
      while(1)
    	{
			wait4(CHILD,&sta,0,&ru);            //Get Status
			//=====Skip SYSCALL exit======//
			if(init==0)
				init=1;              //sys call in
			else
			{
				init=0;//init==1 Call Exit, Skip!
				ptrace(PTRACE_SYSCALL,CHILD,0,0);
				continue;                     
			}
			//======Check Signals=======//
			if(WIFSTOPPED(sta))//Paused by signals   
			{
				if(WSTOPSIG(sta)==SIGXCPU)//Catch SIGXCPU TLE
				{
					ptrace(PTRACE_KILL,CHILD,NULL,NULL);
					WRITE_TO_LOG("SIGXCPU Caught!\nTLE\n");
					setexit(TLE);
					break;
				}
				if(WSTOPSIG(sta)==SIGXFSZ)//OLE
				{
					ptrace(PTRACE_KILL,CHILD,NULL,NULL);
					WRITE_TO_LOG("SIGXFSZ Caught\nOLE\n");
					setexit(OLE);
					break;
				}
			}
			
			if(WIFSIGNALED(sta))//killed by signal
			
				if(WTERMSIG(sta)==SIGKILL)//killed by system or ALAR
				{
					ptrace(PTRACE_KILL,CHILD,NULL,NULL);
					if((exitcode!=TLE)&&(exitcode!=OLE)&&(exitcode!=MLE))
					{	
						WRITE_TO_LOG("Killed by SIGKILL\nRE\n");
						setexit(RE);
					}
					break;
				}
			
			/*Be Careful!There are some differences between
				WIFEXITED,WIFSIGNALED and WIFSTOPPED*/
			
			//===========Check whether Exited=====//
			if(WIFEXITED(sta))                 //Exited normally
			{
				if(WEXITSTATUS(sta)!=0)
				{
					fprintf(LOGFILE,"Exit With a non-zero value:%d\nRE\n",WEXITSTATUS(sta));
					setexit(RE);
				}
				else
					WRITE_TO_LOG("Exit Normally\nAC\n");
				break;
			}	  
			//========Get SYSCALL_ID and jump SYSREAD/WRITE==//
			Call_ID=ptrace(PTRACE_PEEKUSER,CHILD,CALL_ID_ORIG_EAX,NULL);//get call id 
#ifdef jmpIO
			if((Call_ID==SYS_write)||(Call_ID==SYS_read))
			{
				ptrace(PTRACE_SYSCALL,CHILD,NULL,NULL);
				continue;
			} /*Skip SYSREAD/SYSWRITE to make the program run faster*/
#endif
			//TODO:find why CALL_ID will eq -1 and fix it
			//NOTE:be careful the difference between PTRACE_SYSCALL / SINGLESTEP
			if (Call_ID<0){
				ptrace(PTRACE_KILL,CHILD,NULL,NULL);
				WRITE_TO_LOG("Invalid System-Call ID\nRE\n");setexit(RE);
				break;
			}
		  
			//------Check memory usage-------------//  
			maxusage=max(maxusage,ru.ru_minflt);	
			if(maxusage>ML)
			{
				ptrace(PTRACE_KILL,CHILD,NULL,NULL);
				WRITE_TO_LOG("Memory Limit Extended\nMLE\n");
				setexit(MLE);
				break;
			}

			//=============Special SysCall check=========//
			#ifdef debug
			fprintf(LOGFILE,"System Call:%ld\n",Call_ID);
			#endif
			
			if(Call_ID==SYS_open)
			{
				ptrace(PTRACE_GETREGS,CHILD,0,&regs);
				char t[255];
				memset(t,0,sizeof(t));
				//Pick_open(regs.ebx,t);
				printregs;
				New_Pick_open(regs.rdi);
				if (ValidFiles(PROBNAME,t))
				  CallList[Call_ID]--;//Make an Once Accpetion
				fprintf(LOGFILE,"File Opened:%s\n",t);
			}
			if(CallList[Call_ID]<0) //Accept Special Call
				CallList[Call_ID]++;
			else if (CallList[Call_ID]==0)//Invalid System Call
			{
				ptrace(PTRACE_KILL,CHILD,0,0);
				fprintf(LOGFILE,"Killed for Used an "
					"Invalid Call No.%ld\nRF\n",Call_ID);
				setexit(RF);
				break;
			}//else CallList>0 are Valid forever//
	    
	    
			ptrace(PTRACE_SYSCALL,CHILD,NULL,NULL); /*DO NOT use PTRACE_SINGALSTEP*/
		}
      gettimeofday(&EndT,NULL);
    }
    //TODO:Print these to logfiles.
    if (EndT.tv_sec-StartT.tv_sec+(EndT.tv_usec-StartT.tv_usec)/1000000.0>TL)
    {
       WRITE_TO_LOG("Time Limit Extended\nTLE\n");
       setexit(TLE);
    }  
#ifdef debug
  printf("Spend Time : %f  (REAL)\n",EndT.tv_sec-StartT.tv_sec+(EndT.tv_usec-StartT.tv_usec)/1000000.0);
  printf("Spend Time : %f  (USER)\n",ru.ru_utime.tv_sec+ru.ru_utime.tv_usec/1000000.0);
  printf("Spend Time : %f  (SYST)\n",ru.ru_stime.tv_sec+ru.ru_stime.tv_usec/1000000.0);
  printf("Memory Used: %ld  (MAX)\n",maxusage);  
  printf("RunPart Statue:%d\n",exitcode);
  fprintf(LOGFILE,"RunPart Statue:\n%d",exitcode);
#endif
  fclose(LOGFILE);
  return exitcode;
}
