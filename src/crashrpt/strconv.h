/*! \file  strconv.h
*  \brief  String conversion utilities for Unicode/ANSI/UTF-8
*/

#ifndef _STRCONV_H
#define _STRCONV_H

#include <windows.h>
#include <vector>
#include <string>

/**
 * @class strconv_t
 * @brief String conversion helper class with automatic memory management
 */
class strconv_t
{
public:
    strconv_t() = default;
    
    ~strconv_t()
    {
        for (auto* buffer : m_ConvertedStrings)
        {
            delete[] buffer;
        }
    }

    // Disable copy operations
    strconv_t(const strconv_t&) = delete;
    strconv_t& operator=(const strconv_t&) = delete;

    /** Convert ANSI to WideChar (Unicode) */
    LPCWSTR a2w(LPCSTR lpsz)
    {
        if (lpsz == nullptr) return nullptr;

        int count = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsz, -1, nullptr, 0);
        if (count == 0) return nullptr;

        auto* pBuffer = new wchar_t[count];
        int result = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsz, -1, pBuffer, count);
        if (result == 0)
        {
            delete[] pBuffer;
            return nullptr;
        }

        m_ConvertedStrings.push_back(pBuffer);
        return pBuffer;
    }

    /** Convert WideChar to ANSI */
    LPCSTR w2a(LPCWSTR lpsz)
    {
        if (lpsz == nullptr) return nullptr;

        int count = WideCharToMultiByte(CP_ACP, 0, lpsz, -1, nullptr, 0, nullptr, nullptr);
        if (count == 0) return nullptr;

        auto* pBuffer = new char[count];
        int result = WideCharToMultiByte(CP_ACP, 0, lpsz, -1, pBuffer, count, nullptr, nullptr);
        if (result == 0)
        {
            delete[] pBuffer;
            return nullptr;
        }

        m_ConvertedStrings.push_back(pBuffer);
        return pBuffer;
    }

    /** Convert WideChar to UTF-8 */
    LPCSTR w2utf8(LPCWSTR lpsz)
    {
        if (lpsz == nullptr) return nullptr;

        int count = WideCharToMultiByte(CP_UTF8, 0, lpsz, -1, nullptr, 0, nullptr, nullptr);
        if (count == 0) return nullptr;

        auto* pBuffer = new char[count];
        int result = WideCharToMultiByte(CP_UTF8, 0, lpsz, -1, pBuffer, count, nullptr, nullptr);
        if (result == 0)
        {
            delete[] pBuffer;
            return nullptr;
        }

        m_ConvertedStrings.push_back(pBuffer);
        return pBuffer;
    }

    /** Convert ANSI to UTF-8 */
    LPCSTR a2utf8(LPCSTR lpsz)
    {
        if (lpsz == nullptr) return nullptr;

        // First convert ANSI to WideChar
        int count = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsz, -1, nullptr, 0);
        if (count == 0) return nullptr;

        auto* pWideBuffer = new wchar_t[count];
        int result = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpsz, -1, pWideBuffer, count);
        if (result == 0)
        {
            delete[] pWideBuffer;
            return nullptr;
        }

        // Then convert WideChar to UTF-8
        LPCSTR pszResult = w2utf8(pWideBuffer);
        delete[] pWideBuffer;
        return pszResult;
    }

    /** Convert UTF-8 to WideChar */
    LPCWSTR utf82w(LPCSTR lpsz)
    {
        if (lpsz == nullptr) return nullptr;

        int count = MultiByteToWideChar(CP_UTF8, 0, lpsz, -1, nullptr, 0);
        if (count == 0) return nullptr;

        auto* pBuffer = new wchar_t[count];
        int result = MultiByteToWideChar(CP_UTF8, 0, lpsz, -1, pBuffer, count);
        if (result == 0)
        {
            delete[] pBuffer;
            return nullptr;
        }

        m_ConvertedStrings.push_back(pBuffer);
        return pBuffer;
    }

    /** Convert UTF-8 to WideChar with explicit length */
    LPCWSTR utf82w(LPCSTR pStr, UINT cch)
    {
        if (pStr == nullptr) return nullptr;

        int count = MultiByteToWideChar(CP_UTF8, 0, pStr, static_cast<int>(cch), nullptr, 0);
        if (count == 0) return nullptr;

        auto* pBuffer = new wchar_t[count + 1];
        int result = MultiByteToWideChar(CP_UTF8, 0, pStr, static_cast<int>(cch), pBuffer, count);
        if (result == 0)
        {
            delete[] pBuffer;
            return nullptr;
        }

        pBuffer[count] = 0; // Zero-terminate
        m_ConvertedStrings.push_back(pBuffer);
        return pBuffer;
    }

    /** Convert UTF-8 to ANSI */
    LPCSTR utf82a(LPCSTR lpsz)
    {
        return w2a(utf82w(lpsz));
    }

    /** Convert UTF-8 to TCHAR (Unicode or ANSI depending on build) */
    LPCTSTR utf82t(LPCSTR lpsz)
    {
#ifdef UNICODE
        return utf82w(lpsz);
#else
        return utf82a(lpsz);
#endif
    }

    /** Convert TCHAR to ANSI */
    LPCSTR t2a(LPCTSTR lpsz)
    {
#ifdef UNICODE
        return w2a(lpsz);
#else
        return lpsz;
#endif
    }

    /** Convert TCHAR to WideChar */
    LPCWSTR t2w(LPCTSTR lpsz)
    {
#ifdef UNICODE
        return lpsz;
#else
        return a2w(lpsz);
#endif
    }

    /** Convert ANSI to TCHAR */
    LPCTSTR a2t(LPCSTR lpsz)
    {
#ifdef UNICODE
        return a2w(lpsz);
#else
        return lpsz;
#endif
    }

    /** Convert WideChar to TCHAR */
    LPCTSTR w2t(LPCWSTR lpsz)
    {
#ifdef UNICODE
        return lpsz;
#else
        return w2a(lpsz);
#endif
    }

    /** Convert TCHAR to UTF-8 */
    LPCSTR t2utf8(LPCTSTR lpsz)
    {
#ifdef UNICODE
        return w2utf8(lpsz);
#else
        return a2utf8(lpsz);
#endif
    }

private:
    std::vector<void*> m_ConvertedStrings;
};

#endif // _STRCONV_H
