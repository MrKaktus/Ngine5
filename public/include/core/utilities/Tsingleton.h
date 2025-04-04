/*

 Ngine v5.0
 
 Module      : Singleton template.
 Requirements: none
 Description : Allows easy creation of highly maneagable
               thread-safe singletons. These singletons
               gives programmer the ability to decide when
               to init and destroy them.

*/

#ifndef ENG_CORE_UTILITIES_TSINGLETON
#define ENG_CORE_UTILITIES_TSINGLETON

#include "core/defines.h"
#include "core/threading/atomics.h"

// It is not allowed to 
//#ifndef EN_PLATFORM_ANDROID

// This singleton needs to be inherited publicly by the class
// that wants to be a singleton. Such class should also set
// singleton template as friend class like below:
//
// class cachealign MyClass : public Tsingleton<MyClass>
//      {
//      private:
//      MyClass();
//      friend Tsingleton<MyClass>;
//
// Class that will inherit from singleton template should 
// also be cache aligned to prevent cache hazards.
// To reset the singleton just delete it and then get new 
// reference or pointer which will automatically create it 
// again.
template <typename T> 
class cachealign Tsingleton
{
    // Template constructor is protected to prevent creation
    // of other copies of singleton and to allow child class
    // to perform call to it during singleton construction at 
    // the same time.
    protected:
    Tsingleton(){}

    // Copy constructor and assigment operator are directly 
    // declared as private to prevent creation of copies of 
    // singleton.
    private:
    Tsingleton(const Tsingleton&){}    
    Tsingleton& operator= (const Tsingleton&){}

    // This is pointer to singleton class that inherits from
    // this template. Using pointer allows programmer to control
    // creation and destruction time of the singleton. It is not
    // volatile to prevent memory barriers in other places of the
    // code.
    static T* m_singleton; 
    volatile static uint32 m_lock; // It is static because there can be only one instance of class

    public:
    // Singleton destructor is public so there can be order,
    // set by programmer, in which different singletons will
    // be destroyed.
   ~Tsingleton(); 

    forceinline static void create(void);
    forceinline static T&   getInstance(void);
    forceinline static T*   getPointer(void);
};

template <typename T> 
T* Tsingleton<T>::m_singleton(0);

template <typename T> 
volatile uint32 Tsingleton<T>::m_lock(0);

template <typename T>
Tsingleton<T>::~Tsingleton()
{
    // Just set singleton as deleted. Full destruction of
    // singleton is handled by class that inheriths from 
    // this template.
    m_singleton = 0;
}

template <typename T>
forceinline void Tsingleton<T>::create(void)
{
    // This is 'while' not 'if' because you need to wait with 
    // return of m_singleton from this function until it will 
    // be sure that other thread finished initializing it.
    while(m_singleton == 0)
    {
        if (CompareAndSwap(&m_lock,1,0))
        {
            if (m_singleton == 0)
            {
                volatile T* temp = (volatile T*)new T();
                // TODO: Add memory barierr here !
                m_singleton = (T*)temp;
            }
            m_lock = 0;
        }
    } 
}

template <typename T>
forceinline T& Tsingleton<T>::getInstance( void )
{
    create();
    return *m_singleton;  
}

template <typename T>
forceinline T* Tsingleton<T>::getPointer( void )
{
    create();
    return m_singleton; 
}

//#endif

#endif