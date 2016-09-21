/*****************************************************************************************************
* Description:                 Runnable system call test of the first project of CSE422 
*
* Author:                      Dengxue Yan, Washington University in St. Louis
*
* Email:                       Dengxue.Yan@wustl.edu
*
* Rev History:
*       <Author>        <Date>        <Hardware>     <Version>        <Description>
*     Dengxue Yan   09-15-2016 15:30       --           1.00             Create
*****************************************************************************************************/
//====================================================================================================
// Declaration of header files 
//----------------------------------------------------------------------------------------------------
#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

//====================================================================================================
// Macros
//----------------------------------------------------------------------------------------------------
#define MAX_NUM_CHILD 32

#ifndef NULL
#define NULL (void *)0
#endif

//====================================================================================================
// Declaration of local functions, these functions can not be accessed from outside
//----------------------------------------------------------------------------------------------------
static void usage(char *prog_name);
static int simplestrtoul(const char *str);
static int runnabletest(int fork_times);
static int runnabletest1(void);

//====================================================================================================
// Functions implementation
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: main
//      Input: 
//     Return: int: The status of function
//Description: System entrance
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   Dengxue Yan     09-15-2016 15:30           Create
//----------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int index;
    int parameter2;
    char test_method;
    int test_ret;

    if (argc > 3)
    {
        usage(argv[0]);
        exit();
    }

    printf(1, "#========================================================================\n");
    printf(1, "# Command Name : %s\n", argv[0]);
    printf(1, "# Author       : Dengxue Yan\r\n");
    printf(1, "# Email        : Dengxue.Yan@wuslt.edu\r\n");
    printf(1, "#========================================================================\n");
    printf(1, "\n");

    printf(1, "The complete command input is:\n    ");
    for (index = 0; index < argc; index++)
    {
        if (NULL != argv[index])
        {
            printf(1, "%s ", argv[index]);
        }
        else
        {
            printf(1, "NULL ");
        }
    }
    printf(1, "\n\n");

    parameter2 = 5;// Default fork_times
    test_method = '0';// Default test method
    if (argc >= 2)
    {
        if (NULL != argv[1])
        {
            for (index = 0; ' ' == argv[1][index]; index++);
            if (('\0' == argv[1][index]) || ('\0' != argv[1][index + 1]))
            {
                usage(argv[0]);
                exit();
            }
        }
 
        test_method = argv[1][index];

        if (argc >= 3)
        {
            if (NULL != argv[2])
            {
                parameter2 = simplestrtoul(argv[2]);
            }
        }
    }

    switch (test_method)
    {
        case '0':
        {
            printf(1, "#========================================================================\n");
            printf(1, "# Test method : 0\n");
            printf(1, "# Fork times  : %d\n", parameter2);
            printf(1, "#========================================================================\n");
            printf(1, "\n");

           if ((parameter2 < 1) || (parameter2 > MAX_NUM_CHILD))
           {
               printf(1, "Fork times for test method 0 exceeds the range of [1 %d]!\n", MAX_NUM_CHILD);
               usage(argv[0]);
               exit();
           }

            printf(1, "Test method 0 start...\n");
            test_ret = runnabletest(parameter2);
            printf(1, "Test method 0 end!\n\n");

            if (0 == test_ret)
            {
                printf(1, "All runnable tests passed!\n\n");
            }
            else if (test_ret > 0)
            {
                printf(1, "runnable test failed %d times!\n\n", test_ret);
            }
            else 
            {
                usage(argv[0]);
            }

            break;
        }
        case '1':
        {
            printf(1, "#========================================================================\n");
            printf(1, "# Test method : 1\n");
            printf(1, "# Test times  : %d\n", parameter2);
            printf(1, "#========================================================================\n");

            int success_count = 0;
            int failed_count = 0;
            int unknown_error_count = 0;
            printf(1, "Test method 1 start...\n");
            for (index = 0; index < parameter2; index++)
            {
                test_ret = runnabletest1();
                if (0 == test_ret)
                {
                    printf(1, "    test[%d] passed!\n", index);
                    success_count++;
                }
                else if (test_ret > 0)
                {
                    printf(1, "    test[%d] failed!\n", index);
                    failed_count++;
                }
                else
                {
                    printf(1, "    Unknown error occurs at test[%d]!\n", index);
                    unknown_error_count++;
                }
            }
            printf(1, "Test method 1 end!\n\n");

            printf(1, "Total %d times runnable test. The statictics result is:\n", parameter2);
            printf(1, "    Successful: %d\n", success_count);
            printf(1, "        Failed: %d\n", failed_count);
            printf(1, "  Unkown error: %d\n", unknown_error_count);
            break;
        }
        default:
        {
            printf(1, "unknown test method \"%c\"!\n\n", test_method);
            usage(argv[0]);
            break;
        }
    }

    exit();
}

