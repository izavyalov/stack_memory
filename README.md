Stack Memory
============

This tiny program simulates stack overflow and handles segmentation fault following the stack overflow 
with alternate stack setup. 

~~~~~~~~~~~~  
    # Compile usual way  
    g++ -o stack_memory -O3 main.cpp  
    ./stack_memory  
~~~~~~~~~~~~  
  
  
In general the stack memory size can be configured programmatically by calling 
_setrlimit_, see:

  - http://linux.die.net/man/2/setrlimit  
  
  
System \*nix limits
-------------------

To list all limits set on \*-nix system run the following:

~~~~~~~~~~~~  
    $ ulimit -all  

    core file size          (blocks, -c) 0
    data seg size           (kbytes, -d) unlimited
    file size               (blocks, -f) unlimited
    max locked memory       (kbytes, -l) unlimited
    max memory size         (kbytes, -m) unlimited
    open files                      (-n) 2560
    pipe size            (512 bytes, -p) 1
    stack size              (kbytes, -s) 8192
    cpu time               (seconds, -t) unlimited
    max user processes              (-u) 709
    virtual memory          (kbytes, -v) unlimitedi
~~~~~~~~~~~~  

 
