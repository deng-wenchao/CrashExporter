/*! \file  Utility.h
*  \brief  Miscellaneous helper functions
*/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <windows.h>
#include <atlstr.h>
#include <cstdint>

/**
 * @namespace Utility
 * @brief Collection of utility helper functions
 */
namespace Utility
{
    /**
     * @brief Get base name of the EXE file for current process
     * @return Application name without extension
     */
    CString getAppName();

    /**
     * @brief Get path to directory containing the specified module
     * @param hModule Module handle (NULL for current executable)
     * @return Directory path
     */
    CString GetModulePath(HMODULE hModule);

    /**
     * @brief Get full path and filename of the specified module
     * @param hModule Module handle (NULL for current executable)
     * @return Full module path
     */
    CString GetModuleName(HMODULE hModule);

    /**
     * @brief Generate a unique identifier (GUID)
     * @param sGUID Output string to receive GUID
     * @return 0 on success
     */
    int GenerateGUID(CString& sGUID);

    /**
     * @brief Get current system time as formatted string
     * @param sTime Output string to receive time
     */
    void GetSystemTime(CString& sTime);

    /**
     * @brief Get friendly OS name including version and service pack
     * @param sOSName Output string to receive OS name
     * @return 0 on success
     */
    int GetOSFriendlyName(CString& sOSName);

    /**
     * @brief Check if Windows is 64-bit
     * @return TRUE if 64-bit OS
     */
    BOOL IsOS64Bit();

    /**
     * @brief Format an error message from error code
     * @param dwErrorCode Windows error code
     * @return Formatted error message
     */
    CString FormatErrorMsg(DWORD dwErrorCode);

    /**
     * @brief Create a folder and any necessary parent directories
     * @param sFolderName Path to create
     * @return TRUE on success
     */
    BOOL CreateFolder(const CString& sFolderName);

    /**
     * @brief Convert SYSTEMTIME to UINT64
     * @param st System time structure
     * @return Time as UINT64 value
     */
    ULONG64 SystemTimeToULONG64(const SYSTEMTIME& st);

    /**
     * @brief Append a wide string to another, growing buffer as needed
     * @param dest Destination string (address of pointer)
     * @param source Source string to append
     * @return New concatenated string pointer
     */
    LPWSTR AppendString(LPWSTR dest, LPCWSTR source);

    /**
     * @brief Get calling module by address
     * @param pCaller Caller address
     * @return Module handle
     */
    HMODULE GetCallingModule(UINT_PTR pCaller);

    /**
     * @brief Output debug string (wide character version)
     * @param format Format string
     * @param ... Format arguments
     */
    void OutDebugStrW(LPCWSTR format, ...);

    /**
     * @brief Output debug string (ANSI version)
     * @param format Format string
     * @param ... Format arguments
     */
    void OutDebugStrA(LPCSTR format, ...);

#ifdef UNICODE
#define OutDebugStr OutDebugStrW
#else
#define OutDebugStr OutDebugStrA
#endif // UNICODE

#ifdef _DEBUG
#define DbgTrace(...)  // OutDebugStr(__VA_ARGS__) // Enable when needed
#else
#define DbgTrace(...)
#endif

} // namespace Utility

#endif // _UTILITY_H_
