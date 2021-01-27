﻿// Source encoding: utf-8  --  π is (or should be) a lowercase greek pi.
#pragma once
#include "../../assertion-headers/$-assert-reasonable-compiler.hpp"

// Copyright (c) 2020 Alf P. Steinbach. MIT license, with license text:
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "api/text-encoding.hpp"
#include "../../core/failure-handling.hpp"
#include "../../core/language/collection-util.hpp"          // int_size

#include <assert.h>         // assert
#include <limits.h>         // INT_MAX

#include <string>
#include <string_view>

namespace kickstart::system_specific::_definitions {
    namespace winapi = kickstart::winapi;
    using namespace kickstart::failure_handling;        // hopefully, fail
    using namespace kickstart::language;                // int_size
    using   std::string,
            std::wstring_view;

    inline auto to_utf8( const wstring_view& ws )
        -> string
    {
        if( ws.empty() ) {
            return "";
        }
        assert( ws.size() <= size_t( INT_MAX ) );

        const winapi::DWORD flags = 0;
        const int buffer_size = winapi::WideCharToMultiByte(
            winapi::cp_utf8, flags, ws.data(), int_size( ws ), nullptr, 0, nullptr, nullptr
            );
        hopefully( buffer_size != 0 )
            or KS_FAIL( "WideCharToMultiByte failed" );

        auto result = string( buffer_size, '\0' );
        const int string_size = winapi::WideCharToMultiByte(
            winapi::cp_utf8, flags, ws.data(), int_size( ws ), result.data(), buffer_size, nullptr, nullptr
            );
        result.resize( string_size );       // Not clear if this is necessary.
        return result;
    }

   //----------------------------------------------------------- @exported:
    namespace d = _definitions;
    namespace exported_names { using
        d::to_utf8;
    }  // namespace exported names
}  // namespace kickstart::system_specific::_definitions

namespace kickstart::system_specific    { using namespace _definitions::exported_names; }