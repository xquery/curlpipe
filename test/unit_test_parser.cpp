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

#include <curlscript/curlscript.cpp> 
#include <gtest/gtest.h>

using namespace std;

TEST(CurlScriptTestParser,TestCurlScriptParser){
    XMLSerializer s(true);
    string input = load_file("data/test3.cs");
    wstring winput =convert(input);
    csparser parser(winput.c_str(), &s);
    try
    {
        parser.parse_CS();
    }
    catch (csparser::ParseException &pe)
    {
        FAIL();
    }
    EXPECT_EQ(s.getParsed(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<CS>\n  <Exprs>\n    <Expr>\n      <URI>\n        <scheme>\n          <string>https</string>\n          <TOKEN>://</TOKEN>\n        </scheme>\n        <host>\n          <string>www.webcomposite.com</string>\n        </host>  \n        <path/>\n      </URI>\n      <Operator>\n        <TOKEN>&gt;</TOKEN>\n      </Operator> \n      <URI>\n        <scheme>\n          <string>file</string>\n          <TOKEN>://</TOKEN>\n        </scheme>\n        <path>\n          <segment>\n            <TOKEN>/</TOKEN>\n            <string>a.txt</string>\n          </segment>\n        </path>\n      </URI>\n    </Expr>\n\n    <Expr>\n      <URI>\n        <scheme>\n          <string>https</string>\n          <TOKEN>://</TOKEN>\n        </scheme>\n        <host>\n          <string>www.webcomposite.com</string>\n        </host>  \n        <path/>\n      </URI>\n      <Operator>\n        <TOKEN>|</TOKEN>\n      </Operator> \n      <URI>\n        <scheme>\n          <string>file</string>\n          <TOKEN>://</TOKEN>\n        </scheme>\n        <path>\n          <segment>\n            <TOKEN>/</TOKEN>\n            <string>b.txt</string>\n          </segment>\n        </path>\n      </URI>\n    </Expr>\n  </Exprs>\n\n  <EOF/>\n</CS>");
}

TEST(CurlScriptTestParser,TestCurlScriptFailParser){
    XMLSerializer s(true);
    string input = load_file("data/test3-error.cs");
    wstring winput =convert(input);
    csparser parser(winput.c_str(), &s);
    EXPECT_THROW(parser.parse_CS(),csparser::ParseException);
}