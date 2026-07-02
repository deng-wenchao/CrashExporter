/*! \file  StackWalker.h
*  \brief  Stack walking and call stack display functionality
*/

#pragma once

#include <windows.h>
#include <vector>

/**
 * @class StackWalker
 * @brief Displays the call stack in human-readable form
 */
class StackWalker
{
public:
    /**
     * @brief Constructor
     * @param dwProcessId Process ID (default: current process)
     * @param hProcess Process handle (default: current process)
     */
    explicit StackWalker(DWORD dwProcessId = GetCurrentProcessId(), 
                         HANDLE hProcess = GetCurrentProcess());
    
    virtual ~StackWalker();

    // Disable copy operations
    StackWalker(const StackWalker&) = delete;
    StackWalker& operator=(const StackWalker&) = delete;

    /**
     * @brief Record and display the call stack
     * @param context CPU context at point of interest
     * @param hThread Thread handle (default: current thread)
     */
    void ShowCallstack(const CONTEXT* context, HANDLE hThread = GetCurrentThread());

protected:
    /**
     * @brief Output text to debugger
     * @param szText Text to output
     */
    virtual void Print(LPCWSTR szText);

private:
    /**
     * @brief Build symbol search path for symbol handler
     * @return Search path string (caller must free)
     */
    LPWSTR buildSymbolSearchPath();

    /**
     * @brief Initialize symbol handler for function names and line numbers
     * @return TRUE on success
     */
    BOOL InitSymbolHandler();

    /**
     * @brief Load module information (try ToolHelp32, then PSAPI)
     * @param hProcess Process handle
     * @param dwProcessId Process ID
     * @return TRUE on success
     */
    BOOL LoadModules(HANDLE hProcess, DWORD dwProcessId);

    /**
     * @brief Walk the stack and collect frame addresses
     * @param context CPU context
     * @param hThread Thread handle
     */
    void getStackTrace(const CONTEXT* context, HANDLE hThread);

    /**
     * @brief Dump formatted call stack with symbolic information
     * @param showinternalframes TRUE to show all frames
     */
    void dump(BOOL showinternalframes);

    /**
     * @brief Get module list using ToolHelp32
     * @param hProcess Process handle
     * @param pid Process ID
     * @return TRUE on success
     */
    BOOL GetModuleListTH32(HANDLE hProcess, DWORD pid);

    /**
     * @brief Get module list using PSAPI
     * @param hProcess Process handle
     * @return TRUE on success
     */
    BOOL GetModuleListPSAPI(HANDLE hProcess);

    /**
     * @brief Check if filename is a compiler internal module
     * @param filename Module file path to check
     * @return TRUE if internal module
     */
    bool isInternalModule(const PWSTR filename);

    /**
     * @brief Output formatted message to debugger
     * @param format Format string
     * @param ... Format arguments
     */
    void Report(LPCWSTR format, ...);

    std::vector<UINT_PTR> m_frameArray;  //!< Stack frame addresses
    HANDLE m_hProcess;                    //!< Process handle
    DWORD m_dwProcessId;                  //!< Process ID
};

/**
 * @class COutStackWalker
 * @brief StackWalker that captures output to a CString
 */
class COutStackWalker : public StackWalker
{
public:
    COutStackWalker() : StackWalker() {}
    
    explicit COutStackWalker(DWORD dwProcessId, HANDLE hProcess) 
        : StackWalker(dwProcessId, hProcess) {}

    /**
     * @brief Get captured stack walker output
     * @return Stack trace as CString
     */
    CString GetStackWalkerInfo() const { return m_sOutStackWalker; }

    /**
     * @brief Override Print to capture output
     * @param szText Text to print
     */
    virtual void Print(LPCWSTR szText) override
    {
        m_sOutStackWalker += szText;
        StackWalker::Print(szText);
    }

private:
    CString m_sOutStackWalker;  //!< Captured stack trace output
};
