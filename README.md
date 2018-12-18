# Cache-Simulation
-----------
DESCRIPTION
-----------
This project is a simulation of Level-I cache with functionality of creating direct-mapped, set-associative
and fully-associative unified and split Level-1 caches. In simpler terms, It is a implementation of the caches computer architecuture. 
The code is purely written in C++ and it can only run on Linux machines. 
-------------
FUNCTIONALITY
-------------
This Level-I cache simulator has the following command line options to produce cache: 
-s < split > or < unified >.
-c < capacity > with < capacity > in KB: 4, 8, 16, 32, or 64.
-b < blocksize > with < blocksize > in bytes: 4, 8, 16, 32, 64, 128, 256, or 512.
-a < associativity > where < associativity > is integer size of set: 1, 2, 4, 8 or 16.

Additionally, the simulator handles data write hits and misses with following command-line options: 
- -wb write back (if write strategy not specified then write-back should be default for write hits)
– -wt write through
– -wa write allocate (if write strategy not specified then write-allocate should be default for write misses)
– -wn write no-allocate
-------
TESTING
-------
Multiple trace files have been uploaded to test the functionality of the program. 
The trace files include sequence of memory access traces, one per line, terminated at the end of file. 
In the following format, with a leading 0 for data loads, 1 for data stores and 2 for instruction
load.
0 <address>
1 <address> <dataword>
2 <address>
---------
EXECUTION
---------
1. Makefile is the configuration file for building your project. 
Copy the Makefile to the directory where your source files are and run make.
2. Enter command line prompt to generate cache. 
An example prompt might look like: cache -s -c8 -b16 -a4
--------------
CONTRIBUTIONS
--------------
Maryam Bokhari 
Kausar Ahamd 
Hassan Mansoor 
