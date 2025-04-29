TITLE -- CSCB09 Assignment 2

**1. Metadata**
Author: Jeong Yuseon
Date: March 7, 2025

**2. Introduction/Rationale**

    This project is about building a tool to display
    the tables used by the OS to keep track of open files, the assignment
    of file descriptors and processes. In addition, it aims to identify
    how the binary and ASCII format files differ in their compilation with
    regards to time and file size and analyze the data collected from the
    process

    The source files are described below:
        i.   makefile: Compiles the code efficiently.
        ii.  main.c: Control Center in the code.
                – Parses the flagged arguments and decides whether a particular
                PID or all PIDs will be presented
                – Retrieves all the data and stores the location of the specific
                PID in the pid_list
                – displays the graphs or outputs the file depending on the arguments
                passed
        iii.  utility.c: Retrieves data for each PID such as FD, filename, and inode.
        iv.   pid.c: Stores all data in the pid info pid list structure for all PIDs.
        v.    graph.c: Creates graph based on the pid info
       


**3. Description of how you solve / approach the Problem**

    Think of it this way, you are asked to display the table of the OS which keeps track of files, file descriptors and processes. There are three tasks we need to do,

    i. retrieve the data from the OS,
    ii. store it into a data structure
    iii. display the data that is stored

    So, let’s dive into the details....!!!

    1.data retrieval
        i. It took me two attempts to solve this. First attempt of learning 
        how to retrieve the data, then another cycle of trying to efficiently 
        retrieve the data.

        ii. In the first attempt, I realized that all the retrieval was a
        domino effect. What I mean is that when I find the PID, I need
        to use it to get the file descriptor. Using each of the file
        descriptors and PID, I need to find the filename. Lastly, using
        all these data, I needed to get the vnodes. 
        
        If some vnodes were not found in the stat() function and it was possible (Reference #5),I had to search directly into the other terminal command below:
        /proc/<PID>/fdinfo/<fd> (Reference #3).

        iii. In the second attempt, I organized it into one function where,
        given a particular PID number, the function **ProcessPID()** will
        retrieve the file descriptor count from /proc/<PID>/fd. Subsequently,
        the PID along with the file descriptor number was used to retrieve
        the filename from /proc/<PID>/fd/<FD> in the **SystemWideFD()** function.
        Finally, this function will call another **Vnode()** function using
        the stat() call (Reference #5). However, this did not apply for sockets or certain
        files. So I searched the internet for an alternative and this
        was what I found: /proc/pid/fdinfo/ (Reference #3).

    2.storing of data
        i. The first attempt was trying to divide where the line between
        the **PID_INFO** and **FILE INFO** lie. 
        
        The **PID_INFO** is basically used to store all the information about the PID. 
        It consists of the number for the PID, fd count, fd capacity to know the 
        size of this filedata to be stored in the heap and the fd array to store all
        the file details for different files. 
        
        The other struct of **FILE_INFO** was for storing the specific file data such 
        as the FD, filename and the inode associated with it.
        
        I discovered upon retrieving the data that getting some information on certain PIDs
        was constantly giving me an error as it was beyond my accessibility limits.
        So, what I did was to create a temporary struct where it will only be placed into the 
        list if and only if it existed in the linux OS.

        ii. To solve this problem, I used a helper function of tempPIDINFO() that basically 
        creates a temporary struct. It will be passed to the ProcessFD() and depending on the 
        return value will decide whether it should be place on the pid list or not. If it 
        is successful, I will put the value into the pid list and if it is not, then it
        will free this struct.

        iii. In summary,
            a. The **PID_INFO** is a struct that stored the PID information.
            b. The **FILE_INFO** is a struct that stored the File information.

    3.graph
        i. I would say this was a straightforward approach where it only
        required drawing the diagrams based on the pid list and the size
        of the list. The X is the threshold, so any value above that
        count will only be the ones to be printed as stated in the
        assumption area. 

        ii. I only factored out the drawing of the '=' sign to separate the
        creation of the heading to reduce redundancy in code

        iii. The bonus portion was also included in here as it is also part of the 
        graph since the only difference was being redirected to the compositeTable.txt
        or compositeTable.bin file depending on the user preference.

    4.main
        i. This is like a control center responsible for parsing through
        all the arguments. Based on the arguments passed, it will update the struct 
        Based on the flags, it will decide which of the graphs to print.
        Afterwards, it will free all the flags and struct when done with the entire
        process to prevent memory leaks.

        ii. I created a FLAGs struct to store all the flags indicating which
        one to print. There is also specificPID that stores the value
        of the specific PID along with specificIndex to store the location
        of where to find this specificPID in the list. (So, you might
        be wondering why did I have to do this? It is because the available
        filenames change overtime, so I will just check within the entire
        list that is generated at one point in time and get the location
        of this specific PID and store the location in the flag struct.)

**4. Implementation**

******Overview******
    Division of files:
        a.data retrieval ->utility.c
        b.storing of data -> pid.c
        c.graph -> graph.c
        d.control center -> main.c  
        e.How to compile -> Makefile  

******utility.c******
    1. Implementation of the Code
        a. Fetches all the information for a particular PID such as the filename, vnodes, and file descriptor count

        b.Divided into three sections where file descriptor count is found by 'ProcessFD()', filename for 'SystemWideFD' and vnodes for 'Vnode()'.

    2. The modules you created and the function within them
        **HEADER FILE(EXTRA)**
        -#define _POSIX_C_SOURCE 200809L: used to conform to the standards of -std=c99(Reference #4)
        -#include <stdio.h>: for fprintf() and snprintf(), and perror();
        -#include <dirent.h>: for opendir(), readdir(),and closeddir(); 
        -#include <stdlib.h>: for strtol();
        -#include <sys/stat.h> for getting stat()
        -#include <unistd.h> for readlink();
        
        **Modules and Functions within them**

        -(NOTE: Ther utility.h is the module for this function itself, so I did not include it in this list)

    3. Description of each function
        **ProcessFD(PID INFO * pid info)**
        -> parameters: pid_info struct to store the PID and its file descriptors

        -> responsible for retrieving the total number of file descriptors for each PID

        -> this function uses the snprintf() function from the stdio.h to create the string of '/proc/<PID>/fd' using a  associated PID found in the pid_info struct. It uses this data to access all the File descriptor number and save it in the 'pid_info' struct. For each file descriptor number, it passes to the 'SystemWideFD' function to get the respective filenames. If the size of the file descriptor exceeds the fd_capacity, then it reallocs fd_array and assigns a new size for it. 
        
        ->If the data was successfully obtained, it will return a 1 and 0 otherwise. 

        **SystemWideFD(PID INFO * pid info)**
        -> parameters: pid_info struct to store the PID, file descriptors and find the associated filename for its file descriptor

        ->responsible for getting the filenames for each of the file descriptors

        -> this function uses the snprintf() function from the stdio.h also to create the string of '/proc/<PID>/fd/<FILE #>' to get the filename. It uses this to access the filename and save it to the 'pid_info' struct. for each filename, it calls the 'Vnode' function to get the respective vnodes. 

        -> It the data was successfully obtained, it will just update the struct

        **Vnode(PID INFO * pid info, int index)**

        -> parameters pid_info struct to store the data such as PID, file descriptor, filename and its associated vnode. 

        ->responsible for getting the vnode for each target file

        ->this function uses the stat function coming from the '#include <sys/stat.h>' function to get the vnode number. For some functions like sockets and etc, they do not have a vnode number, so I looked up the internet and found that I can just use the '/proc/<PID>/fdinfo/<FD>' to access it (Resource #3).

        -> It the data was successfully obtained, it will just update the struct

******pid.c******
    1. Implementation of the Code
        a. Finds all the PID and stores it into the pid_list by opening the '/proc' directory. The only way we can access it is by opening the directory stream corresponding to the directory using  opendir function (Reference #1). 

        b. Divided into two sections of tempPIDINFO - a struct to temporarily create a struct of 'PID_INFO *'. This is to check whether a specific PID exists in the 'proc' directory before adding to the pid_list
        
    2. The modules you created and the function within them
        **HEADER FILE(EXTRA)**
        -#include <dirent.h> for opendir(), readdir(), closedir()
        -#include <stdio.h> used for standard input/output fnctions like perror();
        -#include <stdlib.h> for standard library utilities like strtol() (reference #2) 
        
        **Modules and Functions within them**

        -#include "utility.h"
            - ProcessFD(PID_INFO *pid_info);
            - SystemWideFD(PID_INFO *pid_info);
            - Vnode (PID_INFO * pid_info, int index);

        -(NOTE: The pid.h is the module for this function itself, so I did not include it in this list)
    3. Description of each function

        **tempPIDINFO(int pid)**

        -> parameters pid is the PID number

        ->responsible for temporarily creating a 'PID_INFO*' struct.

        -> allocates a temp of the size of a single FILE_INFO in the heap and saves the address of this in the temp. If the temp is NULL, then it means it failed to allocate memory so it returns NULL. Else, it stores the pid number in the temp->pid = pid, fd_count = 0, fd_capacity = 20 (random starting size for the fd_array which will be resized later) and allocates a fd_array of the size of FILE_INFO * fd_capacity. If the fd->array is NULL, it means it failed to allocate memory and it returns the temp  

        -> If failed to allocate memory, returns NULL and if successful, it returns. 

        **findAllPID(int *size)**
        ->parameters is the '*size' of the 'pid_list'

        -> it opens the directory of '/proc'. As it is a directory, it uses opendir and stores the address in DIR * dir. If the directory is NULL, it means it failed to open the directory so it returns NULL. It makes the pid_list of a random size that was passed as *size in the heap. If pid_list is NULL, it means it failed to allocate memory for pid_list. So, we close the directory and return NULL to indicate it failed.  It reads the files in the directory and it stores the pid number in pid. If the remaining_char is not equal to '\0', it means that there are certain characters that are not digit in the list so we skip through the list by doing 'continue'. After getting all the pid information for all the PIDs available on the system, it will close the directory and return the 'pid_list'

        -> it returns the pid_list when successful

******graph.c******
    1.Implementation of the Code
        a.Responsible for drawing all the graphs for each of the 'pid_list' based on the request of the main

        b.Also, responsible for creating different files that is stored as either '.txt' or '.bin' depending on the main's call. 

    2.The modules you created and the function within them
        **HEADER FILE(EXTRA)**
        -#include <stdio.h> for standard input and output like printf(), fprintf() and perror()
        -#include <stdlib.h> for malloc() and realloc()
        -#include <string.h> for strncpy()
            
        **Modules and Functions within them**
        -#include <pid.h>
            - findAllPID(int *size);

        -#include <utility.h> 
        (NOTE: THE MAIN USE OF THIS IS FOR THE STRUCT of PID_INFO and FILE_INFO)
            - ProcessFD(PID_INFO *pid_info);
            - SystemWideFD(PID_INFO *pid_info);
            - Vnode(PID_INFO *pid_info, int index);

        -(NOTE: Ther graph.h is the module for this function itself, so I did not include it in this list)

    3. Description of each code
        **print(int size)**

        -> parameters is size to indicate how many '=' sign to print'
        
        -> responsible for creating the division between heading and the information or the bottom line. 

        ->prints a bunch of '=' sign line

        **HandleProcessFDTable(PID_INFO ** pid_list, int size, int X)**
        
        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and X to indicate the threshold.

        ->responsible for displaying the Process-FD table of values ABOVE the threshold

        ->prints the Process-FD table

        **HandleSystemWideTable(PID_INFO ** pid_list, int size, int X)**

        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and X to indicate the threshold.

        ->responsible for displaying the System Wide table of values ABOVE the threshold

        ->prints the System Wide Table

        **HandleVnodesTable(PID_INFO ** pid_list, int size, int X)**

        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and X to indicate the threshold.

        ->responsible for displaying the Vnodes table of values ABOVE the threshold

        ->prints the Vnodes Table

        **HandleCompositeTable(PID_INFO ** pid_list, int size, int X)**

        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and 'X' to indicate the threshold.

        ->responsible for displaying the Composite table of values ABOVE the threshold

        ->prints the Composite Table

        **HandleSummaryTable(PID_INFO ** pid_list, int size, int X)**

        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and 'X' to indicate the threshold.

        ->responsible for displaying the Summary table of values ABOVE the threshold

        ->prints the Summary Table

        **HandleThresholdTable(PID_INFO ** pid_list, int size, int X)**

        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and X to indicate the threshold.

        ->responsible for displaying the Threshold table of values above the threshold

        ->prints the '## Offending processes' Table only 


        **HandleSummaryTable(PID_INFO ** pid_list, int size, int X)**

        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and X to indicate the threshold.

        ->responsible for displaying the Summary table of values ABOVE the threshold

        ->prints the Summary Table


        
        **HandleOutputTXT(PID_INFO ** pid_list, int size, int X)**

        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and 'X' to indicate the threshold.

        ->responsible for writing in ASCII of the compositeTable in a separate file named "compositeTable.txt"

        ->creates the compositeTable.txt

        **HandleOutputTXT(PID_INFO ** pid_list, int size, int X)**

        ->parameters is the 'pid_list' for the list of PIDs, size for the 'pid_list' and 'X' to indicate the threshold.

        ->responsible for writing in binary of the compositeTable in a separate file named "compositeTable.bin"

        ->creates the compositeTable.bin

******main.c******
    1. Implementation of the Code
        a. Checks for all the arguments and updates the flags accordingly

        b. If the user requests a specific pid, then that pid is updated to be used.
	If not, then it will simply use all the PID available in the system. 

        b. Prints out the needed tables based on the flags that are accepted by the user

        c.Frees all the memory to avoid leaks

    2.The modules you created and the function within them
        **HEADER FILE(EXTRA)**
        -#include <stdbool.h> for true and false
        -#include <stdio.h> for input and output functions like fprintf()
        -#include <string.h> for strcmp()and strncmp()
        -#include <dirent.h> for opendir(), closeddir(),readdir()
        -#include <ctype.h> for isdigit()
        -#include <stdlib.h> for atoi()
        -#include <unistd.h> for readlink()
        -#include <sys/stat.h> for stat()

        **Modules and Functions within them**
        -#include "pid.h"
            -findAllPID(int *size);

        -#include "graph.h"
            -HandleProcessFDTable(PID_INFO ** pid_list, int size, int X);
            -HandleSystemWideTable(PID_INFO ** pid_list, int size, int X);
            -HandleVnodesTable(PID_INFO ** pid_list, int size, int X);
            -HandleCompositeTable(PID_INFO ** pid_list, int size, int X);
            -HandleSummaryTable(PID_INFO ** pid_list, int size, int X);
            -HandleThresholdTable(PID_INFO ** pid_list, int size, int X);
            -HandleOutputTXT(PID_INFO ** pid_list, int size, int X);
            -HandleOutputBIN(PID_INFO ** pid_list, int size, int X);
        

    3. Description of each code
    -> Parameters is argc for the total number of arguments of *argv[] for the arguments. 

    -> If the first argument is a digit, checks whether it is a valid number and then saves this as a particular PID. It can only be passed as a first argument to be considered a specific pid number.
    
    It goes through all the arguments and updates the flags to true depending on the arguments passed by the user. If the user has a specific PID that was passed, then it looks through the generated list and then finds the index of this PID. It will go through the flags and choose which table to print or which files to save.
    
    It frees everything in the end to avoid any leaks

    ->returns with the graphs or an error depending on the user's validity of the arguments

**5. PseudoCode/Flow Chart**

![Alternative Text](Midterm/Assignment2/Screen Shot 2025-03-09 at 3.38.47 PM.png)

-> I added the link to the plot, the visual studio does not allow me to generate to attach it on my end. But it is attached in this Assignment file for you to check it out
However, here is my explanation for it

Based on this link the prof has given,
https://en.wikipedia.org/wiki/Flowchart(Reference #9)

The main undergoes the following
    1.process: read CLA(rectangle)
    2.process: parses the CLA(rectangle)
        i. Makes a decision: (triangle) if it is a valid CLAs
    3.process: stores all the data into the pid_list(rectangle)
        i. Predefined Process: fetches all the PIDs from a different file like pid.c that calls utility.c (rectangle with double-struck vertical edges)
    4.Makes a decision:if the second argument after the ./program is a number
        i. Process: fetches all the data for the pid_list
    5.Make a decision: if a certain flag is true (applies for multiple of them)
        i. Predefined Process: generates the plot using graph.c (rectangle with double-struck vertical edges)

Note: look at how the arrow for the if *argv[1] == digit is true points to certain processes, it means that they will adjust their plots if it is for a specific PID. For the ones that does not, it will generate for all PIDs.



**6. Instructions on how to compile your code**
-First, run 'make' to compile all parts of the code
    -If you want to remove the previous files, call 'make clean'
-Second, call './program' along with some command line arguments to decide which one to print
    -If you want a specific PID, run './program <PID#>'
    -It is invalid when you run './program <Flag argument #1> <Flag argument #2> <PID#>'

**7. Expected Results** and **8. Test Cases**

**NOTE: The way the code is arranged is that it will print per process table, systemWide table, Vnodes table, composed table, followed by summary table and then threshold table. **


1. ./program --per-process will print the per process table with a header followed by an '=' sign heading and then a '=' sign bottomline

2. ./program --per-process --systemWide will print the per process table and systemWideTable with '=' sign as division line between them

3. ./program --Vnode will print the Vnode table with a header followed by an '=' sign heading and then a '=' sign bottomline

4../program --composite will print the composed table with the numbering on the left side with the numbering, followed by PID, FD, Filename, and Inode #.  

Hard to explain, just look at the picture below. 

	PID		FD	Filename		Inode
	=====================================================
0   	47052   	0   	/dev/null           	5
1   	47052   	1   	socket:[435802]     	435802
2   	47052   	2   	socket:[435802]     	435802
3   	47052   	3   	socket:[435829]     	435829
4   	47052   	4   	anon_inode:[eventpoll]	2083
5   	47052   	5   	anon_inode:[signalfd]	2083
6   	47052   	6   	anon_inode:inotify  	2083

5. ./program --threshold=20 will print only the Offending processes table above the threshold of 20

## Offending processes:
47052 (51), 47067 (45), 47069 (36), 47072 (23), 47289 (30), 47303 (24), 47326 (37), 47337 (23), 47421 (79), 47492 (26), 48283 (24), 

6. ./program will print the composed table of all PIDs

7. ./program 51260 will print the composed table of a specific PID

	PID		FD	Filename		Inode
	=====================================================
0   	51260   	0   	pipe:[410616]       	410616
1   	51260   	1   	pipe:[410617]       	410617
2   	51260   	2   	pipe:[410618]       	410618
	=====================================================

8. ./program 51419 --output_TXT

If this PID does not exist in the list, then it will print, 

    'Specific PID 51419 not found in the list'

9../program 50657 --output_binary

If the specific PID exists in the list, then it will print 

    '>>> Target PID: 50657'

NOTE: HOWEVER, IT WILL PRINT EVERYTHING AS THE OUTPUT_BINARY AND OUTPUT_TXT IS AIMED AT PRINTING THE ENTIRE COMPOSED TABLE FOR ALL THE PIDS


10../program --threshold=-10

If the number is not positive or is not a number, then it will print the same error message regardless

    'Error: -10 is not a number'

11../program --threshold=apple

Same here:
    'Error: apple is not a number'

12../program --threshold=singe

and here:
    'Error: singe is not a number'

13. ./program 50657 --per_process

If spelling error, then: 
    'Invalid argument --per_process'

14. ./program 50657 --per-process

Expected output is below: 
>>> Target PID: 50657

	PID	FD
	===========
	50657	0
	50657	1
	50657	2
	50657	3
	===========

15../program 50657 --systemWide

Expected output is below: 
>>> Target PID: 50657

	PID	FD	Filename
	========================================
	50657	0	/dev/pts/5
	50657	1	/dev/pts/5
	50657	2	/dev/pts/5
	50657	3	/dev/pts/5
	========================================

16../program 50657 --Vnodes

Expected output is below: 
>>> Target PID: 50657

	FD	Inode
	=============
	0	8
	1	8
	2	8
	3	8
	=============

17. ./program 50657 --composite

Expected output is below: 
>>> Target PID: 50657

	PID		FD	Filename		Inode
	=====================================================
0   	50657   	0   	/dev/pts/5          	8
1   	50657   	1   	/dev/pts/5          	8
2   	50657   	2   	/dev/pts/5          	8
3   	50657   	3   	/dev/pts/5          	8
	=====================================================

18. ./program 50657 --summary

Expected output is below: 
>>> Target PID: 50657

	Summary Table
	===========
47052(51), 47067(45), 47068(17), 47069(36), 47072(23), 47091(14), 47285(4), 47289(30), 47303(24), 47326(37), 47337(23), 47421(79), 47492(26), 47537(11), 48163(3), 48170(3), 48188(13), 48283(24), 50657(4), 52029(3), 52056(5), 

Regardless of the specific PID passed, Summary Table will use the entire pid_list

19. ./program 50657 --threshold=20


Expected output is below:
>>> Target PID: 50657


## Offending processes:
47052 (51), 47067 (45), 47069 (36), 47072 (23), 47289 (30), 47303 (24), 47326 (37), 47337 (23), 47421 (79), 47492 (26), 48283 (24), 


Regardless of the specific PID passed, threshold Table will use the entire pid_list

20. ./program 50657

Expected output is below:
>>> Target PID: 50657

	PID		FD	Filename		Inode
	=====================================================
0   	50657   	0   	/dev/pts/5          	8
1   	50657   	1   	/dev/pts/5          	8
2   	50657   	2   	/dev/pts/5          	8
3   	50657   	3   	/dev/pts/5          	8
	=====================================================


**9. Disclaimers, Assumptions and Default Behavior**
    1. Default Behavior
        i. If no argument is passed to the program, then the program will display
        the composite table (i.e., the same effect as using the --composite flag).

        ii. The valid argument includes the following:
            – per-process
            – systemWide
            – Vnodes
            – composite
            – summary
            – threshold=X (NOTE: NO SPACE ALLOWED BETWEEN = and X)
            – output TXT
            – output binary

    2. Disclaimers(!!I CHOSE THIS TO WORK LIKE THIS!!)
        i. If a threshold value is passed, all the graphs will only print data above the
        threshold.
            – HandleProcessFDTable
            – HandleSystemWideTable
            – HandleVnodesTable
            – HandleCompositeTable
            – HandleSummaryTable
            – HandleThresholdTable
            – HandleOutputTXT
            – HandleOutputBIN

        ii. If there is a space between threshold and X, it will fail to read the data
            - If the X value is not a number, it will fail

        iii. 

        If the HandleThresholdTable is called, then it will only display the ## Offending processes table only. This Offending processes table will use all PIDs regardless of whether or not the user chose a  specific or all PIDs.

        If the Summary Table is called, then it will only display the Summary Table only. This table will use all PIDs regardless of whether or not the user chose a specific or all PIDs.  
        
        This similarly applies to output_TXT and output_binary as well. 


        iv. It only accesses PIDs with access and skips PIDs without access in utility.c

        v. Only one positional argument indicating a particular process id number (PID) is allowed; this PID can only be passed as the (first argument).

        *** Bonus Points Activity:  For one PID, the same PID was used throughout to collect the data for the excel sheet**


**References**
1. https://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html
2. https://www.scaler.com/topics/stdlib-h-in-c/
3. https://man7.org/linux/man-pages/man5/proc_pid_fdinfo.5.html
4. https://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
5. https://man7.org/linux/man-pages/man2/stat.2.html
6. https://www.gnu.org/software/time/#:~:text=The%20%60time'%20command%20runs%20another,resources%20used%20by%20that%20program.&text=Often%20your%20shell%20(e.g.%20bash,built%2Din%20keyword%20or%20not.
7. https://man7.org/linux/man-pages/man1/stat.1.html
8. https://www.totalphase.com/blog/2023/05/binary-ascii-relationship-differences-embedded-applications/#:~:text=Binary%20code%20is%20a%20system,from%20one%20computer%20to%20another.
9. https://en.wikipedia.org/wiki/Flowchart

**Bonus Points** 

Google Sheet:
https://docs.google.com/spreadsheets/d/1oLhDzrGxVTrVv9KVs42hyfBtpTatiNqNSr0ikCQmQZU/edit?usp=sharing

Disclaimer:
->For one PID, the different set of PID was used where the time and size was matched together 
collection.  While for all PID, it is basically the same list ran several times

->Moreover, --output_TXT and --output_binary will only output the ASCII text and binary text files of compositeTable.txt and compositeTable.bin respectively. 

->For the specific PID, it will print on the terminal **>>> Target PID:** instead of printing on the files itself. 

->To measure the time, it was found using 

    time ./program --output_binary(Resource #6)
    
    <!-- Used value from here: real	0m0.041s -->

    real	0m0.041s
    user	0m0.000s
    sys	0m0.019s

->To measure file size, it was found using 

    stat compositeTable.bin (Resource #7)

    <!-- Used value from here: Size: 31916  -->
    
    File: compositeTable.txt
    Size: 31916     	Blocks: 240        IO Block: 1048576 regular file
    Device: 0,70	Inode: 4996832706  Links: 1
    Access: (0600/-rw-------)  Uid: (26113884/jeongyu6)   Gid: (  500/cmsusers)
    Access: 2025-03-08 20:01:15.722157000 -0500
    Modify: 2025-03-09 14:20:13.060044000 -0400
    Change: 2025-03-09 14:20:13.060044000 -0400
    Birth: -


Short Introduction: 
We know that binary code is a system of binary digits, otherwise known as 0 and 1s that represent machine instructions. On the other hand, ASCII(American Standard Code for Information Interchange) code is a set of encoding standard that assigns numeric values typically represented by binary numbers to characters. For example, 01000001 is used to represent 'A' for binary and 'A' is 65 in ASCII(Resource #8). 


File Size:
->The file size for the binary text was smaller compared to the ASCII text. When ran for ALL PIDS, the binary text had a smaller size of 25200 bytes and 31916 bytes for ASCII text. For a specific PID case, the binary text also had a smaller size of 25871.6 bytes and 31244.6 bytes for ASCII text. 

Time:
->The compilation time for the binary text was smaller compared to the ASCII text. When ran for ALL PIDS, the binary text had a shorter time of 0.0258 seconds and 0.026 seconds for ASCII text. For a specific PID case, the binary text also had a shorter time of 0.0245 seconds and 0.0266 seconds for ASCII text. 


Analysis: 
Based on the lecture slides, we have learnt that it is faster to process data for binary text when we are accessing variables while slower in processing characters or strings. If we look at the composite table, we can evidently see a lot of integers compared to strings. This might have led to the file size and the process time for  binary table to be smaller than the ASCII table. 