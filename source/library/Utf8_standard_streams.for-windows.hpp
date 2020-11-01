﻿// Source encoding: utf-8  --  π is (or should be) a lowercase greek pi.
#pragma once
#ifndef _WIN64
#   error "This header is for 64-bit Windows systems only."
#endif
static_assert( sizeof( void* ) == 8 );  // 64-bit system

// Copyright (c) 2020 Alf P. Steinbach. MIT license, with license text:
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Utf8_standard_streams_interface.hpp"

// Part of workaround for sabotage-like Visual C++ 2019 behavior for “extern "C"” funcs:
#if defined( KS_USE_WINDOWS_H ) || defined( BOOST_USE_WINDOWS_H )
#   include <windows.h>
#endif

#include <assert.h>     // assert
#include <stdint.h>     // uint32_t
#include <io.h>         // _get_osfhandle
#include <limits.h>     // INT_MAX

#include <string>       // std::wstring

namespace ks {
    using std::wstring;

    namespace winapi {
        // Visual C++ 2019 (16.3.3) and later may issue errors on the Windows API function
        // declarations here when <windows.h> is also included, as explained in
        //
        // “Including Windows.h and Boost.Interprocess headers leads to C2116 and C2733”
        // https://developercommunity.visualstudio.com/content/problem/756694/including-windowsh-and-boostinterprocess-headers-l.html
        //
        // A fix is to use the per October 2019 undocumented option “/Zc:externC-”.
        //
        // A more fragile fix is to include <windows.h> BEFORE any Kickstart header, or
        // to define KS_USE_WINDOWS_H or BOOST_USE_WINDOWS_H or both in the build.

        #ifdef MessageBox       // <windows.h> has been included
            using ::BOOL, ::DWORD, ::HANDLE, ::UINT;
            using ::GetConsoleMode, ::SetConsoleMode, ::MultiByteToWideChar, ::WriteConsoleW;

            const auto enable_virtual_terminal_processing   = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            const auto enable_extended_flags                = ENABLE_EXTENDED_FLAGS;
            const auto cp_utf8                              = CP_UTF8;
        #else
            using BOOL      = int;
            using DWORD     = uint32_t;
            using HANDLE    = void*;
            using UINT      = unsigned;

            const DWORD enable_virtual_terminal_processing  = 0x0004;
            const DWORD enable_extended_flags               = 0x0080;
            //const DWORD enable_virtual_terminal_input       = 0x0200;

            const UINT cp_utf8      = 65001;

            extern "C" auto __stdcall GetConsoleMode( HANDLE  hConsoleHandle, DWORD* lpMode ) -> BOOL;
            extern "C" auto __stdcall SetConsoleMode( HANDLE  hConsoleHandle, DWORD dwMode ) -> BOOL;

            extern "C" auto __stdcall MultiByteToWideChar(
                UINT                            CodePage,
                DWORD                           dwFlags,
                const char*                     lpMultiByteStr,
                int                             cbMultiByte,
                wchar_t*                        lpWideCharStr,
                int                             cchWideChar
                ) -> int;

            extern "C" auto __stdcall WriteConsoleW(
                HANDLE          hConsoleOutput,
                const void*     lpBuffer,
                DWORD           nNumberOfCharsToWrite,
                DWORD*          lpNumberOfCharsWritten,
                void*           lpReserved
                ) -> BOOL;
        #endif
    }  // namespace winapi

    class Utf8_standard_streams
    {
        template< class T > using Type_ = T;

        FILE*           m_console_streams[3]    = {};
        winapi::HANDLE  m_console_output_handle = {};
        winapi::DWORD   m_original_mode         = winapi::DWORD( -1 );

        ~Utf8_standard_streams()
        {
            if( m_original_mode != winapi::DWORD( -1 ) ) {
                winapi::SetConsoleMode( m_console_output_handle, m_original_mode );
            }
        }

        Utf8_standard_streams()
        {
            static const Type_<FILE*> c_streams[] = {stdin, stdout, stderr};

            for( const int stream_id: {0, 1, 2} ) {
                const auto handle = reinterpret_cast<winapi::HANDLE>( _get_osfhandle( stream_id ) );
                winapi::DWORD dummy;
                const bool is_console = !!winapi::GetConsoleMode( handle, &dummy );
                if( is_console ) {
                    const Type_<FILE*> c_stream = c_streams[stream_id];
                    m_console_streams[stream_id] = c_stream;
                    if( stream_id > 0 ) {
                        m_console_output_handle = handle;
                    }
                }
            }

            if( m_console_output_handle ) {
                if( winapi::GetConsoleMode( m_console_output_handle, &m_original_mode ) ) {
                    const auto support_escapes = 0
                        | (m_original_mode | winapi::enable_virtual_terminal_processing)
                        & ~winapi::enable_extended_flags;
                    winapi::SetConsoleMode( m_console_output_handle, support_escapes );
                }
            }
        }

        struct Console
        {
            static auto read_byte( const Type_<FILE*> f )
                -> int
            {
                const auto& streams = Utf8_standard_streams::singleton();
                (void) streams;
                return ::fgetc( f );
            }

            static auto write( const Type_<const void*> buffer, const Size n, FILE* )
                -> Size
            {
                if( n <= 0 ) {
                    return 0;
                }
                assert( n <= Size( INT_MAX ) );

                auto ws = wstring( n, L'\0' );
                const int flags = 0;
                const int ws_len = winapi::MultiByteToWideChar(
                    winapi::cp_utf8, flags, static_cast<const char*>( buffer ), int( n ), ws.data(), int( n )
                    );
                assert( ws_len > 0 );       // More precisely, the number of UTF-8 encoded code points.
                const auto handle = Utf8_standard_streams::singleton().m_console_output_handle;
                winapi::DWORD n_chars_written;
                winapi::WriteConsoleW( handle, ws.data(), ws_len, &n_chars_written, nullptr );
                return (int( n_chars_written ) < ws_len? 0 : n);    // Reporting actual count is costly.
            }
        };

        struct C_streams
        {
            static auto read_byte( const Type_<FILE*> f )
                -> int
            { return ::fgetc( f ); }

            static auto write( const Type_<const void*> buffer, const Size buffer_size, const Type_<FILE*> f)
                -> Size
            { return ::fwrite( buffer, 1, buffer_size, f ); }
        };

    public:
        using Func = Utf8_standard_streams_interface::Func;

        auto read_byte_func_for( const Type_<FILE*> f ) const
            -> Func::Read_byte&
        {
            for( const auto stream: m_console_streams ) {
                if( stream == f ) {
                    return *Console::read_byte;
                }
            }
            return *C_streams::read_byte;
        }

        auto write_func_for( const Type_<FILE*> f ) const
            -> Func::Write&
        {
            for( const auto stream: m_console_streams ) {
                if( stream == f ) {
                    return *Console::write;
                }
            }
            return *C_streams::write;
        }

        static auto singleton()
            -> Utf8_standard_streams&
        {
            static Utf8_standard_streams the_instance;
            return the_instance;
        }

        static void init() { singleton(); }
    };

    static_assert(
        Utf8_standard_streams_interface::is_implemented_by_<Utf8_standard_streams>()
        );

}  // namespace ks
