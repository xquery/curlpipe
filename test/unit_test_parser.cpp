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

//TEST(CurlScriptTestParser,TestCurlScriptParser){
//    XMLSerializer s(true);
//    string input = load_file("data/test3.cs");
//    wstring winput =convert(input);
//    csparser parser(winput.c_str(), &s);
//    try
//    {
//        parser.parse_CS();
//    }
//    catch (csparser::ParseException &pe)
//    {
//        FAIL();
//    }
//    EXPECT_EQ(s.getParsed(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?><CS><Expr><items><item><URI><TOKEN>[</TOKEN><scheme><TOKEN>https</TOKEN><TOKEN>://</TOKEN></scheme><hostport><host><nstring><alpha>w</alpha><alpha>w</alpha><alpha>w</alpha><schar>.</schar><alpha>w</alpha><alpha>e</alpha><alpha>b</alpha><alpha>c</alpha><alpha>o</alpha><alpha>m</alpha><alpha>p</alpha><alpha>o</alpha><alpha>s</alpha><alpha>i</alpha><alpha>t</alpha><alpha>e</alpha><schar>.</schar><alpha>c</alpha><alpha>o</alpha><alpha>m</alpha></nstring></host></hostport><path/><TOKEN>]</TOKEN></URI></item></items>  <Operator><TOKEN>&gt;</TOKEN></Operator> <item><URI><TOKEN>[</TOKEN><scheme><TOKEN>file</TOKEN><TOKEN>://</TOKEN></scheme><path><segment><TOKEN>/</TOKEN><string><character><alpha>a</alpha></character><character><schar>.</schar></character><character><alpha>t</alpha></character><character><alpha>x</alpha></character><character><alpha>t</alpha></character></string></segment></path><TOKEN>]</TOKEN></URI></item></Expr>\n"
//                             "<Expr><items><item><URI><TOKEN>[</TOKEN><scheme><TOKEN>https</TOKEN><TOKEN>://</TOKEN></scheme><hostport><host><nstring><alpha>w</alpha><alpha>w</alpha><alpha>w</alpha><schar>.</schar><alpha>w</alpha><alpha>e</alpha><alpha>b</alpha><alpha>c</alpha><alpha>o</alpha><alpha>m</alpha><alpha>p</alpha><alpha>o</alpha><alpha>s</alpha><alpha>i</alpha><alpha>t</alpha><alpha>e</alpha><schar>.</schar><alpha>c</alpha><alpha>o</alpha><alpha>m</alpha></nstring></host></hostport><path/><TOKEN>]</TOKEN></URI></item></items> <Operator><TOKEN>|</TOKEN></Operator> <item><URI><TOKEN>[</TOKEN><scheme><TOKEN>file</TOKEN><TOKEN>://</TOKEN></scheme><path><segment><TOKEN>/</TOKEN><string><character><alpha>b</alpha></character><character><schar>.</schar></character><character><alpha>t</alpha></character><character><alpha>x</alpha></character><character><alpha>t</alpha></character></string></segment></path><TOKEN>]</TOKEN></URI></item></Expr>\n"
//                             "<EOF/></CS>");
//}

TEST(CurlScriptTestParser,TestCurlScriptFailParser){
    ASTserializer s(true);
    string input = load_file("data/test-api-error1.cs");
    wstring winput =convert(input);
    csparser parser(winput.c_str(), &s);
    EXPECT_THROW(parser.parse_CS(),csparser::ParseException);
}