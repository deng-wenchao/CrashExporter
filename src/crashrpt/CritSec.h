/*! \file  CritSec.h
*  \brief  Critical section wrapper classes with RAII support
*/

#ifndef _CRITSEC_H
#define _CRITSEC_H

#include <windows.h>

/**
 * @class CCritSec
 * @brief Wrapper for Windows critical section with RAII semantics
 */
class CCritSec
{
public:
    CCritSec()
    {
        InitializeCriticalSection(&m_CritSec);
    }

    ~CCritSec()
    {
        DeleteCriticalSection(&m_CritSec);
    }

    void Lock()
    {
        EnterCriticalSection(&m_CritSec);
    }

    void Unlock()
    {
        LeaveCriticalSection(&m_CritSec);
    }

    // Disable copy constructor and assignment operator
    CCritSec(const CCritSec&) = delete;
    CCritSec& operator=(const CCritSec&) = delete;

private:
    CRITICAL_SECTION m_CritSec;
};

/**
 * @class CAutoLock
 * @brief RAII lock guard for critical sections
 */
class CAutoLock
{
public:
    explicit CAutoLock(CCritSec* plock) : m_pLock(plock)
    {
        m_pLock->Lock();
    }

    ~CAutoLock()
    {
        m_pLock->Unlock();
    }

    // Disable copy constructor and assignment operator
    CAutoLock(const CAutoLock&) = delete;
    CAutoLock& operator=(const CAutoLock&) = delete;

private:
    CCritSec* m_pLock;
};

#endif // _CRITSEC_H
