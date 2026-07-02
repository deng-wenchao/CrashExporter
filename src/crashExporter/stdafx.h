/*! \file  stdafx.h
*  \brief  Precompiled header - standard system include files
*/

#pragma once

// Windows headers
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#include <atldef.h>
#include <atlbase.h>
#include <atlapp.h>

// CString support
#define _WTL_USE_CSTRING
#include <atlmisc.h>

// STL headers
#include <string>
#include <vector>
#include <map>
#include <memory>

// System headers
#include <Psapi.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <signal.h>
#include <exception>
#include <cstdint>
#include <sys/stat.h>

// DebugHelp
#include "dbghelp.h"

// Disable secure CRT warnings for legacy compatibility
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
