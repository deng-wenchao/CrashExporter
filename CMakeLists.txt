cmake_minimum_required(VERSION 3.15)
project(CrashExporter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_SHARED_LIBS "Build CrashRpt as shared library" ON)
option(BUILD_TESTS "Build tests" OFF)

# Platform check
if(NOT WIN32)
    message(WARNING "This project is designed for Windows. Building on non-Windows platform may fail.")
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(ZLIB QUIET)
if(NOT ZLIB_FOUND)
    # Use bundled zlib
    set(ZLIB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/zlib)
    set(ZLIB_LIBRARIES "")
    add_library(zlib_static STATIC
        thirdparty/zlib/adler32.c
        thirdparty/zlib/compress.c
        thirdparty/zlib/crc32.c
        thirdparty/zlib/deflate.c
        thirdparty/zlib/gzclose.c
        thirdparty/zlib/gzlib.c
        thirdparty/zlib/gzread.c
        thirdparty/zlib/gzwrite.c
        thirdparty/zlib/inflate.c
        thirdparty/zlib/infback.c
        thirdparty/zlib/inftrees.c
        thirdparty/zlib/inffast.c
        thirdparty/zlib/trees.c
        thirdparty/zlib/uncompr.c
        thirdparty/zlib/zutil.c
    )
    target_include_directories(zlib_static PUBLIC ${ZLIB_INCLUDE_DIRS})
    set(ZLIB_LIBRARIES zlib_static)
endif()

# Build libpng from source
set(LIBPNG_SOURCES
    thirdparty/libpng/png.c
    thirdparty/libpng/pngerror.c
    thirdparty/libpng/pnggccrd.c
    thirdparty/libpng/pngget.c
    thirdparty/libpng/pngmem.c
    thirdparty/libpng/pngpread.c
    thirdparty/libpng/pngread.c
    thirdparty/libpng/pngrio.c
    thirdparty/libpng/pngrtran.c
    thirdparty/libpng/pngrutil.c
    thirdparty/libpng/pngset.c
    thirdparty/libpng/pngtrans.c
    thirdparty/libpng/pngvcrd.c
    thirdparty/libpng/pngwio.c
    thirdparty/libpng/pngwrite.c
    thirdparty/libpng/pngwtran.c
    thirdparty/libpng/pngwutil.c
)

add_library(libpng_static STATIC ${LIBPNG_SOURCES})
target_include_directories(libpng_static PUBLIC 
    ${CMAKE_SOURCE_DIR}/thirdparty/libpng
    ${ZLIB_INCLUDE_DIRS}
)
target_link_libraries(libpng_static PRIVATE ${ZLIB_LIBRARIES})

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/crashrpt
    ${CMAKE_SOURCE_DIR}/src/crashExporter
    ${CMAKE_SOURCE_DIR}/thirdparty/wtl
    ${CMAKE_SOURCE_DIR}/thirdparty/dbghelp/include
    ${CMAKE_SOURCE_DIR}/thirdparty/zlib
    ${CMAKE_SOURCE_DIR}/thirdparty/libpng
    ${CMAKE_SOURCE_DIR}/thirdparty/tinyxml
    ${CMAKE_SOURCE_DIR}/thirdparty/minizip
    ${CMAKE_SOURCE_DIR}/thirdparty/jpeg
    ${CMAKE_SOURCE_DIR}/thirdparty/libogg/include
    ${CMAKE_SOURCE_DIR}/thirdparty/libtheora/include
    ${ZLIB_INCLUDE_DIRS}
)

# Precompiled header support
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

# ============================================================================
# CrashRpt Library
# ============================================================================

set(CRASHRPT_SOURCES
    src/crashrpt/StdAfx.cpp
    src/crashrpt/CrashRpt.cpp
    src/crashrpt/CrashHandler.cpp
    src/crashrpt/Utility.cpp
    src/crashrpt/SharedMem.cpp
    src/crashrpt/StackWalker.cpp
)

set(CRASHRPT_HEADERS
    src/crashrpt/StdAfx.h
    src/crashrpt/CrashRpt.h
    src/crashrpt/CrashHandler.h
    src/crashrpt/Utility.h
    src/crashrpt/SharedMem.h
    src/crashrpt/StackWalker.h
    src/crashrpt/CritSec.h
    src/crashrpt/strconv.h
    include/CrashRpt.h
    include/dbghelp_VC6.h
    include/DbgHelp.h
)

