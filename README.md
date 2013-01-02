# This is Recursive G's Judge System!
# This program is still unavailable!!
# Ignore all the discriptions below!!

# Recursive G's Judge System Discription
This is a Linux based Judge System which is 
written by Recursive G. It provide a set of command
tools to compile and run your program. You can
set some limites and compare the output with the
standard answer. It also contains a Web upload
system. It provides the function of judge and 
rank. It's good to use as a ACM platform.
#Features
1. An interface based on Web which will use PHP and Sqlite (maybe Python in the future).
2. A back-end written by purity C with an console interface.
3. Set a accurate limit.
4. Auto Judge and queue system.
5. Muti-Languages support.
6. Special Judge (SPJ) supported.
7. High security
8. Blocks of data support.
9. And so on.
#Usage
##./control PROGNAME  [Options]
	  PROGNAME  Name of your program. e.g.foo
	  
	  -c  Compiler     The compiler name. Default:find in goj.conf
	  -A  Arguments	   Args will be sent to the compiler.
	      		   Defult:find in goj.conf
	  -T  TimeLimit    Unit:Seconds. Float is available. Defult: 1
	  -M  MemoryLimit  Unit:MB Defult:64
	  -F  FileLimit    Unit:MB Defult:64
	  -f  Usefile	   Whether program open input file by itself
	      		   or redirect to stdin by judge system.
			   Y/N   Defult:N
	  -si StandardIn   %n means the nth data.Defult:PROGNAME%n.in
	  -so StandardOut  Defult:PROGNAME%n.out
	  -ui UserInput	   The filename of user input. Igroned when -f 
	      		   is N.   Defult: PROGNAME.in
	  -uo UserOutput   Same as -ui. Defult: PROGNAME.out
    
    The program controls the process of judge and write a log file.
    It makes a temp folder for each session.
    WorkFlow:control->preprocess->compiler->(Prepare data)->runprog->
    compare->(loop until all the datas are tested)->(write a log file)
    +-----------------+
    |Not finished yet!|
    +-----------------+


##./preprocess TEMPDIR/SOURCENAME
	 The argument will be like this:
	    ./preprocess 10037/foo.cpp
    
    The module check the source for danger functions
    and delete all the comments like {$I+}
    Return Code:0 or RF
    +-----------------+
    |Not finished yet!|
    +-----------------+

##./compiler TEMPDIR COMMAND

    Change directory to TEMPDIR and run COMMAND
    Return Code:0 or CE
    +-----------------+
    |Not finished yet!|
    +-----------------+

##./runprog JOBNAME EXENAME TimeLimit MemLimit FileLimit

    This is the core part of the whole system.
    It run the program and monitoring it.
    It can be run like this:
        ../runprog foo bar 1 64 256
    TODO:Improve the ValidFile() in oj_system.h

