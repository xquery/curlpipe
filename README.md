# curlscript (develop)
WARNING - under development (as in everything is probably broken) test

[![Build Status](https://travis-ci.org/xquery/curlscript.svg?branch=develop)](https://travis-ci.org/xquery/curlscript)
[![Coverage Status](https://coveralls.io/repos/github/xquery/curlscript/badge.svg?branch=develop)](https://coveralls.io/github/xquery/curlscript?branch=develop)

[Curl](https://curl.haxx.se/) is a great ~~swiss army knife~~ http client which does much more then  make it easy to work with HTTP, supporting a plethora of protocols addressable with a URI.

Over the years, the curl command line interface has grown - exposing hundreds of options - most users only ever invoke a subset of features, learning more advanced features over time.

Curlscript is an experiment in exposing much of curl's goodness via a little [DSL](https://en.wikipedia.org/wiki/Domain-specific_language) making it easy to build up
pipelines of execution using natural semantics rather then 'flag soup'.  

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
```

GET and save to file  
```$bash
[http://www.httpbin.org/get] > /tmp/response.txt
```

GET and narrow down result
```$bash
[http://www.httpbin.org/get] ? [/id] 
```

```$bash
[http://www.httpbin.org/get].id  
```

POST some json
```$bash
{"id":1 , "name":"James Fuller"} | [http://www.httpbin.org/post] ;

[/tmp/data.json] | [http://www.httpbin.org/post] ;
```

POST some xml
```$bash
<person id="1"><name>James Fulle<name></person> | [http://www.httpbin.org/post] ;

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
[http://www.httpbin.org/put] = {"test:1}
```

DELETE
```$bash
[http://www.httpbin.org/delete] = []
```
```$bash
[http://www.httpbin.org/delete] = [/dev/null]
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

* At this early stage, being intentionally [lazy](http://threevirtues.com/) in defining internals or worrying too much about performance.
* The curlscript language is defined with an [EBNF](etc/csparser.ebnf) which is used to produce a strict parser.
* Designing a programming language is hard - for me coherence and a parse tree that is easy to run are my first goals, please do [raise an issue](https://github.com/xquery/curlscript/issues) if you feel strongly where syntax could change.
* I find using [CMake](https://cmake.org/) non intuitive ... though will never learn it if I do not use it, hence its usage here. 

## Developing curlscript

Please [raise an issue](https://github.com/xquery/curlscript/issues) or fork and create a [pr](https://github.com/xquery/curlscript/pulls). 

### Compiling

Do the cmake thing:
```$bash
mkdir build
cd build
cmake ..
make 
make test
make install
```

Note that running cmake will pull down dependencies and requires access to internet. This is problematic 
approach for those wishing to build without accessing the internet and will change in the near future eg. should be
able to build and install deps independently.

The build should helpfully complain if there are other common dependencies missing from your platform (ex. openssl).

### Testing


### Release Package

* edit CMakeLists.txt setting DISABLE_TESTS=true
* run cpack in cmake build directory
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