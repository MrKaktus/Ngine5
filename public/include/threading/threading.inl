namespace en
{
   namespace threads
   {

   forceinline TaskState::TaskState() : 
      busy(0)    // We could init it to 1 to indicate it is used by first thread
   {
   }
   
   forceinline bool TaskState::working(void) const
   {
   return busy != 0;
   }

   forceinline Task::Task(TaskState* state) :
      state(state)
   {
   }
   
   forceinline TaskState* Task::run(void)
   {
   return Scheduler.run(this);
   }

   forceinline bool Task::running(void)
   {
   if (state)
      return state->working();
   return false;
   }

   }
}