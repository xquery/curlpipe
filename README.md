# curlscript (develop)
WARNING - under development (as in everything is probably broken) test

[![Build Status](https://travis-ci.org/xquery/curlscript.svg?branch=develop)](https://travis-ci.org/xquery/curlscript)
[![Coverage Status](https://coveralls.io/repos/github/xquery/curlscript/badge.svg?branch=develop)](https://coveralls.io/github/xquery/curlscript?branch=develop)

[Curl](https://curl.haxx.se/) is a great ~~swiss army knife~~ http client which does much more then just make it easy to work with HTTP.

Over the years, the curl command line interface has grown rapidly, exposing hundreds of flags - most users initially only ever invoke a subset of features, learning more advanced features over time.

Curlscript is an experiment in exposing much of curl's goodness via a little [DSL](https://en.wikipedia.org/wiki/Domain-specific_language) making it easy to build up
pipelines of execution using natural semantics.  

## Getting started

## Usage

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
```

POST some xml
```$xml
&lt;person id="1"&gt;&lt;name&gt;James Fuller&lt;name&gt;&lt;/person&gt; | [http://www.httpbin.org/post] ;
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
$mypayload = [/tmp/payload];
$mypayload | [http://www.httpbin.org/delete]
```
Comparison
```$bash
[http://www.httpbin.org/get].url == 'http://www.httpbin.org/get'
```
```$bash
[http://www.httpbin.org/get] .url != 'http://www.example.com'
```

## Points of Interests

* At this stage I have not thought much about performance, once I learn more I expect to rewrite internals
* The curlscript language is defined with an [EBNF](etc/csparser.ebnf) which is used to produce a strict parser.
* I have unabashedly stolen syntactical forms and conventions from several languages - it maybe this approach creates a ['chimera'](https://en.wikipedia.org/wiki/Chimera_(mythology))
causing discomfort for most people ... please do [raise an issue](https://github.com/xquery/curlscript/issues) if you feel strongly where syntax must change.

## Developing curlscript

### Compiling

Just do the normal cmake thing:
```
mkdir build
cd build
cmake ..
make 
```

Note that running cmake will pull down dependencies and requires access to internet.

Some code dependencies (ex. openssl) should already be existing on the platform. The build will complain
if missing.

### Testing


### Release Package

* edit CMakeLists.txt setting DISABLE_TESTS=true
* run cpack in cmake build directory
```
> cpack --config CPackConfig.cmake

````

### Dependencies
This project depends on the following external libs:
[loguru](https://github.com/emilk/loguru)
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