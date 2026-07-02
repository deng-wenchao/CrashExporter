/*! \file  SharedMem.h
*  \brief  Shared memory class for transferring data between crashed app and crash export process
*/

#pragma once

#include <windows.h>
#include <atlstr.h>
#include <map>
#include <cstdint>

// Forward declaration
#ifndef _MINIDUMP_API_
typedef enum _MINIDUMP_TYPE {
    MiniDumpNormal = 0x00000000,
} MINIDUMP_TYPE;
#endif

#ifndef _EXCEPTION_POINTERS_DEFINED
struct _EXCEPTION_POINTERS;
typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS;
typedef EXCEPTION_POINTERS* PEXCEPTION_POINTERS;
#endif

/**
 * @struct STRING_DESC
 * @brief String block description - followed by string data
 */
struct STRING_DESC
{
    BYTE m_uchMagic[3];  //!< Magic sequence "STR"
    WORD m_wSize;        //!< Total bytes occupied by this block
};

/**
 * @struct CRASH_DESCRIPTION
 * @brief Crash description structure stored in shared memory
 */
struct CRASH_DESCRIPTION
{
    BYTE m_uchMagic[3];                 //!< Magic sequence "CRD"
    WORD m_wSize;                       //!< Total bytes occupied by this block
    DWORD m_dwTotalSize;                //!< Total size of the whole used shared mem
    UINT m_uFileItems;                  //!< Count of file item records
    UINT m_uCustomProps;                //!< Count of user-defined properties
    DWORD m_dwInstallFlags;             //!< Flags passed to crInstall() function
    MINIDUMP_TYPE m_MinidumpType;       //!< Minidump type
    BOOL m_bAddScreenshot;              //!< Add screenshot?
    DWORD m_dwScreenshotFlags;          //!< Screenshot flags
    DWORD m_dwRestartCmdLineOffs;       //!< Offset of app restart command line
    DWORD m_dwCrashGUIDOffs;            //!< Offset to crash GUID
    DWORD m_dwPathToDebugHelpDllOffs;   //!< Offset of dbghelp path
    DWORD m_dwImageNameOffs;            //!< Offset to image name
    DWORD m_dwProcessId;                //!< Process ID
    DWORD m_dwThreadId;                 //!< Thread ID
    int m_nExceptionType;               //!< Exception type
    DWORD m_dwExceptionCode;            //!< SEH exception code
    DWORD m_dwInvParamExprOffs;         //!< Invalid parameter expression offset
    DWORD m_dwInvParamFunctionOffs;     //!< Invalid parameter function offset
    DWORD m_dwInvParamFileOffs;         //!< Invalid parameter file offset
    UINT m_uInvParamLine;               //!< Invalid parameter line
    UINT m_uFPESubcode;                 //!< FPE subcode
    PEXCEPTION_POINTERS m_pExceptionPtrs; //!< Exception pointers
    DWORD m_dwStackWalkerOffs;          //!< StackWalker info offset
};

constexpr DWORD SHARED_MEM_MAX_SIZE = 10 * 1024 * 1024;  // 10 MB

/**
 * @class CSharedMem
 * @brief Manages shared memory between CrashRpt.dll and crashExporter.exe
 */
class CSharedMem
{
public:
    CSharedMem();
    ~CSharedMem();

    // Disable copy operations
    CSharedMem(const CSharedMem&) = delete;
    CSharedMem& operator=(const CSharedMem&) = delete;

    /**
     * @brief Initialize shared memory
     * @param szName Name of the shared memory region
     * @param bOpenExisting TRUE to open existing, FALSE to create new
     * @param uSize Size in bytes
     * @return TRUE on success
     */
    BOOL Init(LPCTSTR szName, BOOL bOpenExisting, ULONG64 uSize);

    /**
     * @brief Check if initialized
     * @return TRUE if initialized
     */
    BOOL IsInitialized() const { return m_hFileMapping != nullptr; }

    /**
     * @brief Destroy shared memory resources
     */
    void Destroy();

    /**
     * @brief Get shared memory name
     * @return Name string
     */
    CString GetName() const { return m_sName; }

    /**
     * @brief Get shared memory size
     * @return Size in bytes
     */
    ULONG64 GetSize() const { return m_uSize; }

    /**
     * @brief Create a view of shared memory
     * @param dwOffset Offset from beginning
     * @param dwLength Length of view
     * @return Pointer to view start
     */
    LPBYTE CreateView(DWORD dwOffset, DWORD dwLength);

    /**
     * @brief Destroy a view
     * @param pViewPtr Pointer to view to destroy
     */
    void DestroyView(LPBYTE pViewPtr);

private:
    CString m_sName;                        //!< Name of the file mapping
    HANDLE m_hFileMapping;                  //!< Memory mapped object handle
    DWORD m_dwAllocGranularity;             //!< System allocation granularity
    ULONG64 m_uSize;                        //!< Size of the file mapping
    std::map<LPBYTE, LPBYTE> m_aViewStartPtrs; //!< Map of view pointers
};
