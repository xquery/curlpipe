/******************************************************************************
 * curlpipe - https://github.com/xquery/curlpipe
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

#define SUPPORT_NETRC 1

#ifdef _WIN32
#define WAITMS(x) Sleep(x)
#else
/* Portable sleep for platforms other than Windows. */
#define WAITMS(x)                               \
  struct timeval wait = { 0, (x) * 1000 };      \
  (void)select(0, NULL, NULL, NULL, &wait);
#endif

using namespace std;

CURLM *curlm;
struct CURLMsg *m;
int http_status_code,rc;
struct curl_slist *headers = NULL;

int init_http(){
    DLOG_S(INFO) << "init http";
    curlm = curl_multi_init();
    headers = curl_slist_append(headers, "Accept:text/html,application/xhtml+xml,application/xml,application/json;q=0.9,*/*;q=0.8");
    return CURLE_OK;
}

int cleanup_http(){
    DLOG_S(INFO) << "cleanup http";
    curl_multi_cleanup(curlm);
    curl_global_cleanup();
    return CURLE_OK;
}

static size_t headerCallback(void *ptr, size_t size, size_t nmemb, void *stream){
    DLOG_S(INFO) << "response header:" << (char *)ptr;
    return size * nmemb;
}

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *) userp)->append((char *) contents, size * nmemb);
    return size * nmemb;
}

int http_set_options(CURL *c, CURLU *urlp){
    DLOG_S(INFO) << "set http opts";
    char *url;
    rc = curl_url_get(urlp, CURLUPART_URL, &url, 0);
    DLOG_S(INFO) << "set url to: " << url;
    curl_easy_setopt(c, CURLOPT_CURLU, urlp);
    curl_easy_setopt(c, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
#ifdef SUPPORT_NETRC
    curl_easy_setopt(c, CURLOPT_NETRC,CURL_NETRC_OPTIONAL);
#endif
    curl_easy_setopt(c, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, headerCallback);
#ifndef NDEBUG
    curl_easy_setopt(c, CURLOPT_VERBOSE, 1L);
#endif
    curl_easy_setopt(c, CURLOPT_HEADER, 0L);
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(c, CURLOPT_USERAGENT, "curlpipe via curl");
    curl_easy_setopt(c, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, writeCallback);
    return CURLE_OK;
}

int exec(){
    int still_running = -1;
    curl_multi_perform(curlm, &still_running);
    CURLMcode mc;
    int numfds;
    while (still_running) {
        mc = curl_multi_wait(curlm, NULL, 0, 1000, &numfds);
        if(mc != CURLM_OK) {
            LOG_S(ERROR) << "curl_multi_wait() failed, code " << mc;
            break;
        }
        curl_multi_perform(curlm, &still_running);
    }

    CURL *eh = nullptr;
    CURLMsg *msg = nullptr;
    CURLcode return_code;
    int msgs_left = 0;
    while ((msg = curl_multi_info_read(curlm, &msgs_left))) {
        if (msg->msg == CURLMSG_DONE) {
            eh = msg->easy_handle;
            http_status_code=0;
            const char *url;

            curl_easy_getinfo(eh, CURLINFO_RESPONSE_CODE, &http_status_code);
            curl_easy_getinfo(eh, CURLINFO_EFFECTIVE_URL, &url);

            return_code = msg->data.result;
            if(return_code!=CURLE_OK) {
                LOG_S(ERROR) << "url: " << url;
                LOG_S(ERROR) << "http status code: " << http_status_code;
                LOG_S(ERROR) << "error code: " << curl_easy_strerror(msg->data.result);
                continue;
            }

            if(http_status_code==200) {
                DLOG_S(INFO) << "HTTP Status Code: " << http_status_code;
            } else {
                LOG_S(ERROR) << "HTTP Status Code: " << http_status_code;
            }

            curl_multi_remove_handle(curlm, eh);
            curl_easy_cleanup(eh);
        }else{
            LOG_S(ERROR) << "error: after curl_multi_info_read(), CURLMsg=" << msg->msg;}
    }
    return 0;
}

string http_get(CURLU *urlp){

    std::ostringstream ss;
    string readBuffer;

    int handle_count;
    CURL *c = NULL;

    c = curl_easy_init();

    if (c) {
        http_set_options(c, urlp);
        headers = NULL;
        curl_easy_setopt(c, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(c, CURLOPT_WRITEDATA, &readBuffer);

        curl_multi_add_handle(curlm, c);

        DLOG_S(INFO) << "perform HTTP GET";
        int result = exec();
        if(result==0){
            ss << readBuffer;
        }else{
            //error
        }
    }

    return ss.str();
}

string http_post(CURLU *urlp, string payload){

    std::ostringstream ss;
    string readBuffer;

    CURL *c = NULL;
    c = curl_easy_init();

    if (c) {
        http_set_options(c,urlp);

        //ungodly hack (for now)
        headers = NULL;
        if(payload.find("{") == 0){
            headers = curl_slist_append(headers, "Content-Type:application/json");
        }else if(payload.find("<") == 0){
            headers = curl_slist_append(headers, "Content-Type:application/xml");
        }else if(payload.find("&") != std::string::npos){
            headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
        }else {
            headers = curl_slist_append(headers, "Content-Type:text/plain");
        }
        curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(c, CURLOPT_POST, 1L);

        curl_easy_setopt(c, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(c, CURLOPT_POSTFIELDS, payload.c_str());

        curl_multi_add_handle(curlm, c);

        DLOG_S(INFO) << "perform HTTP POST";
        int result = exec();
        if(result==0){
            ss << readBuffer;
        }else{
            //error
        }
    }
    return ss.str();
}

string http_delete(CURLU *urlp){

    std::ostringstream ss;
    string readBuffer;

    CURL *c = NULL;
    c = curl_easy_init();

    if (c) {
        http_set_options(c,urlp);
        curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(c, CURLOPT_WRITEDATA, &readBuffer);

        curl_multi_add_handle(curlm, c);

        DLOG_S(INFO) << "perform HTTP DELETE";
        int result = exec();
        if(result==0){
            ss << readBuffer;
        }else{
            //error
        }
    }
    return ss.str();
}

string http_put(CURLU *urlp, string payload){

    std::ostringstream ss;
    string readBuffer;

    CURL *c = NULL;
    c = curl_easy_init();


    if (c) {
        http_set_options(c,urlp);

        //ungodly hack (for now)
        headers = NULL;
        if(payload.find("{") == 0){
            headers = curl_slist_append(headers, "Content-Type:application/json");
        }else if(payload.find("<") == 0){
            headers = curl_slist_append(headers, "Content-Type:application/xml");

        }else if(payload.find("&") != std::string::npos){
            headers = curl_slist_append(headers, "Content-Type:application/x-www-form-urlencoded");
        }else {
            headers = curl_slist_append(headers, "Content-Type:text/plain");
        }
        curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "PUT");

        curl_easy_setopt(c, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(c, CURLOPT_POSTFIELDS, payload.c_str());

        curl_multi_add_handle(curlm, c);

        DLOG_S(INFO) << "perform HTTP PUT";
        int result = exec();
        if(result==0){
            ss << readBuffer;
        }else{
            //error
        }
    }
    return ss.str();
}