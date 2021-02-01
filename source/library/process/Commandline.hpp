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

#include "../core/failure-handling.hpp"
#include "../core/language/collection-util.hpp"             // Array_span_
#include "../system-specific/get_commandline_data.hpp"      // get_command_line_data

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace kickstart::process::_definitions {
    namespace k = kickstart;
    using   k::system_specific::Commandline_data, k::system_specific::get_commandline_data;
    using   std::unique_ptr, std::make_unique,
            std::string,
            std::string_view,
            std::move,                  // From <utility>
            std::vector;

    using namespace kickstart::failure_handling;    // hopefully, fail
    using namespace kickstart::language;            // Array_span_, Type_, end_ptr_of

    class Commandline
    {
        using Self = Commandline;
        Commandline( const Self& ) = delete;
        auto operator=( const Self& ) -> Self& = delete;

        Commandline_data        m_data;

        Commandline():
            m_data( get_commandline_data() )
        {}

        Commandline( const int n_parts, const Type_<const char**> parts ):
            m_data{ parts[0], vector<string>( parts + 1, parts + n_parts ) }
        {}

        static inline auto new_or_existing_singleton(
            const int                   n_parts,
            const Type_<const char**>   parts
            ) -> const Commandline&
        {
            static unique_ptr<Commandline> p_the_instance = nullptr;

            // TODO: correctness: add mutex; speed: fast path optimization for already existing.
            hopefully( n_parts == 0 or not p_the_instance )
                or KS_FAIL( "Creation arguments specified for access of existing singleton." );
            if( not p_the_instance ) {
                p_the_instance = unique_ptr<Commandline>(
                    n_parts == 0? new Commandline() : new Commandline( n_parts, parts )
                    );
            }
            return *p_the_instance;
        }

    public:
        auto fulltext() const   -> const string&    { return m_data.fulltext; }
        auto verb() const       -> const string&    { return m_data.parts[0]; }

        auto args() const
            -> Array_span_<const string>
        { return {begin_ptr_of( m_data.parts ) + 1, end_ptr_of( m_data.parts )}; }

        operator const string& () const { return fulltext(); }

        static inline auto create_singleton(
            const int                   n_parts,
            const Type_<const char**>   parts
            ) -> const Commandline&
        {
            assert( n_parts > 0 );
            return new_or_existing_singleton( n_parts, parts );
        }

        static inline auto singleton()
            -> const Commandline&
        { return new_or_existing_singleton( 0, nullptr ); }
    };

    inline auto the_commandline() -> const Commandline& { return Commandline::singleton(); }


    //----------------------------------------------------------- @exported:
    namespace d = _definitions;
    namespace exported_names { using
        d::Commandline,
        d::the_commandline;
    }  // namespace exported_names
}  // namespace kickstart::process::_definitions

namespace kickstart::process        { using namespace _definitions; }