//----------------------------------------------------------------------------------------------------
// Local Functions
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: usage
//      Input: char *prog_name
//     Output: void
//     Return: void
//Description: Print the usage message
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   Dengxue Yan     09-15-2016 15:30           Create
//----------------------------------------------------------------------------------------------------
static void usage(char *prog_name)
{
    printf(1, "usage: %s [test_method [parameter2]]\n", prog_name);
    printf(1, "    test_method: Optional. Spefify the test method. Only support test method 0 and 1.\n");
    printf(1, "    parameter2 : Optional. When test_method is present:\n");
    printf(1, "        For test method 0: parameter2 specify fork times with range is [1, %d]. If not included, the default fork times is 5\n", MAX_NUM_CHILD);
    printf(1, "        For test method 1: parameter2 specify test times.\n");
}

//----------------------------------------------------------------------------------------------------
//   Function: simplestrtoul
//      Input: const char *str
//     Output: void
//     Return: int
//Description: A simple function to conver a string to decimal unsigned int.
//             The negative return value indicates errors were found in the input string.
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   Dengxue Yan     09-15-2016 15:30           Create
//----------------------------------------------------------------------------------------------------
static int simplestrtoul(const char *str)
{
    int index;
    int value;

    if (NULL == str)
    {
        return -1;
    }

    value = 0;
    for (index = 0; ' ' == str[index]; index++);
    for (; '\0' != str[index]; index++)
    {
        if ((str[index] < '0') || (str[index] > '9'))
        {
            return -1;
        }
        else
        {
            value = value * 10 + str[index] - '0';
        }
    }

    return value;
}

