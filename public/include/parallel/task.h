/*

 Ngine v5.0
 
 Module      : Task
 Requirements: none
 Description : Base for application tasks that can be run by Scheduler.

*/

#ifndef ENG_PARALLEL_TASK
#define ENG_PARALLEL_TASK

#include "core/defines.h"
#include "core/types.h"
#include "parallel/sharedAtomic.h"

namespace en
{

// Task execution state is tracked by atomic counter, that represents amount
// of worker threads that are processing this task (or have it in it's queue).
// Counter is increased to 1, when task is passed for execution, and finally
// decreased to 0 when task is finished.
class TaskState : private SharedAtomic
{
    public:
    TaskState();
    void acquire(void);  // Task is acquired by worker thread (scheduled for execution)
    void release(void);  // Task is released by worker thread (finished working on this task)
    bool finished(void); // Returns true, if given task is finished
};

static_assert(sizeof(TaskState) == 64, "en::TaskState size mismatch!");

// Task is just a function that will be executed with any optional data
// that it needs for processing. This data won't be copied (as task private
// copy) while task is pushed for execution. Therefore ensure that any 
// external data or state reffered by task is available until task is
// processed, or pass data ownership to task itself (preffered method).
// Task is responsible for releasing data passed to it.
typedef void(*TaskFunction)(void* taskData);

} // en

#endif
