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

#include <cxxopts.hpp>
#include <vector>
#include <numeric>
#include <functional>
#include <iostream>

#include <curlpipe/curlpipe.cpp>

using namespace std;

int banner(){
    cout << "curlpipe " << get_version() << " | ⓒ 2017-2018 James Fuller <jim.fuller@webcomposite.com> | https://github.com/xquery/curlpipe" << endl;
    return CS_OK;
}

int usage(){
    banner();
    cout << "\n> curlpipe mycurlpipe.cp \n\n"
        << "    -h | --help   : Help.\n"
        << "    -d | --debug  : Emit debug info logging.\n"
        << "    -i | --info   : Emit info logging.\n"
        << "    -l | --log    : Enable logging to file.\n"
        << "    -q | --quiet  : Suppress output to console.\n"
        << "    -a | --auth   : Pass a username:password pair as the argument.\n"
        << "    -A | --auth-type : Specify the auth mechanism (basic|digest).\n"
        << "    -p | --params : Define set of parameters for transclusion with file (json|xml).\n"
        << "    -P | --param  : Define parameter(s) for transclusion.\n"
        << "    -f | --file   : Supply curlpipe file uri.\n" << endl;
    
    return CS_OK;
}

cxxopts::Options setopts(){
    cxxopts::Options opts("curlpipe", "http pipelines with curl.");
    opts.add_options()
            ("positional",
             "Positional arguments: these are the arguments that are entered "
             "without an option", cxxopts::value<std::vector<std::string>>())
            ("h,help", "Help.")
            ("d,debug", "Emit debug info logging")
            ("i,info", "Emit info logging")
            ("l,log", "Enable logging to file", cxxopts::value<string>())
            ("s,serialiser", "Switch serialiser",cxxopts::value<string>())
            ("q,quiet", "Suppress output to console.")
            ("a,auth", "Pass a username:password pair as the argument.",cxxopts::value<string>())
            ("A,auth-type", "Specify the auth mechanism (basic|digest).",cxxopts::value<string>())
            ("p,params", "Define set of parameters for transclusion with file (json|xml).",cxxopts::value<string>())
            ("P,param", "Define parameter(s) for transclusion.",cxxopts::value<string>())
            ("o,options", "Define set of options controlling curlpipe behavior (json|xml).",cxxopts::value<string>())
            ("O,option", "Define option(s) controlling curlpipe behavior.",cxxopts::value<string>())
            ("f,file", "Supply curlpipe file uri.", cxxopts::value<string>());
    opts.parse_positional({"input", "output", "positional"});
    return opts;
}

int main(int argc, char** argv ){

#ifndef NDEBUG
    DLOG_S(INFO) << "debug mode";
#endif

    cxxopts::Options opts = setopts();
    auto result = opts.parse(argc,argv);

    if(result["help"].count() == 1){
        usage();
        return EXIT_SUCCESS;}

    if(result["log"].count() == 1){
        set_log_file(result["log"].as<string>());
        DLOG_S(INFO) << "set logging to " << result["log"].as<string>(); }

    DLOG_S(INFO) << "start processing...";
    if(result["info"].count() == 1){
        banner();
        set_log_verbosity_info();
    }else{
        set_log_verbosity_error();}

    if(result["debug"].count() == 1){
        set_log_verbosity_max();}

    if(result["positional"].count() == 1){
        DLOG_S(INFO) << "set positional opt";
        auto& v = result["positional"].as<std::vector<std::string>>();
        for (const auto& s : v) {
            curlpipe::exec(s, result["quiet"].count());}
    } else{
        if(result["file"].count() == 1){
            DLOG_S(INFO) << "set file opt";
            curlpipe::exec(result["file"].as<string>(), result["quiet"].count());
        }else{
            LOG_S(ERROR) << "must supply curlpipe file.";
            return EXIT_FAILURE; }}

    DLOG_S(INFO) << "finished processing, dumping output to stdout.";
    return EXIT_SUCCESS;
}
