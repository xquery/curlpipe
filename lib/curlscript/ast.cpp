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

#include <vector>
#include "log.h"
#include "expr.h"
#include <pugixml.hpp>

using namespace std;

namespace curlscript {
    
    vector<expr> generate_ast(string parsed){

        pugi::xml_document doc;
        doc.load_string(parsed.c_str());
        std::vector<expr> exprs;

        pugi::xpath_node_set expressions = doc.select_nodes("/CS/Expr");

        for (pugi::xpath_node_set::const_iterator it = expressions.begin(); it != expressions.end(); ++it)
        {
            expr cur;
            pugi::xpath_node node = *it;
            pugi::xml_node expression = node.node();

            int i =0;
            for (pugi::xml_node expr_part: expression.children())
            {
                string part_name = expr_part.name();
                if( part_name == "items" && i == 0)  {
                    //must handle multiple item


                    string scheme = expr_part.child("item").child("URI").child("scheme").text().as_string();
                    string host = expr_part.child("item").child("URI").child("hostport").child("host").child(
                            "nstring").text().as_string();
                    string path = expr_part.child("item").child("URI").child("path").child("segment").child(
                            "string").text().as_string();
                    string uri = scheme + host + "/" + path;

                    std::vector<item> items;
                    item cur_item;

                    cur_item.scheme = scheme;
                    cur_item.host = host;
                    cur_item.path = path;
                    DLOG_S(INFO) << scheme;

                    items.push_back(cur_item);
                    cur.items1 = items;
                }
                if(part_name == "Operator")
                {
                    cur.op = expr_part.text().as_string();
                }
                if( part_name == "items" && i > 0)  {
                    string scheme = expr_part.child("item").child("URI").child("scheme").text().as_string();
                    string host = expr_part.child("item").child("URI").child("hostport").child("host").child(
                            "nstring").text().as_string();
                    string path = expr_part.child("item").child("URI").child("path").child("segment").child(
                            "string").text().as_string();
                    string uri = scheme + host + "/" + path;

                    std::vector<item> items;

                    item cur_item;

                    cur_item.scheme = scheme;
                    cur_item.host = host;
                    cur_item.path = path;
                    items.push_back(cur_item);
                    cur.items2 = items;
                }
                i++;
            }
            exprs.push_back(cur);
        }
    return exprs;
    }
}
