﻿// Source encoding: utf-8  --  π is (or should be) a lowercase greek pi.
#pragma once
#include <kickstart/core/language/assertion-headers/~assert-reasonable-compiler.hpp>

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

#include <kickstart/core/collection-util/collection-pointers.hpp>       // begin_ptr_of
#include <kickstart/core/failure-handling.hpp>
#include <kickstart/core/stdlib-extensions/c-files/clib-file-types.hpp> // C_file
#include <kickstart/core/stdlib-extensions/strings.hpp>                 // for_each_part_of
#include <kickstart/core/text-encoding/utf8/bom.hpp>                    // bom

#include <stdio.h>

#include <optional>
#include <string>
#include <string_view>

namespace kickstart::system_specific::_definitions {
    using namespace kickstart::failure_handling;
    using namespace kickstart::c_files;
    using namespace kickstart::collection_util;
    using namespace kickstart::language;            // Size, Index
    using namespace kickstart::strings;             // for_each_part
    using   std::optional,
            std::string,
            std::string_view;

    inline auto is_console( const C_file f ) -> Truth;
    inline void raw_output_to_console( const C_file, const string_view& );
    inline auto raw_input_or_eof_from_console( const C_file ) -> optional<string>;

    inline void output_to_console( const C_file f, const string_view& s )
    {
        #ifdef KS_CHECK_CONSOLE_STREAMS_PLEASE
            assert( is_console( f ) );
        #endif

        for_each_part_of( s, utf8::bom, [f]( const string_view& part ) {
            raw_output_to_console( f, part );
        } );
    }

    inline auto input_or_eof_from_console( const C_file f )
        -> optional<string>
    {
        #ifdef KS_CHECK_CONSOLE_STREAMS_PLEASE
            assert( is_console( f ) );
        #endif

        return raw_input_or_eof_from_console( f );
    }

    namespace d = _definitions;
    namespace exports { using
        d::is_console,
        d::output_to_console,
        d::input_or_eof_from_console;
    }  // namespace exports
}  // namespace kickstart::system_specific::_definitions

namespace kickstart::system_specific    { using namespace _definitions::exports; }
