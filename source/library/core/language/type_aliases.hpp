﻿// Source encoding: utf-8  --  π is (or should be) a lowercase greek pi.
#pragma once
#include "../../assertion-headers/assert-reasonable-compiler.hpp"

// kickstart.core.hpp - minimal convenience functionality for C++ learners.
// The “core” is because the (rather large) iostreams headers are not included.
//
// Requires C++17 or later.

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

#include "bit-level.hpp"        // bits_per_

#include <stddef.h>     // size_t, ptrdiff_t
#include <stdint.h>     // int32_t, int64_t

namespace kickstart::language::_definitions {
    template< class T > using Type_ = T;

    using Size          = ptrdiff_t;
    using Index         = ptrdiff_t;
    using C_str         = const char*;
    using Byte          = unsigned char;


    using Integer       = int64_t;
    using Number        = double;       static_assert( bits_per_<Number> >= 64, "!" );

    using Integer32     = int32_t;
    using Number32      = float;        static_assert( bits_per_<Number32> >= 32, "!" );


    //----------------------------------------------------------- @exported:
    namespace d = _definitions;
    namespace exported_names { using
        d::Type_,
        d::Size, d::Index, d::C_str, d::Byte,
        d::Integer, d::Number, d::Integer32, d::Number32;
    }  // namespace exported names
}  // namespace kickstart::language::_definitions

namespace kickstart::language       { using namespace _definitions::exported_names; }
namespace kickstart::core           { using namespace language; }