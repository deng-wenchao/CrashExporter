/*! \file  CrashInfoReader.h
*  \brief  Retrieves crash information from CrashRpt.dll via shared memory
*/

#pragma once

#include <windows.h>
#include <dbghelp.h>

// Forward declarations
class CSharedMem;
struct CRASH_DESCRIPTION;

/**
 * @class CCrashInfoReader
 * @brief Reads and unpacks crash information from shared memory
 */
class CCrashInfoReader
{
public:
    // === Public Member Variables ===

    // Identification
    CString m_sCrashGUID;           //!< Unique crash identifier
    CString m_sImageName;           //!< Path to application executable
    CString m_sAppName;             //!< Application name (without path/extension)
    CString m_sCallstack;           //!< Call stack string

    // Configuration
    CString m_sDbgHelpPath;         //!< Path to dbghelp.dll
    BOOL m_bShowMeeagebox;          //!< Show notification messagebox
    BOOL m_bAppRestart;             //!< Restart crashed application
    CString m_sRestartCmdLine;      //!< Command line for restart

    // Report generation options
    BOOL m_bGenerateMinidump;       //!< Generate minidump file
    BOOL m_bGenerateCrashWalk;      //!< Generate crash walk file
    MINIDUMP_TYPE m_MinidumpType;   //!< Minidump type
    BOOL m_bAddScreenshot;          //!< Include screenshot
    DWORD m_dwScreenshotFlags;      //!< Screenshot flags

    // Exception information
    DWORD m_dwProcessId;            //!< Process ID at crash
    DWORD m_dwThreadId;             //!< Thread ID at crash
    PEXCEPTION_POINTERS m_pExInfo;  //!< Exception pointers
    int m_nExceptionType;           //!< Exception type
    DWORD m_dwExceptionCode;        //!< SEH exception code
    UINT m_uFPESubcode;             //!< FPE subcode

    // Invalid parameter info
    CString m_sInvParamExpr;        //!< Expression that failed
    CString m_sInvParamFunction;    //!< Function name
    CString m_sInvParamFile;        //!< Source file
    UINT m_uInvParamLine;           //!< Source line number

    // System information
    CString m_sSystemTime;                  //!< Time of crash
    ULONG64 m_dwExceptionAddress;           //!< Exception address
    CString m_sMemUsage;                    //!< Memory usage at crash
    CString m_sOSName;                      //!< OS friendly name
    BOOL m_bOSIs64Bit;                      //!< Is 64-bit OS

    // === Constructor/Destructor ===

    CCrashInfoReader();
    ~CCrashInfoReader() = default;

    // Disable copy operations
    CCrashInfoReader(const CCrashInfoReader&) = delete;
    CCrashInfoReader& operator=(const CCrashInfoReader&) = delete;

    /**
     * @brief Initialize from shared memory
     * @param szFileMappingName Name of shared memory mapping
     * @return 0 on success
     */
    int Init(LPCTSTR szFileMappingName);

    /**
     * @brief Unpack crash description from shared memory
     * @return 0 on success
     */
    int UnpackCrashDescription();

private:
    /**
     * @brief Unpack a string from shared memory
     * @param dwOffset Offset in shared memory
     * @param str Output string
     * @return 0 on success
     */
    int UnpackString(DWORD dwOffset, CString& str);

    /**
     * @brief Collect miscellaneous crash information
     */
    void CollectMiscCrashInfo();

    CSharedMem* m_SharedMem;        //!< Shared memory object
    CRASH_DESCRIPTION* m_pCrashDesc; //!< Crash description pointer
};