set(CRASHRPT_RC
    src/crashrpt/dllversion.rc
)

if(BUILD_SHARED_LIBS)
    add_library(CrashRpt SHARED ${CRASHRPT_SOURCES} ${CRASHRPT_HEADERS} ${CRASHRPT_RC})
    target_compile_definitions(CrashRpt PRIVATE CRASHRPT_EXPORTS)
else()
    add_library(CrashRpt STATIC ${CRASHRPT_SOURCES} ${CRASHRPT_HEADERS})
    target_compile_definitions(CrashRpt PUBLIC CRASHRPT_LIB)
endif()

target_compile_definitions(CrashRpt PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _WINSOCK_DEPRECATED_NO_WARNINGS
    UNICODE
    _UNICODE
)

if(MSVC)
    target_compile_options(CrashRpt PRIVATE /W4)
    target_link_libraries(CrashRpt PRIVATE
        dbghelp.lib
        psapi.lib
        shell32.lib
        ole32.lib
        comctl32.lib
        winmm.lib
        ${ZLIB_LIBRARIES}
    )
else()
    target_link_libraries(CrashRpt PRIVATE
        ${ZLIB_LIBRARIES}
    )
endif()

target_precompile_headers(CrashRpt PRIVATE
    <windows.h>
    <atlbase.h>
    <atlapp.h>
    <atlmisc.h>
)

# ============================================================================
# CrashExporter Executable
# ============================================================================

set(CRASHEXPORTER_SOURCES
    src/crashExporter/stdafx.cpp
    src/crashExporter/CrashExporter.cpp
    src/crashExporter/ErrorReportExporter.cpp
    src/crashExporter/CrashInfoReader.cpp
    src/crashExporter/ScreenCap.cpp
    src/crashExporter/AssyncNotification.cpp
    src/crashrpt/SharedMem.cpp
    src/crashrpt/Utility.cpp
)

set(CRASHEXPORTER_HEADERS
    src/crashExporter/stdafx.h
    src/crashExporter/CrashExporter.h
    src/crashExporter/ErrorReportExporter.h
    src/crashExporter/CrashInfoReader.h
    src/crashExporter/ScreenCap.h
    src/crashExporter/AssyncNotification.h
    src/crashrpt/SharedMem.h
    src/crashrpt/Utility.h
)

set(CRASHEXPORTER_RC
    src/crashExporter/version.rc
)

add_executable(CrashExporter WIN32 ${CRASHEXPORTER_SOURCES} ${CRASHEXPORTER_HEADERS} ${CRASHEXPORTER_RC})

target_compile_definitions(CrashExporter PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _WINSOCK_DEPRECATED_NO_WARNINGS
    UNICODE
    _UNICODE
)

if(MSVC)
    target_compile_options(CrashExporter PRIVATE /W4)
    target_link_libraries(CrashExporter PRIVATE
        CrashRpt
        libpng_static
        dbghelp.lib
        psapi.lib
        shell32.lib
        ole32.lib
        comctl32.lib
        winmm.lib
        gdiplus.lib
        ${ZLIB_LIBRARIES}
    )
else()
    target_link_libraries(CrashExporter PRIVATE
        CrashRpt
        libpng_static
        ${ZLIB_LIBRARIES}
    )
endif()

target_precompile_headers(CrashExporter PRIVATE
    <windows.h>
    <atlbase.h>
    <atlapp.h>
    <atlmisc.h>
)

# ============================================================================
# Tests (Optional)
# ============================================================================

if(BUILD_TESTS)
    enable_testing()
    
    set(TEST_SOURCES
        Tests/CrashExporterTests.cpp
        Tests/CrashExporterTestsSuite.cpp
    )
    
    add_executable(CrashExporterTests ${TEST_SOURCES})
    target_link_libraries(CrashExporterTests PRIVATE CrashRpt)
    
    add_test(NAME CrashExporterTests COMMAND CrashExporterTests)
endif()

# ============================================================================
# Installation
# ============================================================================

install(TARGETS CrashRpt CrashExporter
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "CrashExporter Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build shared libs: ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
