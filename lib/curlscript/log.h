/******************************************************************************
 * curlscript - https://github.com/xquery/curlscript
 ******************************************************************************
 * Copyright (c) 2017-2018 James Fuller <jim.fuller@webcomposite.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
******************************************************************************/

#ifndef CURLSCRIPT_LOG_H
#define CURLSCRIPT_LOG_H

#ifndef LOGURU_IMPLEMENTATION
#define LOGURU_IMPLEMENTATION   1

#define CURLSCRIPT_VERSION_MAJOR 0
#define CURLSCRIPT_VERSION_MINOR 1
#define CURLSCRIPT_VERSION_PATCH 0

#define LOGURU_WITH_STREAMS     1
#define LOGURU_FILENAME_WIDTH   16
#define LOGURU_REDEFINE_ASSERT  1

#ifndef NDEBUG
    #define LOGURU_STACKTRACES      1
    #define LOGURU_RTTI             1
#endif


#include <loguru.hpp>
#endif

#endif //CURLSCRIPT_LOG_H
