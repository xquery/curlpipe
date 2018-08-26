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

using namespace std;

CURLM *curlm;
struct curl_slist *headers = NULL;

int init_http(){
    DLOG_S(INFO) << "init http";
    curlm = curl_multi_init();
    return CURLE_OK;
}

int cleanup_http(){
    DLOG_S(INFO) << "cleanup http";
    curl_slist_free_all(headers);
    curl_multi_cleanup(curlm);
    curl_global_cleanup();
    return CURLE_OK;
}

static size_t headerCallback(void *ptr, size_t size, size_t nmemb,
                                   void *stream)
{
    DLOG_S(INFO) << "response header:" << (char *)ptr;
    return size * nmemb;
}


static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *) userp)->append((char *) contents, size * nmemb);
    return size * nmemb;
}
string http_get_info(){


}

int http_set_options(CURL *c){
    DLOG_S(INFO) << "set http opts";
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(c, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(c, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, headerCallback);
#ifndef NDEBUG
    curl_easy_setopt(c, CURLOPT_VERBOSE, 1L);
#endif
    curl_easy_setopt(c, CURLOPT_HEADER, 0L);
    curl_easy_setopt(c, CURLOPT_USERAGENT, "curlscript via curl/7.19.6");
    curl_easy_setopt(c, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, writeCallback);
    return CURLE_OK;
}

string http_get(string url){

    DLOG_S(INFO) << "doing HTTP GET " << url;

    std::ostringstream ss;
    string readBuffer;

    int handle_count;
    CURL *c = NULL;
    c = curl_easy_init();

    if (c) {
        http_set_options(c);
        curl_easy_setopt(c, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(c, CURLOPT_URL, url.c_str());
        curl_easy_setopt(c, CURLOPT_WRITEDATA, &readBuffer);

        curl_multi_add_handle(curlm, c);

        CURLMcode code;
        DLOG_S(INFO) << "perform HTTP GET";
        while (1) {
            code = curl_multi_perform(curlm, &handle_count);
            if (handle_count == 0) {
                ss << readBuffer;
                curl_multi_remove_handle(curlm, c);
                curl_easy_cleanup(c);
                break;
            }
        }
    }
    return ss.str();
}

string http_post(string url, string payload){

    DLOG_S(INFO) << "perform HTTP POST " << url;

    std::ostringstream ss;
    string readBuffer;

    int handle_count;
    CURL *c = NULL;
    c = curl_easy_init();

    if (c) {
        http_set_options(c);
        curl_easy_setopt(c, CURLOPT_POST, 1L);
        curl_easy_setopt(c, CURLOPT_URL, url.c_str());
        curl_easy_setopt(c, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(c, CURLOPT_POSTFIELDS, payload.c_str());

        curl_multi_add_handle(curlm, c);

        CURLMcode code;
        DLOG_S(INFO) << "perform HTTP POST";
        while (1) {
            code = curl_multi_perform(curlm, &handle_count);
            if (handle_count == 0) {
                ss << readBuffer;
                curl_multi_remove_handle(curlm, c);
                curl_easy_cleanup(c);
                break;
            }
        }
    }
    return ss.str();
}