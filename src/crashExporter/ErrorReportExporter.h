/*! \file  ErrorReportExporter.h
*  \brief  Export crash error information (minidump, stack walk, screenshot, etc.)
*/

#pragma once

#include <windows.h>
#include <vector>
#include "AssyncNotification.h"
#include "CrashInfoReader.h"

/**
 * @class CErrorReportExporter
 * @brief Main class for collecting and exporting crash report files
 */
class CErrorReportExporter
{
public:
    /**
     * @brief Action type enumeration
     */
    enum ActionType
    {
        COLLECT_CRASH_INFO = 0x01,  //!< Collect crash info
        RESTART_APP        = 0x02   //!< Restart crashed application
    };

    /**
     * @brief Constructor
     */
    CErrorReportExporter();

    /**
     * @brief Destructor - cleans up resources
     */
    virtual ~CErrorReportExporter();

    // Disable copy operations
    CErrorReportExporter(const CErrorReportExporter&) = delete;
    CErrorReportExporter& operator=(const CErrorReportExporter&) = delete;

    /**
     * @brief Get singleton instance
     * @return Pointer to singleton instance
     */
    static CErrorReportExporter* GetInstance();

    /**
     * @brief Initialize with shared memory name
     * @param szFileMappingName Name of shared memory mapping
     * @return TRUE on success
     */
    BOOL Init(LPCTSTR szFileMappingName);

    /**
     * @brief Clean up temporary files and finalize
     * @return TRUE on success
     */
    BOOL Finalize();

    /**
     * @brief Get crash information reader
     * @return Pointer to crash info reader
     */
    CCrashInfoReader* GetCrashInfo() { return &m_CrashInfo; }

    /**
     * @brief Get error report directory path
     * @return Directory path
     */
    CString GetErrorReportDir() const { return m_sErrorReportDirName; }

    /**
     * @brief Wait for async operation to complete
     */
    void WaitForCompletion();

    /**
     * @brief Get current operation status
     * @param nProgressPct Output: progress percentage
     * @param msg_log Output: log messages
     */
    void GetCurOpStatus(int& nProgressPct, std::vector<CString>& msg_log);

    /**
     * @brief Export crash report to disk
     */
    void ExportReport();

    /**
     * @brief Terminate all running crashExporter.exe processes
     * @return 0 on success
     */
    static int TerminateAllcrashExporterProcesses();

private:
    /**
     * @brief Perform work synchronously
     * @param Action Action to perform
     * @return TRUE on success
     */
    BOOL DoWork(int Action);

    /**
     * @brief Worker thread procedure
     * @param lpParam Thread parameter (this pointer)
     * @return Exit code
     */
    static DWORD WINAPI WorkerThread(LPVOID lpParam);

    /**
     * @brief Run action(s) asynchronously
     * @param Action Action to perform
     * @return TRUE on success
     */
    BOOL DoWorkAssync(int Action);

    /**
     * @brief Take desktop screenshot
     * @return TRUE on success
     */
    BOOL TakeDesktopScreenshot();

    /**
     * @brief Create minidump file
     * @return TRUE on success
     */
    BOOL CreateMiniDump();

    /**
     * @brief Create crash info text file
     * @return TRUE on success
     */
    BOOL CreateCrashInfo();

    /**
     * @brief Build crash info string
     * @return Crash info text
     */
    CString BuildCrashInfo();

    /**
     * @brief Set privileges required for MiniDumpWriteDump
     * @return TRUE on success
     */
    BOOL SetDumpPrivileges();

    /**
     * @brief Minidump callback function
     * @param CallbackParam User parameter
     * @param CallbackInput Callback input data
     * @param CallbackOutput Callback output data
     * @return TRUE to continue
     */
    static BOOL CALLBACK MiniDumpCallback(PVOID CallbackParam,
                                          PMINIDUMP_CALLBACK_INPUT CallbackInput,
                                          PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);

    /**
     * @brief Handle minidump progress
     * @param CallbackInput Callback input data
     * @param CallbackOutput Callback output data
     * @return TRUE to continue
     */
    BOOL OnMinidumpProgress(PMINIDUMP_CALLBACK_INPUT CallbackInput,
                           PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);

    /**
     * @brief Restart the crashed application
     * @return TRUE on success
     */
    BOOL RestartApp();

    /**
     * @brief Unblock parent process
     */
    void UnblockParentProcess();

    // === Member Variables ===

    static CErrorReportExporter* m_pInstance;  //!< Singleton instance
    CCrashInfoReader m_CrashInfo;               //!< Crash information
    HANDLE m_hThread;                           //!< Worker thread handle
    AssyncNotification m_Assync;                //!< Async notification
    int m_Action;                               //!< Current action
    CString m_sErrorReportDirName;              //!< Error report directory
};
