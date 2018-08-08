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

#include <cstdio>
#include <cwchar>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <cassert>
#include <vector>
#include <numeric>
#include <functional>
#include <iostream>

#include <curl/curl.h>
#include "log.h"
#include "expr.h"
#include "http.cpp"

using namespace std;

namespace curlscript {

    void eval_exprs(vector<expr> exprs, std::ostringstream &output){

        init_http();

        for_each(
            begin(exprs),
            end(exprs),
            [&](expr& expr){
                DLOG_S(INFO) << "order:" << expr.order;
                output << "my expirement";
                for (auto & statement : expr.statements) {
                    for (auto & item : std::get<0>(statement)) {
                        DLOG_S(INFO) << "uri:" << item.uri.get_uri();
                    }
                    DLOG_S(INFO) << "op:" << std::get<1>(statement);
                }
                DLOG_S(INFO) << "end" ; });


        DLOG_S(INFO) << "#expr:" << exprs.size();
//        for (auto & expr : exprs) {
//            for (auto & statement : expr.statements) {
//                for (auto & item : std::get<0>(statement)) {
//                    DLOG_S(INFO) << "uri:" << item.uri.get_uri();
//                }
//                DLOG_S(INFO) << "op:" << std::get<1>(statement);
//            }
//            DLOG_S(INFO) << "end" ;
//        }
        cleanup_http();
    }


}
