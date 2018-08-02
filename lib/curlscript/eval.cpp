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

#include <curl/curl.h>
#include "log.h"
#include "expr.h"

using namespace std;

namespace curlscript {

    CURLM *curlm;

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        ((std::string *) userp)->append((char *) contents, size * nmemb);
        return size * nmemb;
    }

    string http_get(string url){

        curlm = curl_multi_init();

        std::ostringstream ss;
        string readBuffer;

        int handle_count;
        CURL *curl1 = NULL;
        curl1 = curl_easy_init();

        if (curl1) {
//          curl_easy_setopt(curl1, CURLOPT_HTTPHEADER, headers);
//          curl_easy_setopt(curl1, CURLOPT_USERNAME, config.user.c_str());
//          curl_easy_setopt(curl1, CURLOPT_PASSWORD, config.pass.c_str());
//            curl_easy_setopt(curl1, CURLOPT_HTTPAUTH, CURLAUTH_ANY);

            curl_easy_setopt(curl1, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(curl1, CURLOPT_USERAGENT, "curlscript via curl/7.19.6");
            curl_easy_setopt(curl1, CURLOPT_FAILONERROR, 1);
            curl_easy_setopt(curl1, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl1, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl1, CURLOPT_HTTPGET, 1L);
            curl_easy_setopt(curl1, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl1, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl1, CURLOPT_WRITEDATA, &readBuffer);

            curl_multi_add_handle(curlm, curl1);

            CURLMcode code;
            while (1) {
                code = curl_multi_perform(curlm, &handle_count);
                if (handle_count == 0) {
                    ss << readBuffer;
                    break;
                }
            }
        }
        return ss.str();
    }

    void eval_exprs(vector<expr> exprs){

        for (auto & expr : exprs) {

            for (auto & item : expr.items1) {
                DLOG_S(INFO) << item.get_uri();
                if(expr.op.empty()){
//                  DLOG_S(INFO) << http_get(item.get_uri());
                }
            }

            DLOG_S(INFO) << expr.op;

            for (auto & item : expr.items2) {
                DLOG_S(INFO) << item.get_uri();
                if(expr.op.empty()){
//                  DLOG_S(INFO) << http_get(item.get_uri());
                }
            }

        }
        DLOG_S(INFO) << exprs.size();
        curl_global_cleanup();

    }



}
