#include "common.hxx"
string CCString(long long i){char temp[50];sprintf(temp,"%lld",i);return temp;}
bool CONFIG::InitConfigure(int argc, char * argv[])
{
	RTInfo="Exited Normally";
	RTCode=AC;
	EXENAME=argv[1];
	infileName=EXENAME+".stdin";
	outfileName=EXENAME+".stdout";
	errfileName=EXENAME+".stderr";
	LogFile.open((EXENAME+".log").c_str());
	if (!LogFile.is_open()) return false;
	getrlimit(RLIMIT_CPU,&TimeL);
	getrlimit(RLIMIT_FSIZE,&FileL);
	TimeLimit=atoi(argv[3]);
	TimeL.rlim_cur=ceil(TimeLimit/1000.0);
	FileL.rlim_cur=atoi(argv[4])*1024*1024;
	MemoryLimit=atoi(argv[2]);
	AcceptedFiles.clear();
	AcceptedFiles.insert(infileName);
	AcceptedFiles.insert(outfileName);
	AcceptedFiles.insert(errfileName);
	//More files here
	return true;
}
void CONFIG::LogHeader(){
	using std::endl;
	LogFile<<"====RGOJ=Monitor=Log==="<<endl;
	LogFile<<"Task Name : "<<EXENAME<<endl;
	LogFile<<"Time Limit : "<<TimeLimit<<"ms"<<endl;
	LogFile<<"Memory Limit : "<<MemoryLimit<<"KB"<<endl;
	LogFile<<"File Size Limit : "<<FileL.rlim_cur/1024/1024<<"MB"<<endl;
}
void CONFIG::Log(string str){LogFile<<"[Log]"<<str<<std::endl;}
void CONFIG::Log_Debug(string str){
#ifdef _debug
LogFile<<"[Debug]"<<str<<std::endl;
#endif
}
bool CONFIG::CheckOpen(string str){//Need to be improved
	//if (AcceptedFiles.find(str)!=AcceptedFiles.end())return false;
	return true;
}
bool CONFIG::CallIllegal(int Call_ID){
	if(CallList[Call_ID]<0){ //Accept Special Call
		CallList[Call_ID]++;
		return false;
	}
	else if (CallList[Call_ID]==0)//Invalid System Call
	{
		Log("Killed for Used an Invalid Call No."+CCString(Call_ID));
		RTInfo="Killed for Used an Invalid Call No."+CCString(Call_ID);
		return true;
	}
	return false;
}
void CONFIG::Sumarize(struct timeval *S,struct timeval *E,struct rusage *ru)
{
	LogFile<<"[Sumarize]Start Time:"<<S->tv_sec<<"."<<S->tv_usec<<std::endl;
	LogFile<<"[Sumarize]Start Time:"<<E->tv_sec<<"."<<E->tv_usec<<std::endl;
	LogFile<<"[Sumarize]Wall Clock Time Escaped:"<<(E->tv_sec-S->tv_sec)*1E6+E->tv_usec-S->tv_usec<<"microseconds"<<std::endl;
	long long int ust=ru->ru_utime.tv_sec*1E6+ru->ru_utime.tv_usec,sst=ru->ru_stime.tv_sec*1E6+ru->ru_stime.tv_usec;
	LogFile<<"[Sumarize]User Space Time:"<<ust<<"microseconds"<<std::endl;
	LogFile<<"[Sumarize]System Space Time:"<<sst<<"microseconds"<<std::endl;
	if (ust+sst>TimeLimit*100){
		RTCode=TLE;
		RTInfo="Running for too long";
	}
	LogFile<<"[Sumarize]"<<RTInfo<<std::endl;
	LogFile.close();
}
	
	
	
CONFIG cfg;
bool InitArguments(int argc,char *argv[])
{
	if(argc!=5){std::cout<<"Usage : ./judger Name MemoryLimit[KB] TimeLimit[millisecond] OutputLimit[MB]"<<std::endl;return false;}
	return cfg.InitConfigure(argc,argv);
}

bool MemoryOverflow(struct rusage *ru){
	cfg.maxusage=max(cfg.maxusage,ru->ru_minflt);	
	if(cfg.maxusage>cfg.MemoryLimit)
	{
		cfg.RTInfo="Memory Limit Extended";
		cfg.RTCode=MLE;
		return true;
	}
	return false;
	
}


string getOpenFile(struct user_regs_struct *regs){
	unsigned long long int addr=regs->rdi;
	union u{
		unsigned long long int val;
		char chars[PTRACE_PEEKDATA_RETURN_SIZE+3];
	} data;
	unsigned long long int offset=0,finish=0,len=0,i;
	char filename[300];
	filename[0]=0;
	while(!(finish)){
		data.val=ptrace(PTRACE_PEEKDATA,cfg.childpid,addr+offset,0);
		for(i=0;i<PTRACE_PEEKDATA_RETURN_SIZE;++i){
			if (data.chars[i]==0){filename[len]=0;finish=1;break;}
			filename[len++]=data.chars[i];
		}
		offset+=PTRACE_PEEKDATA_RETURN_SIZE;
	}
	return filename;
}

bool CheckSpecialSYSCALL(struct user_regs_struct *regs){
	int CallID=regs->orig_rax;
	cfg.Log_Debug("Child Process made a call #"+CCString(CallID));
	/*if (CallID==__NR_open){
		string OpenFile=getOpenFile(regs);
		cfg.Log("Child Process Attempts to open the file : "+OpenFile);
		if (cfg.CheckOpen(OpenFile))return false;
		cfg.RTCode=RF;
		cfg.RTInfo="Illegal File Open Attempt : "+OpenFile;
		return true;
	}*/
	return cfg.CallIllegal(CallID);
}
bool CheckExited(int sta){
	if(WIFSTOPPED(sta))//Paused by signals   
	{
		if(WSTOPSIG(sta)==SIGXCPU)//Catch SIGXCPU TLE
		{
			cfg.RTCode=TLE;
			return false;
		}
		if(WSTOPSIG(sta)==SIGXFSZ)//OLE
		{
			cfg.RTCode=OLE;
			return false;
		}
	}
	if(WIFSIGNALED(sta))//killed by signal
		if(WTERMSIG(sta)==SIGKILL)//killed by system or ALAR
			if(cfg.RTCode==AC)
			{
				cfg.RTInfo="Killed By SIGKILL";
				cfg.RTCode=RE;
				return false;
			}
	if(WIFEXITED(sta))                 //Exited normally
	{
		int extstat;
		if((extstat=WEXITSTATUS(sta))!=0)
		{
			cfg.RTInfo="Exit With a non-zero value : "+CCString(extstat);
			cfg.RTCode=RE;
		}
		else
			cfg.RTCode=AC;
		return false;
	}
	return true;
}


