Erl-John Tauto-An
etauto-an@csu.fullerton.edu

This assignment was written in C, compiled with GCC on Ubuntu 22.04LTS.

To compile, run the following commands in terminal:
  gcc -o memory_simulator main.c memory.c parser.c scheduler.c

To run:
  For a memory size of 2000 and page size of 100
    ./memory_simulator in1.txt 2000 100 

  For a memory size of 2000 and page size of 200
    ./memory_simulator in1.txt 2000 200 

  For a memory size of 2000 and page size of 400
    ./memory_simulator in1.txt 2000 400 

Special Notes:
This project was developed following sample outputs 2 and 3 as sample output 1
does not seem to come from the same program.

  The noted differences were:
    - out1.txt prints an input queue with one process as [1] while out2.txt and
      out3.txt prints [1 ].
    - out1.txt only prints a memory map after a process is moved to memory
      whereas the others prints one also when a process arrives.
    - More importantly, out1.txt seems to incorrectly move process 5 to memory
      when it shouldn't since prior to t=200 only five pages are available but
      6 are needed due to the process' 250kb memory pieces.
        - out2.txt and out3.txt seem to handle this by placing the process on
          hold in the queue until more space is deallocated and all of its
          memory requirements can be met.

On the other hand, this might just be a misunderstanding of memory management
concepts so if that is the case disregard the previous comments.

Thank you.
