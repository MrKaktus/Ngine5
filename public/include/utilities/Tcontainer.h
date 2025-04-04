/*

 Ngine v5.0
 
 Module      : Template for uniform container
 Requirements: none
 Description : Stores elements of any type. 
               It also ensures that id's of 
               elements will be unique (id's
               are in range 1-2^32).
*/

#ifndef ENG_UTILITIES_CONTAINER
#define ENG_UTILITIES_CONTAINER

#include <vector>

#include "core/types.h"

template <typename obj>
class Tcontainer
{
    private: 
    vector<obj*>   m_table;          // Table of elements
    vector<uint32> m_free;           // Free id's

    public:
   ~Tcontainer();
    inline uint32 add(obj* res);        // Returns element id or 0
    inline obj*   get(uint32 id);       // Returns element pointer or nullptr
    inline uint32 getFirst(void);       // Returns id of first element or 0
    inline uint32 getNext(uint32 prev); // Returns id of next element or 0 if reached end of container
    inline bool   del(uint32 id);       // Deletes element or returns false
    inline obj*   remove(uint32 id);    // Removes element from container without deleting it from memory. Pointer to element or nullptr is returned
    inline void   clear(void);          // Clears whole container
    inline uint32 count(void);          // Returns elements count
};

template <typename obj>
Tcontainer<obj>::~Tcontainer()
{
    this->clear();
}

template <typename obj>
inline uint32 Tcontainer<obj>::add(obj* res)
{
    uint32 tid = 0;
    if (m_free.size() > 0)
    {
        tid = m_free[m_free.size()-1];
        m_free.pop_back();
        m_table[tid-1] = res;
    }
    else
    {
        m_table.push_back(res);
        tid = m_table.size();
    }

    return tid;
}

template <typename obj>
inline obj* Tcontainer<obj>::get(uint32 id)
{
    if (id == 0)
    {
        return 0;
    }
    if (m_table.size() < id)
    {
        return 0;
    }

    return m_table[id-1];
}

template <typename obj>
inline uint32 Tcontainer<obj>::getFirst(void)
{
    if (m_table.size() == 0)
    {
        return 0;
    }

    for(uint32 i=0; i<m_table.size(); i++)
    {
        if (m_table[i] != nullptr)
        {
            return i+1;
        }
    }

    return 0;
}

template <typename obj>
inline uint32 Tcontainer<obj>::getNext(uint32 prev)
{
    if (prev == 0)
    {
        return 0;
    }
    if (m_table.size() < prev)
    {
        return 0;
    }

    for(uint32 i=prev; i<m_table.size(); i++)
    {
        if (m_table[i] != nullptr)
        {
            return i+1;
        }
    }

    return 0;
}

template <typename obj>
inline bool Tcontainer<obj>::del(uint32 id)
{
    if (id == 0)
    {
        return false;
    }

    if (m_table.size() < id)
    {
        return false;
    }

    if (m_table[id-1] == nullptr)
    {
        return true;
    }

    delete m_table[id-1];
    if (m_table.size() == id)
    {
        m_table.pop_back();
    }
    else
    {
        m_table[id-1] = (obj*)0;
        m_free.push_back(id);
    }

    return true;
}

template <typename obj>
inline obj* Tcontainer<obj>::remove(uint32 id)
{
    if (id == 0)
    {
        return nullptr;
    }
    if (m_table.size() < id)
    {
        return nullptr;
    }
    if (m_table[id-1] == nullptr)
    {
        return nullptr;
    }
    obj* result = m_table[id-1];
    if (m_table.size() == id)
    {
        m_table.pop_back();
    }
    else
    {
        m_table[id-1] = (obj*)0;
        m_free.push_back(id);
    }

    return result;
}

template <typename obj>
inline void Tcontainer<obj>::clear(void)
{
    m_free.clear();
    for(uint32 i=0; i<m_table.size(); i++)
    {
        if (m_table[i] != nullptr)
        {
            delete m_table[i];                      // <- Removed for performance gain in Dtor.
        }
    }

    m_table.clear();
}

template <typename obj>
inline uint32 Tcontainer<obj>::count(void)
{
    return m_table.size() - m_free.size();
}

#endif
