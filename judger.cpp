#include "common.hxx"
using std::string;
extern CONFIG cfg;

void ExecuteChild()
{
	if((cfg.childpid=fork())==0)
    {
		freopen(cfg.infileName.c_str(),"r",stdin);
		freopen(cfg.outfileName.c_str(),"w",stdout);
		freopen(cfg.errfileName.c_str(),"w",stderr);
		setrlimit(RLIMIT_CPU,&(cfg.TimeL));
		setrlimit(RLIMIT_FSIZE,&(cfg.FileL));
		ptrace(PTRACE_TRACEME,0,0,0);
		execl(cfg.EXENAME.c_str(), "", NULL);
    }
    //ptrace(PTRACE_SETOPTIONS,cfg.childpid,0,PTRACE_O_EXITKILL);
    cfg.LogHeader();
}
void ContinueLoop(){ptrace(PTRACE_SYSCALL,cfg.childpid,0,0);}
void peekRegisters(user_regs_struct *regs){ptrace(PTRACE_GETREGS,cfg.childpid,0,regs);}
void StopLoop(){ptrace(PTRACE_KILL,cfg.childpid,0,0);}
void Alarm_In(int sig)
{
  if(sig==SIGALRM)
    {
      StopLoop();
      cfg.Log_Debug("SIGALARM!!");
      cfg.RTInfo="SIGALRM Received";
      cfg.RTCode=TLE;
    }
}
int main(int argc,char *argv[])
{
try{
	if (InitArguments(argc,argv))//Successfully
	{
		signal(SIGALRM,Alarm_In);
		ExecuteChild();
		struct timeval StartTime,StopTime;
		struct rusage ru;
		struct user_regs_struct regs;
		int intoCall=1;
		int sta;
		gettimeofday(&StartTime,0);
		alarm(cfg.TimeL.rlim_cur+1);
		while(1)
		{
			wait4(cfg.childpid,&sta,0,&ru);
			if (intoCall^=1){
				ContinueLoop();continue;	
			}
			cfg.Log_Debug("NowRunTime:"+CCString(ru.ru_utime.tv_usec+ru.ru_utime.tv_sec*1000));
			if (!CheckExited(sta)){
				StopLoop();break;
			}
			peekRegisters(&regs);
			if (regs.orig_rax==__NR_read||regs.orig_rax==__NR_write){
				cfg.Log_Debug("IOSKipped");
				ContinueLoop();continue;//Skip I/O in order to save time
			}
			if (MemoryOverflow(&ru)){
				StopLoop();break;
			}
			if (CheckSpecialSYSCALL(&regs)){
				StopLoop();cfg.RTCode=RF;break;
			}
			ContinueLoop();
		}
		gettimeofday(&StopTime,0);
		cfg.Sumarize(&StartTime,&StopTime,&ru);
	}
	else
    {
        cfg.RTCode=CRASH;
    }
    return cfg.RTCode;
}catch(std::exception& e){return CRASH;}
}


