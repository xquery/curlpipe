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

#ifndef CURLPIPE_EXPR_H
#define CURLPIPE_EXPR_H

#include <string>
#include <curl/curl.h>

using namespace std;

namespace curlpipe {

    struct var {
        string var_name;
    };

    struct uri {
        CURLU *urlp = curl_url();
        void cleanup(){
            curl_url_cleanup(urlp);
        };
        CURLU* get_curl_uri() {
            return urlp;
        };
        string get_uri() {
            char *uris;
            curl_url_get(urlp, CURLUPART_URL, &uris, 0);
            string sURI(uris);
            return sURI;};
    };

    struct item {
        struct var;
        struct uri uri;
    };

    struct expr {
        string uid;
        int order;
        string var_name;
        string var_value;
        vector <tuple<vector<item>,string,vector<item>>> statements;
    };
}

#endif //CURLPIPE_EXPR_H
