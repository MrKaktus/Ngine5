/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/parallel/psxThread.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "assert.h"

#include "threading/mutex.h"   // TODO: Should be moved to core/parallel/

#include <cpuid.h>

#if defined(EN_PLATFORM_LINUX)
#include <sched.h>             // for core execution mask
#endif
#if defined(EN_PLATFORM_OSX)
#include <sys/sysctl.h>
#include <mach/mach.h>
#endif

#define CPUID(INFO, LEAF, SUBLEAF) __cpuid_count(LEAF, SUBLEAF, INFO[0], INFO[1], INFO[2], INFO[3])

namespace en
{
// There is several ways to get current thread ID:
//
// 1) Linux:
// #include <sys/types.h>
// pid_t gettid(void);
//
// 2) macOS:
// pthread_threadid_np(nullptr, &tid);
//
// 3) BSD (?) (non mac):
// pthread_id_np_t tid = pthread_getthreadid_np();
//
// 4)
// pthread_t self = pthread_self();
// pthread_id_np_t tid;
// pthread_getunique_np(&self, &tid);
//
uint64 currentThreadSystemId(void)
{
    uint64 systemId;
    pthread_threadid_np(nullptr, &systemId);
    return systemId;
}

void wakeUpMainThread(void)
{
    // TODO: How main thread messages processing looks like in POSIX?
    //       This is OS specific, will interact with Delegate in macOS.
}

void setThreadName(std::string threadName)
{
    // Sets name of current thread
    pthread_setname_np(threadName.c_str());
}

uint32 currentCoreId(void)
{
    // Linux specific implementations:
    //
    // int result = getcpu(unsigned *cpu, unsigned *node, struct getcpu_cache *tcache);
    // assert( result >= 0 );
    //
    // int result = sched_getcpu(void);
    // assert( result >= 0 );

    // WA for macOS:
    uint32 CPUInfo[4];
    CPUID(CPUInfo, 1, 0);

    if ((CPUInfo[3] & (1 << 9)) == 0)
    {
        // There is no APIC on chip
        // This means that application runs on macOS, on CPU older than Pentium 4,
        // which should never happen in real life (it's 2018 as I wrote this code).
        assert( 0 );
    }
        
    // CPUInfo[1] is EBX, bits 24-31 are APIC ID, identifying local executing core
    // TODO: FIXME: This will return core only on up to 8-core machines, so won't
    //              work on iMacPro or MacPro with more cores!
    uint32 core = CPUInfo[1] >> 24;
   
    // For more recent core detection see also:
    // https://stackoverflow.com/questions/22310028/is-there-an-x86-instruction-to-tell-which-core-the-instruction-is-being-run-on
    return core;
}

typedef void*(*ThreadFunctionInternal)(void* thread);

psxThread::psxThread(ThreadFunction function, void* threadState) :
    handle(),
    mutex(PTHREAD_MUTEX_INITIALIZER),
    localState(threadState),
    index(0xFFFFFFFF),
    valid(false),
    Thread()
{
    int ret = pthread_attr_init(&attr);
    assert( ret == 0 );
   
    ret = pthread_create(&handle, (const pthread_attr_t*)&attr, (ThreadFunctionInternal)function, (void*)this);
    assert( ret == 0 );
    if (!ret)
        valid = true;

    // Query this thread system ID.
    uint64 thisThreadSystemId = 0;
    ret = pthread_threadid_np(handle, &thisThreadSystemId);
   
    // pthread_threadid_np() returns 0 instead of thread system Id, if given
    // thread didn't started yet. For example see:
    // http://codemonkeytowriter.blogspot.com/2013/12/weird-pthreadthreadidnp-behavior-in.html
    while(ret == 22) // ESRCH ?
    {
       _mm_pause();
        ret = pthread_threadid_np(handle, &thisThreadSystemId);
    }
    assert( ret == 0 );
    assert( thisThreadSystemId );
   
    // Register unique local thread ID
    index = registerThread(thisThreadSystemId);
}

psxThread::~psxThread()
{
    // Mark that thread as terminated
    releaseThread(index);
   
    pthread_kill(handle, 0);
    pthread_attr_destroy(&attr);
    valid = false;
}

void* psxThread::state(void)
{
    return localState;
}
   
void psxThread::name(std::string threadName)
{
    // Sets name of current thread
    pthread_setname_np(threadName.c_str());
}
   
uint32 psxThread::id(void)
{
    return index;
}

uint64 psxThread::coresExecutionMask(void)
{
    uint64 result = 0;

#if defined(EN_PLATFORM_LINUX)
    cpu_set_t coreSet;
    int psxResult = pthread_getaffinity_np(handle, sizeof(cpu_set_t), &coreSet);
    assert( psxResult == 0 );

    for(uint32 i=0; i<min(64, CPU_SETSIZE); ++i)
        if (CPU_ISSET(i, &coreSet))
            setBit(result, i);
      
#elif defined(EN_PLATFORM_OSX)
    // sched_getaffinity() and pthread_setaffinity_np() are not supported on macOS
    // See also: https://yyshen.github.io/2015/01/18/binding_threads_to_cores_osx.html
   
    // Query count of physical cores
    uint32 physicalCores = 0;
    size_t bufferLength  = sizeof(physicalCores);
    int ret = sysctlbyname("hw.physicalcpu_max", &physicalCores, &bufferLength, nullptr, 0);
    assert( ret == 0 );

    // WA: specify that current thread can run on all CPU cores, as in macOS
    // there is no way to specify thread core execution mask (specifying thread
    // affinity is only a hint).
    for(uint32 i=0; i<physicalCores; ++i)
        setBit(result, i);
   
#else
    // Should never reach this place
    assert( 0 );
   
#endif

    return result;
}

void psxThread::executeOn(const uint64 coresMask)
{
#if defined(EN_PLATFORM_LINUX)
    cpu_set_t coreSet;
    CPU_ZERO(&coreSet);
    for(uint32 i=0; i<64; ++i)
        if (checkBit(coresMask, i))
            CPU_SET(i, &coreSet);

    int ret = pthread_setaffinity_np(handle, sizeof(cpu_set_t), &coreSet);
    assert( ret == 0 );

#elif defined(EN_PLATFORM_OSX)
    // sched_getaffinity() and pthread_setaffinity_np() are not supported on macOS
    // See also: https://yyshen.github.io/2015/01/18/binding_threads_to_cores_osx.html
    thread_affinity_policy_data_t policy = { .affinity_tag = static_cast<integer_t>(coresMask) };
   
    // Thread port shouldn't be deleted as it's cached on posix lib side.
    // See also: https://codereview.chromium.org/276043002/
    mach_port_t threadPort = pthread_mach_thread_np(handle);
    thread_policy_set(threadPort,
                      THREAD_AFFINITY_POLICY,
                      (thread_policy_t)&policy,
                      1);

#else
    // Should never reach this place
    assert( 0 );
   
#endif
}

void psxThread::sleep(void)
{
    // Only target thread can put itself to sleep
    assert( handle == pthread_self() );

    int ret = pthread_cond_init(&cond, nullptr);
    assert( ret == 0 );
   
    // Sleep by waiting for signal
    ret = pthread_cond_wait(&cond, &mutex);
    assert( ret == 0 );
   
    // Alternative implementation:
    //pause();
}

void psxThread::sleepFor(const Time time)
{
    // Only target thread can put itself to sleep
    assert( handle == pthread_self() );

    // TODO: Will mach_wait_until() be woken up by posix signal???
    //       This needs to be interruptable!
    en::sleepFor(time);
}

void psxThread::sleepUntil(const Time time)
{
    // Only target thread can put itself to sleep
    assert( handle == pthread_self() );

    // TODO: Will mach_wait_until() be woken up by posix signal???
    //       This needs to be interruptable!
    en::sleepUntil(time);
}

void psxThread::wakeUp(void)
{
    // Obviously thread cannot wake itself on its own
    assert( handle != pthread_self() );
   
    // Wake-up thread by sending signal
    pthread_cond_signal(&cond);
   
    // Alternative implementation:
    //pthread_kill(handle, SIGCONT);
}

bool psxThread::working(void)
{
    // TODO: This will fail if thread crashes, or is early terminated
    return valid;
}

void psxThread::exit(uint32 ret)
{
    assert( handle == pthread_self() );

    pthread_exit(nullptr);
}
   
void psxThread::waitUntilCompleted(void)
{
    assert( handle != pthread_self() );
   
    pthread_join(handle, nullptr);
}
   
std::unique_ptr<Thread> startThread(ThreadFunction function, void* threadState)
{
    return std::unique_ptr<Thread>( new psxThread(function, threadState) );
}

}
#endif

