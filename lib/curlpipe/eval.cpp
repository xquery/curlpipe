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
#include <tuple>

#include <curl/curl.h>
#include "log.h"
#include "expr.h"
#include "http.cpp"
#include "helpers.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/pointer.h>

#include <pugixml.hpp>

using namespace std;
using namespace rapidjson;

namespace curlpipe {

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
                                    if(!item.literal.empty()){
                                        DLOG_S(INFO) << "item literal:" << item.literal;
                                        if(item.selector.empty()) {
                                            out += item.literal;
                                        }else{
                                            string selector = item.selector;
                                            selector = "/" + replaceString(selector,".","/");

                                            string result = item.literal;
                                            Document json;
                                            if (json.Parse(result.c_str()).HasParseError()) {
                                                //could be xml
                                                pugi::xml_document doc;
                                                doc.load_string(result.c_str());
                                                if(doc){
                                                    pugi::xpath_node_set select = doc.select_nodes(selector.c_str());

                                                    for (pugi::xpath_node_set::const_iterator it = select.begin(); it != select.end(); ++it)
                                                    {
                                                        pugi::xpath_node node = *it;
                                                        ostringstream oss;
                                                        node.node().print(oss, "", pugi::format_raw);
                                                        out += oss.str();
                                                    }
                                                }else{
                                                    LOG_S(ERROR) << "selector can only be used on xml or json.";
                                                }
                                            }else{
                                                Value* results = Pointer(selector.c_str()).Get(json);
                                                rapidjson::StringBuffer buffer;
                                                buffer.Clear();
                                                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                                                results->Accept(writer);

                                                out += string( buffer.GetString() );
                                            }
                                        }
                                    }else{
                                        DLOG_S(INFO) << "item uri:" << item.uri.get_uri();
                                        if(!item.uri.get_uri().empty()){

                                            if(item.selector.empty()){
                                                out += http_get(item.uri.urlp, item.headers);
                                            }else{
                                                string selector = item.selector;
                                                selector = "/" + replaceString(selector,".","/");

                                                string result = http_get(item.uri.urlp, item.headers);
                                                // if json
                                                Document json;
                                                if (json.Parse(result.c_str()).HasParseError()) {
                                                   //could be xml
                                                    pugi::xml_document doc;
                                                    doc.load_string(result.c_str());
                                                    if(doc){
                                                        pugi::xpath_node_set select = doc.select_nodes(selector.c_str());

                                                        for (pugi::xpath_node_set::const_iterator it = select.begin(); it != select.end(); ++it)
                                                        {
                                                            pugi::xpath_node node = *it;
                                                            ostringstream oss;
                                                            node.node().print(oss, "", pugi::format_raw);
                                                            out += oss.str();
                                                        }
                                                    }else{
                                                        LOG_S(ERROR) << "selector can only be used on xml or json.";
                                                    }
                                                }else{
                                                    Value* results = Pointer(selector.c_str()).Get(json);
                                                    rapidjson::StringBuffer buffer;
                                                    buffer.Clear();
                                                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                                                    results->Accept(writer);

                                                    out += string( buffer.GetString() );
                                                }


                                            }
                                        }
                                        item.uri.cleanup();
                                    }
                                }
                            }else{
                                string op = (string)std::get<1>(statement);
                                DLOG_S(INFO) << "op:" << op;

                                // my kingdom for a switch on string literal!
                                if(op == ">"){
                                    for (auto & item2 : std::get<2>(statement)) {
                                        save_file(item2.uri.get_uri().substr(7),out);
                                        item2.uri.cleanup();
                                    }
                                }
                                if(op == ">>"){
                                    for (auto & item2 : std::get<2>(statement)) {
                                        append_file(item2.uri.get_uri().substr(7),out);
                                        item2.uri.cleanup();
                                    }
                                }
                                if(op == "|"){
                                    for (auto & item2 : std::get<2>(statement)) {
                                        if(out.empty()){
                                            out =http_delete(item2.uri.urlp, item2.headers);
                                        }else{
                                            out =http_post(item2.uri.urlp, out, item2.headers);
                                        }
                                        item2.uri.cleanup();
                                    }
                                }
                                if(op == "=|"){
                                    for (auto & item2 : std::get<2>(statement)) {
                                        if(out.empty()){
                                            out =http_delete(item2.uri.urlp, item2.headers);
                                        }else{
                                            out =http_put(item2.uri.urlp,out, item2.headers);
                                        }
                                        item2.uri.cleanup();
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
