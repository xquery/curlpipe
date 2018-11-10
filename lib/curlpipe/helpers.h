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

#ifndef CURLPIPE_HELPERS_H
#define CURLPIPE_HELPERS_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "version.h"
#include "log.h"

using namespace std;

string get_version(){
    std::ostringstream s;
    s <<  CURLPIPE_VERSION_MAJOR << "." << CURLPIPE_VERSION_MINOR << "." << CURLPIPE_VERSION_PATCH;
    return s.str();
}

int set_log_file(string log_uri) {
    DLOG_S(INFO) << "set log opt";
    loguru::add_file(log_uri.c_str(), loguru::Append, loguru::Verbosity_MAX);
    return 0;
};

int set_log_verbosity(int verbosity){
    DLOG_S(INFO) << "set logging to " << verbosity;
    loguru::g_stderr_verbosity = verbosity;
    return 0;
}

int set_log_verbosity_info(){
    set_log_verbosity(loguru::Verbosity_INFO);
    return 0;
}

int set_log_verbosity_error(){
    set_log_verbosity(loguru::Verbosity_ERROR);
    return 0;
}

int set_log_verbosity_max(){
    set_log_verbosity(loguru::Verbosity_MAX);
    return 0;
}

inline int check_file_exists(string file_uri){
    DLOG_S(INFO) << "checking if file " << file_uri << " exists";
    ifstream cs_file(file_uri);
    CHECK_F(cs_file.good(),"Failed to open '%s'", file_uri.c_str());
    return 0;
}

inline string load_file(string file_uri){
    check_file_exists(file_uri);
    std::ifstream t(file_uri);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

int save_file(string file_uri, string data){
    DLOG_S(INFO) << "saving output to " << file_uri.substr(7) ;
    DLOG_S(INFO) << data ;
    ofstream outfile;
    outfile.open(file_uri.substr(7), ios::out);
    outfile << "" + data;
    outfile.close();
    return 0;
}

int append_file(string file_uri, string data)
{
    ofstream file;
    file.open(file_uri.substr(7), std::ios::out | std::ios::app);
    if (file.fail())
        throw std::ios_base::failure(std::strerror(errno));
    file.exceptions(file.exceptions() | std::ios::failbit | std::ifstream::badbit);
    file << ""+data;
    return 0;
}

inline std::wstring convert( string input )
{
    std::wstring winput(input.length(), L' '); // Make room for characters
    std::copy(input.begin(), input.end(), winput.begin());
    return winput;
}

inline std::string convert(std::wstring wstring)
{
    std::string str( wstring.begin(), wstring.end() );
    return str;
}

#endif //CURLPIPE_HELPERS_H
