/*

 Ngine v5.0
 
 Module      : Threads and task pooling support.
 Requirements: none
 Description : Enables easy interface to features
               of task pooling and parallel execution.
               This file contains declaration of
               task that will execute application 
               main function.

*/

#include "threading/maintask.h"

MainTask::MainTask(int argc, char* argv[]) : en::threads::Task(new en::threads::TaskState())
{
 m_argc   = argc;
 m_argv   = argv;
 m_return = 0;
}

void MainTask::work(void)
{
 m_return = ApplicationMainC(m_argc, m_argv);
}

uint32 MainTask::returned(void)
{
 return m_return;
}