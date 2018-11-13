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

#include "curlpipe.h"
#include "log.h"
#include "helpers.h"
#include "serializer.h"
#include "ast.cpp"
#include "eval.cpp"

#define CS_OK 0;

using namespace std;

namespace curlpipe{

  int exec(const string file_uri, const bool quiet) {

      DLOG_S(INFO) << "loading " << file_uri;
      return execScript(load_file(file_uri),quiet);
  }

    int execScript(const string script, const bool quiet) {

        bool indent = false;

        ASTserializer s(indent);
        wstring winput =convert(script);

        csparser parser(winput.c_str(), &s);
        try {
            DLOG_S(INFO) << "serializing to xml representation";
            parser.parse_CS(); }
        catch (csparser::ParseException &pe) {
            LOG_S(ERROR) << "parser error, " << convert(pe.getMessage());
            return EXIT_FAILURE; }

        DLOG_S(INFO) << s.getParsed();
        vector<expr> exprs = generate_ast(s.getParsed());

        std::ostringstream output;
        DLOG_S(INFO) << "evaluate AST";
        eval_exprs(exprs, output);

        if(!quiet){
            cout << output.str(); }
        return 0;
    }

  int exec(const string file_uri){
      return exec(file_uri, false);
  }
}
