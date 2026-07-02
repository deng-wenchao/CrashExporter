/*! \file  ScreenCap.h
*  \brief  Desktop screenshot capture functionality
*/

#ifndef __SCREENCAP_H__
#define __SCREENCAP_H__

#include <windows.h>
#include <vector>
#include <ctime>

extern "C" {
#include "png.h"
}

/**
 * @struct WindowInfo
 * @brief Information about a window
 */
struct WindowInfo
{
    CString m_sTitle;   //!< Window title
    CRect m_rcWnd;      //!< Window rectangle
    DWORD dwStyle;      //!< Window style
    DWORD dwExStyle;    //!< Extended window style
};

/**
 * @struct MonitorInfo
 * @brief Information about a monitor
 */
struct MonitorInfo
{
    CString m_sDeviceID;    //!< Device identifier
    CRect m_rcMonitor;      //!< Monitor rectangle in screen coordinates
    CString m_sFileName;    //!< Output image filename
};

/**
 * @struct ScreenshotInfo
 * @brief Complete screenshot capture information
 */
struct ScreenshotInfo
{
    ScreenshotInfo() : m_bValid(FALSE), m_CreateTime(0) {}

    BOOL m_bValid;                      //!< TRUE if structure is valid
    time_t m_CreateTime;                //!< Capture timestamp
    CRect m_rcVirtualScreen;            //!< Virtual screen coordinates
    std::vector<MonitorInfo> m_aMonitors;   //!< Captured monitors
    std::vector<WindowInfo> m_aWindows;     //!< Captured windows
};

/**
 * @enum SCREENSHOT_TYPE
 * @brief Types of screenshots
 */
enum SCREENSHOT_TYPE
{
    SCREENSHOT_TYPE_VIRTUAL_SCREEN = 0,       //!< Entire virtual desktop
    SCREENSHOT_TYPE_MAIN_WINDOW = 1,          //!< Main window of process
    SCREENSHOT_TYPE_ALL_PROCESS_WINDOWS = 2   //!< All windows of process
};

/**
 * @class CScreenCapture
 * @brief Captures desktop screenshots and saves as PNG files
 */
class CScreenCapture
{
public:
    CScreenCapture();
    ~CScreenCapture();

    // Disable copy operations
    CScreenCapture(const CScreenCapture&) = delete;
    CScreenCapture& operator=(const CScreenCapture&) = delete;

    /**
     * @brief Take a desktop screenshot
     * @param szSaveToDir Directory to save images
     * @param ssi Output: screenshot information
     * @param type Type of screenshot to take
     * @param dwProcessId Process ID (for window screenshots)
     * @param bGrayscale Create grayscale image
     * @param nIdStartFrom Starting ID for filenames
     * @return TRUE on success
     */
    BOOL TakeDesktopScreenshot(LPCTSTR szSaveToDir,
                               ScreenshotInfo& ssi,
                               SCREENSHOT_TYPE type = SCREENSHOT_TYPE_VIRTUAL_SCREEN,
                               DWORD dwProcessId = 0,
                               BOOL bGrayscale = FALSE,
                               int nIdStartFrom = 0);

private:
    /**
     * @brief Get virtual screen rectangle
     * @param rcScreen Output: screen rectangle
     */
    void GetScreenRect(LPRECT rcScreen);

    /**
     * @brief Find visible windows for a process
     * @param dwProcessId Process ID
     * @param bAllProcessWindows TRUE to find all windows, FALSE for main window only
     * @param paWindows Output: list of windows
     * @return TRUE on success
     */
    BOOL FindWindows(DWORD dwProcessId, 
                     BOOL bAllProcessWindows, 
                     std::vector<WindowInfo>* paWindows);

    /**
     * @brief Capture screen rectangles and save as images
     * @param arcCapture Rectangles to capture
     * @param sSaveDirName Directory to save images
     * @param nIdStartFrom Starting ID for filenames
     * @param bGrayscale Create grayscale images
     * @param monitor_list Output: list of captured monitors
     * @return TRUE on success
     */
    BOOL CaptureScreenRect(std::vector<CRect> arcCapture,
                          CString sSaveDirName,
                          int nIdStartFrom,
                          BOOL bGrayscale,
                          std::vector<MonitorInfo>& monitor_list);

    /**
     * @brief Monitor enumeration callback
     */
    static BOOL CALLBACK EnumMonitorsProc(HMONITOR hMonitor,
                                          HDC hdcMonitor,
                                          LPRECT lprcMonitor,
                                          LPARAM dwData);

    /**
     * @brief Window enumeration callback
     */
    static BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM lParam);

    // === PNG Management Functions ===

    /**
     * @brief Initialize PNG file writer
     * @param nWidth Image width
     * @param nHeight Image height
     * @param bGrayscale Grayscale mode
     * @param sFileName Output filename
     * @return TRUE on success
     */
    BOOL PngInit(int nWidth, int nHeight, BOOL bGrayscale, CString sFileName);

    /**
     * @brief Write a scan line to PNG
     * @param pRow Row data
     * @param nRowLen Row length
     * @param bGrayscale Grayscale mode
     * @return TRUE on success
     */
    BOOL PngWriteRow(LPBYTE pRow, int nRowLen, BOOL bGrayscale);

    /**
     * @brief Finalize and close PNG file
     * @return TRUE on success
     */
    BOOL PngFinalize();

    /**
     * @struct FindWindowData
     * @brief Data for window enumeration
     */
    struct FindWindowData
    {
        DWORD dwProcessId;                    //!< Target process ID
        BOOL bAllProcessWindows;              //!< Find all windows or just main
        std::vector<WindowInfo>* paWindows;   //!< Output window list
    };

    // === Member Variables ===

    CPoint m_ptCursorPos;                 //!< Cursor position
    std::vector<CRect> m_arcCapture;      //!< Rectangles to capture
    CURSORINFO m_CursorInfo;              //!< Cursor information
    int m_nIdStartFrom;                   //!< Starting image ID
    CString m_sSaveDirName;               //!< Save directory
    BOOL m_bGrayscale;                    //!< Grayscale mode flag
    FILE* m_fp;                           //!< PNG file handle
    png_structp m_png_ptr;                //!< libpng write struct
    png_infop m_info_ptr;                 //!< libpng info struct
    std::vector<MonitorInfo> m_monitor_list; //!< Captured monitors
};

#endif // __SCREENCAP_H__
