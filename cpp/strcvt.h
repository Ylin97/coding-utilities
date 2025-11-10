#pragma once
#include <Windows.h>

#include <string>
#include <vector>

namespace strcvt
{
/**
 * Convert C-style string to std::wstring.
 *
 * \param str : Input C-style string
 * \param length : Length of the input string, -1 for null-terminated
 * \param cp : Code page for source conversion
 * \return converted std::wstring
 */
inline std::wstring CStr2WString(const char* str, int length = -1,
                                 unsigned int cp = CP_ACP)
{
    if (!str) return std::wstring();

    int wlen = MultiByteToWideChar(cp, 0, str, length, nullptr, 0);
    if (wlen == 0) return std::wstring();
    std::vector<wchar_t> buffer(wlen);
    MultiByteToWideChar(cp, 0, str, length, buffer.data(), wlen);
    return std::wstring(buffer.cbegin(), buffer.cend());
}

/**
 * Convert std::string to std::wstring.
 *
 * \param str : Input std::string
 * \param cp : Code page for source conversion
 * \return Converted std::wstring
 */
inline std::wstring String2WString(const std::string& str,
                                   unsigned int cp = CP_ACP)
{
    if (str.empty()) return std::wstring();
    return CStr2WString(str.c_str(), static_cast<int>(str.size()), cp);
}

/**
 * Convert std::wstring to std::string.
 *
 * \param wstr : Input std::wstring
 * \param cp: Code page for target conversion
 * \return converted std::string
 */
inline std::string WString2String(const std::wstring& wstr,
                                  unsigned int cp = CP_ACP)
{
    if (wstr.empty()) return std::string();

    int len =
        WideCharToMultiByte(cp, 0, wstr.c_str(), static_cast<int>(wstr.size()),
                            nullptr, 0, nullptr, nullptr);
    if (len == 0) return std::string();
    std::vector<char> buffer(len);
    WideCharToMultiByte(cp, 0, wstr.c_str(), static_cast<int>(wstr.size()),
                        buffer.data(), len, nullptr, nullptr);
    return std::string(buffer.cbegin(), buffer.cend());
}

// ============ UTF-8 Specific Conversions ============

/**
 * Convert UTF-8 C-style string to std::wstring.
 *
 * \param str : Input UTF-8 C-style string
 * \param length : Length of the input string, -1 for null-terminated
 * \return converted std::wstring
 */
inline std::wstring U8CStr2WString(const char* str, int length = -1)
{
    if (!str || length == 0) return std::wstring();
    return CStr2WString(str, length, CP_UTF8);
}

inline std::wstring U8String2WString(const std::string& str)
{
    if (str.empty()) return std::wstring();
    return String2WString(str, CP_UTF8);
}

/**
 * Convert std::wstring to UTF-8 std::string.
 *
 * \param wstr : Input std::wstring
 * \return converted UTF-8 std::string
 */
inline std::string WString2U8String(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    return WString2String(wstr, CP_UTF8);
}

/**
 * Convert UTF-8 C-style string to std::string (System default code page).
 *
 * \param utf8_str : Input UTF-8 C-style string
 * \param length : Length of the input string, -1 for null-terminated
 * \return converted std::string
 */
inline std::string U8CStr2String(const char* utf8_str, int length = -1)
{
    if (!utf8_str || length == 0) return std::string();
    // UTF-8 -> UTF-16
    std::wstring wide_str = U8CStr2WString(utf8_str, length);
    // UTF-16 -> ANSI (System default code page, e.g. GBK)
    return WString2String(wide_str, CP_ACP);
}

/**
 * Convert UTF-8 std::string to std::string (System default code page).
 *
 * \param utf8_str: Input UTF-8 std::string
 * \return converted std::string
 */
inline std::string U8String2String(const std::string& utf8_str)
{
    if (utf8_str.empty()) return std::string();
    return U8CStr2String(utf8_str.c_str(), static_cast<int>(utf8_str.size()));
}

/**
 * Convert std::string (System default code page) to UTF-8 std::string.
 *
 * \param str : Input std::string
 * \return converted UTF-8 std::string
 */
inline std::string String2U8String(const std::string& str)
{
    if (str.empty()) return std::string();
    // ANSI (System default code page, e.g. GBK) -> UTF-16
    std::wstring wide_str = String2WString(str, CP_ACP);
    // UTF-16 -> UTF-8
    return WString2U8String(wide_str);
}

// ========== Auxiliary Functions ===========

/**
 * Get the length of UTF-8 string.
 *
 * \param str : UTF-8 string
 * \return length of the string (code points)
 */
size_t U8StrLength(const char* str)
{
    if (!str) return 0;

    size_t count = 0;
    while (*str) {
        unsigned char c = static_cast<unsigned char>(*str);
        if (c < 0x80)
            str += 1;  // 1 byte ASCII
        else if ((c >> 5) == 0x6)
            str += 2;  // 110xxxxx
        else if ((c >> 4) == 0xE)
            str += 3;  // 1110xxxx
        else if ((c >> 3) == 0x1E)
            str += 4;  // 11110xxx
        else
            break;  // invalid UTF-8
        ++count;
    }
    return count;
}

/**
 * Get the length of UTF-8 string.
 *
 * \param str : UTF-8 string
 * \return length of the string (code points)
 */
size_t U8StrLength(const std::string& str)
{
    if (str.empty()) return 0;
    return U8StrLength(str.c_str());
}
}  // namespace strcvt
