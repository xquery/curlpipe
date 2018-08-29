# curlscript (develop)
WARNING - under development (as in everything is probably broken!)

[![Build Status](https://travis-ci.org/xquery/curlscript.svg?branch=develop)](https://travis-ci.org/xquery/curlscript)
[![Coverage Status](https://coveralls.io/repos/github/xquery/curlscript/badge.svg?branch=develop)](https://coveralls.io/github/xquery/curlscript?branch=develop)

[Curl](https://curl.haxx.se/) is a great ~~swiss army knife~~ http client that does much more then just make it easy to work with HTTP, supporting a plethora of protocols addressable by a URI.

Over the years, the curl command line interface has grown - exposing hundreds of options - most users only ever invoke a subset of features, learning more advanced features over time.

Curlscript is an experiment in exposing much of curl's goodness via a little [DSL](https://en.wikipedia.org/wiki/Domain-specific_language) making it easy to build up
pipelines of execution using a language with built in primitives and natural semantics rather then 'flag soup'.  

## Getting started

Download a release or build the software.

## Usage

```$bash
> curlscript -h 
```

## Examples

GET   
```$bash
[http://www.httpbin.org/get]

[file:///tmp/test.txt]
```

GET setting headers
```$bash
[http://www.httpbin.org/get "Accept":"application/json"]
```

GET and save to file  
```$bash
[http://www.httpbin.org/get] > [/tmp/response.txt]
```

GET and append to file  
```$bash
[http://www.httpbin.org/get] >> [/tmp/response.txt]
```

GET and narrow down result
```$bash
[http://www.httpbin.org/get] . [/id] 

[http://www.httpbin.org/get].id  
```

trinary operator
```$bash
[http://www.httpbin.org/get] ? [/tmp/success.txt] : "failure" > [/tmp/fail.txt]

[http://www.httpbin.org/get] =~ "test"
       ? [/tmp/success.txt] 
       : "failure" > [/tmp/fail.txt]
       
```

POST some json
```$bash
{"id":1 , "name":"James Fuller" age="${age}"} | [http://www.httpbin.org/post] ;

[/tmp/data.json] | [http://www.httpbin.org/post] ;
```

POST some xml
```$bash
<person id="1"><name>John Smith<name><age>${age}</age></person> | [http://www.httpbin.org/post] ;

[/tmp/data.xml] | [http://www.httpbin.org/post] ;
```

POST name=value
```$bash
"id=1&name=James Fuller" | [http://www.httpbin.org/post] ;
```

POST zip file
```$bash
[/tmp/mydoc.zip] | [http://www.httpbin.org/post] ;
```

PUT some json
```$bash
{"test:1} = [http://www.httpbin.org/put]
```

DELETE
```$bash
[] = [http://www.httpbin.org/delete]
```
```$bash
[/dev/null] = [http://www.httpbin.org/delete] 
```

set variable
```$bash
$opt = [/.curlscriptrc]
```
```$bash
$opt.defaults.header.Accept = "applicationxml"
```
```$bash
$myid = 1;
$mypayload = [/tmp/payload];
$mypayload | [http://www.httpbin.org/delete]
```

transclusion in URI
```
$mypayload | [http://www.httpbin.org/delete/${myid}]
```

Comparison
```$bash
[http://www.httpbin.org/get].url == 'http://www.httpbin.org/get'
```
```$bash
[http://www.httpbin.org/get] .url != 'http://www.example.com'
```

## Points of Interests

* Intentionally [lazy](http://threevirtues.com/) in defining internals or worrying too much about performance at this stage.
* Curlscript language is defined with an [EBNF](etc/csparser.ebnf) which is used to produce a strict parser.
* Designing a programming language is hard - coherence and an easy to run parse tree are the first goals, please do [raise an issue](https://github.com/xquery/curlscript/issues) if you feel strongly where syntax could change.
* I find using [CMake](https://cmake.org/) non intuitive ... its enforced usage on this project is an attempt to learn more (otherwise you would see a Makefile here!). 
* Curlscript is not intended as a drop in replacement for the curl cli and is currently http centric in initial releases.
* Woefully ignorant of windows platform ... looking at [appveyor](https://www.appveyor.com/) to eventually help solve that.
 
## Developing curlscript

Please [raise an issue](https://github.com/xquery/curlscript/issues) or make a contribution by forking the repository and creating a [pr](https://github.com/xquery/curlscript/pulls). 

### Compiling

To build software, run cmake:

```$bash
> mkdir build
> cd build
> cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_MANUAL=OFF -DBUILD_TESTING=OFF 
> make 
> make test
> make install
```

Running cmake requires access to the internet to pull down dependencies. This is problematic  approach for 
those wishing to build on a standalone machine off the network (tracked as [issue 1](https://github.com/xquery/curlscript/issues/1))

The build will complain if there are other common dependencies missing from your platform (ex. openssl).

### Testing

After building usually it is just a matter of running make test target.

```$bash
> make test
```

or you could run directly
```$bash
> cd test
> ./runAllTests
```

### Release Package

Release packages are built using CPack.

Make a release build, ensuring you make a 'release' build and disable tests.
```$bash
> cmake -DCMAKE_BUILD_TYPE=Release -DDISABLE_TESTS=true -DENABLE_MANUAL=OFF -DBUILD_TESTING=OFF 
> make
```
Then run cpack in cmake build directory
```
> cpack --config CPackConfig.cmake

````

### Dependencies
This project depends on the following external libs:
* [REx Parser Generator](http://www.bottlecaps.de/rex/): Gunther Rademacher <grd@gmx.net> excellant parser generator. 
* [loguru](https://github.com/emilk/loguru): no fuss logging.
* [cxxopt](https://github.com/jarro2783/cxxopts): parsing command args.
* [googletest](https://github.com/google/googletest): testing.
* [rapidjson](https://github.com/miloyip/rapidjson): json munging.
* [pugixml](https://github.com/zeux/pugixml): xml dancing.

These dependencies will have their own dependencies. Please review individual projects for more details.

## License

curlscript is provided under the [MIT License](COPYING)

MIT License

Copyright (c) 2017-2018 James Fuller <jim.fuller@webcomposite.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.