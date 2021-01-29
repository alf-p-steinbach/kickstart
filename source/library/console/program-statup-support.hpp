﻿// Source encoding: utf-8  --  π is (or should be) a lowercase greek pi.
#pragma once
#include "../assertion-headers/~assert-reasonable-compiler.hpp"

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

#ifdef _WIN32
#   include "../system-specific/windows/api/consoles.hpp"        // GetACP
#endif

#include "../core/language/type_aliases.hpp"            // Type_, C_str
#include "../core/language/startup-function-types.hpp"  // Simple_startup, Startup_with_args
#include "../core/text-encoding-ascii/string-util.hpp"  // is_all_ascii
#include "../core/failure-handling.hpp"                 // Clean_app_exception
#include "../process/Commandline.hpp"
#include "utf8/io.hpp"                                  // output_error_message

#include <assert.h>         // assert
#include <stdlib.h>         // EXIT_..., strtod
#include <string.h>         // strerror

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace kickstart::console_startup::_definitions {
    namespace process = kickstart::process;
    using namespace kickstart::failure_handling;
    using namespace kickstart::language;
    using namespace std::string_literals;

    using   kickstart::utf8_io::output_error_message;
    using   std::function,
            std::string_view,
            std::vector;

    inline auto with_exceptions_displayed( const function<Simple_startup>& do_things )
        -> int
    {
        try{
            do_things();
            return EXIT_SUCCESS;
        } catch( const Clean_app_exit_exception& x ) {
            output_error_message( ""s + x.what() + "\n" );
        } catch( const exception& x ) {
            output_error_message( "!"s + x.what() + "\n" );
        }
        return EXIT_FAILURE;
    }

    // For non-Windows platforms the parts are unconditionally assumed to be UTF-8 encoded.
    // In Windows the parts are assumed to be UTF-8 encoded if the process' ANSI codepage is UTF-8.
    // Otherwise, in Windows, non-ASCII characters will cause a `std::invalid_argument` exception.
    inline auto with_exceptions_displayed(
        const function<Startup_with_args>&      do_things,
        const int                               n_cmd_parts,
        const Type_<const C_str*>               cmd_parts
        ) -> int
    {
        assert( n_cmd_parts >= 1 );
        assert( cmd_parts != nullptr );
        #ifdef _WIN32
        {
            namespace winapi = kickstart::winapi;
            if( winapi::GetACP() != winapi::cp_utf8 ) {
                for( int i = 0; i < n_cmd_parts; ++i ) {
                    hopefully( ascii::is_all_ascii( cmd_parts[i] ) )
                        or KS_FAIL_( std::invalid_argument, "Not UTF-8 locale and ommmand line contains non-ASCII code(s)." );
                }
            }
        }
        #endif
        return with_exceptions_displayed( [&]() -> void
        {
            const auto args = vector<string_view>( cmd_parts + 1, cmd_parts + n_cmd_parts );
            do_things( cmd_parts[0], args );
        } );
    }

    inline auto with_exceptions_displayed( const function<Startup_with_args>& do_things )
        -> int
    {
        const auto& c = process::the_commandline();
        return with_exceptions_displayed( [&]() -> void
        {
            do_things( c.verb(), c.args() );
        } );
    }


    //----------------------------------------------------------- @exported:
    namespace d = _definitions;
    namespace exported_names { using
        d::Simple_startup,
        d::Startup_with_args,
        d::with_exceptions_displayed;
    }  // namespace exported names
}  // namespace kickstart::console_startup::_definitions

namespace kickstart::console_startup    { using namespace _definitions::exported_names; }