//----------------------------------------------------------------------------------------------------
//   Function: runnabletest
//      Input: int fork_times
//     Output: void
//     Return: int
//Description: 
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   Dengxue Yan     09-15-2016 15:30           Create
//----------------------------------------------------------------------------------------------------
static int runnabletest(int fork_times)
{
    int i;
    int pids[fork_times];
    int runnables;
    int test_failed;

    if ((fork_times < 1) || (fork_times > MAX_NUM_CHILD))
    {
        printf(1, "Fork times exceeds the range of [1 %d]!\n", MAX_NUM_CHILD);
        return -1;
    }

    test_failed = 0;

    runnables = runnable();
    printf(1, "    Original runnable processes is: %d\n", runnables);
    
    for(i = 0; i < fork_times; i++)
    { 
       pids[i] = fork();   //fork a new process
       if (0 == pids[i])// if you're the child, loop forever
       {
          for(;;);
       }
    }

    // at this point, there should be N new processes. The init process should 
    // be in a sleep state after boot, the shell process (sh) should be in a 
    // sleep state after running a process.  So, N + 3 total processes, 2 have 
    // state = SLEEP, 2 have state = RUNNING.  So N - 1 runnable proccesses, 
    // assuming we haven't created any background proccesses uutside of this 
    // program   
    for (i = 0; i < 10000; i++)
    {
        runnables = runnable();
        if (runnables == (fork_times - 1))
        {
            break;
        }
    }
    printf(1, "\n    After forked %d times, runnable processes is: %d\n", fork_times, runnables);
    if (10000 != i)
    {
       printf(1, "        runnable test1: passed\n");
    }
    else
    {
       test_failed++;
       printf(1, "        runnable test1: failed\n");
    }
    
    printf(1, "\n    Then kill child process one by one:\n");
    // kill our running processes one by one
    for(i = 0; i < fork_times - 1; i++)
    {
       kill(pids[i]);
       wait();
       runnables = runnable();
       printf(1, "    killed %d child processes(PID=%d), runnable processes: %d\n", i + 1, pids[i], runnables);
       if(runnables == (fork_times - i - 2))
       {
          printf(1,"        child process kill test: passed\n");
       }
       else
       {
          printf(1,"        child process kill test: failed\n");
          test_failed++;
       }
    }
    
    kill(pids[i]);
    wait();
    runnables = runnable();
    printf(1, "\n    Killed the last child processes(PID=%d), runnable processes: %d\n", pids[i], runnables);
    if (0 == runnables)
    {
        printf(1,"        child process kill test: passed\n");
    }
    else
    {
        test_failed++;
        printf(1,"        child process kill test: failed\n");
    }
    
    for(i = 0; i < fork_times; i++)
    { 
        pids[i] = fork();   //fork a new process
        if (0 == pids[i])// if you're the child, loop forever
        {
           for(;;);
        }
    }

    // at this point, there should be N new processes. The init process should 
    // be in a sleep state after boot, the shell process (sh) should be in a 
    // sleep state after running a process.  So, N + 3 total processes, 2 have 
    // state = SLEEP, 2 have state = RUNNING.  So N - 1 runnable proccesses, 
    // assuming we haven't created any background proccesses uutside of this 
    // program   
    for (i = 0; i < 10000; i++)
    {
        runnables = runnable();
        if (runnables == (fork_times - 1))
        {
            break;
        }
    }
    printf(1, "\n    Fork %d children proecsses again, runnable processes is: %d\n", fork_times, runnables);
    if (10000 != i)
    {
       printf(1,"        runnable test3: passed\n");
    }
    else
    {
       test_failed++;
       printf(1,"        runnable test3: failed\n");
    }
    
    for (i = 0; i < fork_times; i++)
    {
        kill(pids[i]);
    }
    for (i = 0; i < fork_times; i++)
    {
        wait();
    } 

    runnables = runnable();
    printf(1, "\n    Kill all child processes at one time, runnable processes is: %d\n", runnables);
    if (0 == runnables)
    {
       printf(1, "        runnable test4: passed\n");
    }
    else
    {
       test_failed++;
       printf(1, "        runnable test4: failed\n");
    }
    
    return test_failed;
}

//----------------------------------------------------------------------------------------------------
//   Function: runnabletest1
//      Input: void
//     Output: void
//     Return: int
//Description: A simple runnable test
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   Dengxue Yan     09-15-2016 15:30           Create
//----------------------------------------------------------------------------------------------------
static int runnabletest1(void)
{
    int i;
    int test_failed;
    int pids[MAX_NUM_CHILD];
    int runnables;

    test_failed = 0;
    for (i = 0; i < MAX_NUM_CHILD; i++)
    { 
        pids[i] = fork();   //fork a new process
        if (0 == pids[i])// if you're the child, loop forever
        {
           for(;;);
        }
    }

    // at this point, there should be N new processes. The init process should 
    // be in a sleep state after boot, the shell process (sh) should be in a 
    // sleep state after running a process.  So, N + 3 total processes, 2 have 
    // state = SLEEP, 2 have state = RUNNING.  So N - 1 runnable proccesses, 
    // assuming we haven't created any background proccesses uutside of this 
    // program   
    runnables = runnable();
    if(runnables != (MAX_NUM_CHILD - 1))
    {
       test_failed = 1;
    }
    
    for (i = 0; i < MAX_NUM_CHILD; i++)
    {
        kill(pids[i]);
    }

    for (i = 0; i < MAX_NUM_CHILD; i++)
    {
        wait();
    } 

    runnables = runnable();
    if (0 != runnables)
    {
       test_failed = 1;
    }
    
    return test_failed;
}
