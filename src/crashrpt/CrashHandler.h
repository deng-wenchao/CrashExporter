/*! \file   CrashHandler.h
*  \brief  Exception handling and crash report generation
*/

#pragma once

#include <windows.h>
#include <atlstr.h>
#include <map>
#include <dbghelp.h>

// Forward declarations
class CCritSec;
class CSharedMem;
class StackWalker;
class COutStackWalker;

// Project headers
#include "CrashRpt.h"
#include "Utility.h"
#include "CritSec.h"
#include "SharedMem.h"
#include "StackWalker.h"

#include <eh.h>
#include <signal.h>

/**
 * @struct ThreadExceptionHandlers
 * @brief Per-thread exception handler pointers
 */
struct ThreadExceptionHandlers
{
    ThreadExceptionHandlers() 
        : m_prevTerm(nullptr)
        , m_prevUnexp(nullptr)
        , m_prevSigFPE(nullptr)
        , m_prevSigILL(nullptr)
        , m_prevSigSEGV(nullptr)
    {}

    terminate_handler m_prevTerm;       //!< Previous terminate handler
    unexpected_handler m_prevUnexp;     //!< Previous unexpected handler
    void (__cdecl* m_prevSigFPE)(int);  //!< Previous FPE handler
    void (__cdecl* m_prevSigILL)(int);  //!< Previous SIGILL handler
    void (__cdecl* m_prevSigSEGV)(int); //!< Previous SIGSEGV handler
};

/**
 * @class CCrashHandler
 * @brief Main crash handling class - installs handlers, catches exceptions, generates reports
 */
class CCrashHandler
{
public:
    /**
     * @brief Get singleton instance for current process
     * @return Pointer to crash handler instance
     */
    static CCrashHandler* GetCurrentProcessCrashHandler();

    /**
     * @brief Release singleton instance
     */
    static void ReleaseCurrentProcessCrashHandler();

    /**
     * @brief Default constructor
     */
    CCrashHandler();

    /**
     * @brief Destructor - cleans up resources
     */
    virtual ~CCrashHandler();

    // Disable copy operations
    CCrashHandler(const CCrashHandler&) = delete;
    CCrashHandler& operator=(const CCrashHandler&) = delete;

    /**
     * @brief Initialize crash handler with configuration
     * @param dwFlags Installation flags
     * @param MiniDumpType Type of minidump to generate
     * @param lpcszRestartCmdLine Command line for app restart (optional)
     * @return 0 on success
     */
    int Init(DWORD dwFlags = 0,
             MINIDUMP_TYPE MiniDumpType = MiniDumpNormal,
             LPCTSTR lpcszRestartCmdLine = nullptr);

    /**
     * @brief Check if handler was initialized
     * @return TRUE if initialized
     */
    BOOL IsInitialized() const { return m_bInitialized; }

    /**
     * @brief Clean up and release all resources
     * @return 0 on success
     */
    int Destroy();

    /**
     * @brief Add screenshot to error report
     * @param dwFlags Screenshot flags
     * @return 0 on success
     */
    int AddScreenshot(DWORD dwFlags);

    /**
     * @brief Generate error report from exception info
     * @param pExceptionInfo Exception information (optional)
     * @return 0 on success
     */
    int GenerateErrorReport(CR_EXCEPTION_INFO* pExceptionInfo = nullptr);

    /**
     * @brief Install process-wide exception handlers
     * @param dwFlags Handler types to install
     * @return 0 on success
     */
    int SetProcessExceptionHandlers(DWORD dwFlags);

    /**
     * @brief Remove process-wide exception handlers
     * @return 0 on success
     */
    int UnSetProcessExceptionHandlers();

    /**
     * @brief Install thread-specific exception handlers
     * @param dwFlags Handler types to install
     * @return 0 on success
     */
    int SetThreadExceptionHandlers(DWORD dwFlags);

    /**
     * @brief Remove thread-specific exception handlers
     * @return 0 on success
     */
    int UnSetThreadExceptionHandlers();

