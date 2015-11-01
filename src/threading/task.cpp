
#include "core/threading/atomics.h"
#include "threading/task.h"

//*****************************************************************************

void NTaskState::set(bool state)
{
 // Several threads at a time can try to
 // change state of this task if it is
 // splitted for parallel processing.
 if (state)
      AtomicIncrease(&m_busy);
 else AtomicDecrease(&m_busy);
}

//*****************************************************************************


