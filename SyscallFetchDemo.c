#include "rgoj_header.h"

pid_t pid;
char* chr;
int getstr(unsigned long long int addr,char str[])  
{
  union u{
    unsigned long long int val;
    char chars[PTRACE_PEEKDATA_RETURN_SIZE+3];
  } data;
  int offset=0,finish=0,len=0,i;
  char filename[300];
  filename[0]=0;
  while(!(finish)){
    data.val=ptrace(PTRACE_PEEKDATA,pid,addr+offset,0);
    for(i=0;i<PTRACE_PEEKDATA_RETURN_SIZE;++i){
      if (data.chars[i]==0){filename[len]=0;finish=1;break;}
      filename[len++]=data.chars[i];
    }
    offset+=PTRACE_PEEKDATA_RETURN_SIZE;
  }
  strcpy(str,filename);
  return len;
}

int main(int argc,char* argv[])
{
  unsigned long long int reginfo;
  int len=0;
  char tmpstr[256];
  printf("Program startup successful\n");
  int sta,callid,flag=1;
  if((pid=fork())==0)
    {//ChildProcess
      freopen("stdout.txt","w",stdout);
      freopen("stderr.txt","w",stderr);
      ptrace(PTRACE_TRACEME,NULL,NULL,NULL);
      execlp("ls","",NULL);
      printf("execFailed");
    }
  else
    //Parent Process
    while(1){
      wait(&sta);
      if (WIFEXITED(sta)){
        printf("Child Exited!\n");
	    break;
      }
      if (flag){flag=0;}else{flag=1;ptrace(PTRACE_SYSCALL,pid,0,0);continue;}
      callid=ptrace(PTRACE_PEEKUSER,pid,SYSCALL_ENTER_REG_ADDR,NULL);
      if (callid==__NR_write){
		printf("Called write\n");
		reginfo=ptrace(PTRACE_PEEKUSER,pid,SYSCALL_write_STRING_POINTER_ADDR,0);
		len=getstr(reginfo,tmpstr);
		printf("Writed:%s\nTotal %d characters\n",tmpstr,len);
      }
      else if(callid==__NR_open){
		printf("Called Open\n");
		reginfo=ptrace(PTRACE_PEEKUSER,pid,SYSCALL_open_FILE_POINTER_ADDR,0);
		len=getstr(reginfo,tmpstr);
		printf("Opened:%s\nLength %d\n",tmpstr,len);
      }
      else{
		printf("Made a normal call:%d\n",callid);
      }
      ptrace(PTRACE_SYSCALL,pid,NULL,NULL);
    }
  return 0; 
}