    /**
     * @brief Get installation flags
     * @return Flags value
     */
    DWORD GetFlags() const { return m_dwFlags; }

private:
    // === Exception Handler Functions ===

    static LONG WINAPI SehHandler(PEXCEPTION_POINTERS pExceptionPtrs);
    static DWORD WINAPI StackOverflowThreadFunction(LPVOID threadParameter);
    static void __cdecl TerminateHandler();
    static void __cdecl UnexpectedHandler();
    static void __cdecl PureCallHandler();
    
    static void __cdecl InvalidParameterHandler(
        const wchar_t* expression,
        const wchar_t* function,
        const wchar_t* file,
        unsigned int line,
        uintptr_t pReserved);

    static int __cdecl NewHandler(size_t size);

    static void SigabrtHandler(int code);
    static void SigfpeHandler(int code, int subcode);
    static void SigintHandler(int code);
    static void SigillHandler(int code);
    static void SigsegvHandler(int code);
    static void SigtermHandler(int code);

    // === Crash Report Generation ===

    void GetExceptionPointers(DWORD dwExceptionCode, EXCEPTION_POINTERS* pExceptionPointers);
    
    CRASH_DESCRIPTION* PackCrashInfoIntoSharedMem(CSharedMem* pSharedMem);
    
    DWORD PackString(const CString& str);
    
    int LaunchcrashExporter(LPCTSTR szCmdLineParams, 
                           BOOL bWait, 
                           HANDLE* phProcess = nullptr);
    
    BOOL IsExportProcessAlive();

    void InitPrevExceptionHandlerPointers();
    
    int PerCrashInit();
    
    void CrashLock(BOOL bLock);

    // === Member Variables ===

    static CCrashHandler* m_pProcessCrashHandler;  //!< Process singleton

    // Previous handler pointers
    LPTOP_LEVEL_EXCEPTION_FILTER m_oldSehHandler;  //!< Previous SEH handler
    _purecall_handler m_prevPurec;                  //!< Previous pure call handler
    _PNH m_prevNewHandler;                          //!< Previous new handler
    _invalid_parameter_handler m_prevInvpar;        //!< Previous invalid param handler
    void (__cdecl* m_prevSigABRT)(int);             //!< Previous SIGABRT
    void (__cdecl* m_prevSigINT)(int);              //!< Previous SIGINT
    void (__cdecl* m_prevSigTERM)(int);             //!< Previous SIGTERM

    std::map<DWORD, ThreadExceptionHandlers> m_ThreadExceptionHandlers; //!< Per-thread handlers
    CCritSec m_csThreadExceptionHandlers;           //!< Thread map lock

    // State variables
    BOOL m_bInitialized;            //!< Initialization flag
    DWORD m_dwFlags;                //!< Installation flags
    MINIDUMP_TYPE m_MinidumpType;   //!< Minidump type
    CString m_sImageName;           //!< Process image name
    CString m_sRestartCmdLine;      //!< Restart command line

    // Screenshot settings
    BOOL m_bAddScreenshot;          //!< Include screenshot flag
    DWORD m_dwScreenshotFlags;      //!< Screenshot flags

    // Synchronization
    CCritSec m_csCrashLock;         //!< Crash lock
    HANDLE m_hEvent;                //!< Sync event 1
    HANDLE m_hEvent2;               //!< Sync event 2

    // Shared memory
    CSharedMem m_SharedMem;                     //!< Shared memory object
    CRASH_DESCRIPTION* m_pCrashDesc;            //!< Crash description pointer
    CSharedMem* m_pTmpSharedMem;                //!< Temporary shared mem
    CRASH_DESCRIPTION* m_pTmpCrashDesc;         //!< Temporary crash desc

    // Export process
    HANDLE m_hExportProcess;        //!< crashExporter.exe process handle
    BOOL m_bContinueExecution;      //!< Continue after crash flag

    // Paths
    CString m_sPathToDebugHelpDll;      //!< Path to dbghelp.dll
    CString m_sPathTocrashExporter;     //!< Path to crashExporter.exe

    // Stack walking
    COutStackWalker m_StackWalker;  //!< Stack walker for output
};
