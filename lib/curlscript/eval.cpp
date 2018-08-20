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
#include "helpers.h"

using namespace std;

namespace curlscript {

    void eval_exprs(vector<expr> exprs, std::ostringstream &output){

        DLOG_S(INFO) << "begin exec expressions";
        init_http();

        for_each(
            begin(exprs),
            end(exprs),
            [&](expr& expr){
                DLOG_S(INFO) << "begin exec expr";

                output << std::accumulate(
                        begin(expr.statements),
                        end(expr.statements),
                        string{""},
                        [&](string out, tuple<vector<item>,string,vector<item>>& statement) {
                            string op = std::get<1>(statement);
                            if(op.empty()){
                                for (auto & item : std::get<0>(statement)) {
                                    out += http_get(item.uri.get_uri());
                                }
                            }else{
                                string op = std::get<1>(statement);
                                DLOG_S(INFO) << "op:" << op;
                                if(op.compare(">") == 0){
                                    for (auto & item : std::get<2>(statement)) {
                                        save_file(item.uri.get_uri(),out);
                                    }
                                }
                                if(op.compare(">>") == 0){
                                    for (auto & item : std::get<2>(statement)) {
                                        append_file(item.uri.get_uri(),out);
                                    }
                                }
                                if(op.compare("|") == 0){
                                    for (auto & item : std::get<2>(statement)) {
                                        out =http_post(item.uri.get_uri(),out);
                                    }
                                }
                            }
                            return out;});
                DLOG_S(INFO) << "end exec expr" ; });

        DLOG_S(INFO) << "#expr:" << exprs.size();
        cleanup_http();
        DLOG_S(INFO) << "end exec expressions";

    }


}
