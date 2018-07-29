# curlscript (develop)
[![Build Status](https://travis-ci.org/xquery/curlscript.svg?branch=develop)](https://travis-ci.org/xquery/curlscript)
[![Coverage Status](https://coveralls.io/repos/github/xquery/curlscript/badge.svg?branch=develop)](https://coveralls.io/github/xquery/curlscript?branch=develop)

[Curl](https://curl.haxx.se/) is a great ~~swiss army knife~~ http client which does much more then just working with HTTP. Curl is comprised of 
libcurl (programmatic api) and the curl tool (command line interface). Over the years the curl tool has grown, exposing hundreds
of flags ('knobs') - in most users only ever invoke a subset of these flags.

Curlscript is an experiment in exposing curl's goodness via a little DSL. 
   
```$bash
$opt = [/wwww.com]
set opt.defaults.header.Accept = "applicationxml"

[file://wwww.com] | [http://myalias.com] ;

[file://wwww.com], [file://wwww.com] | [http://myalias.com] ;

[file://wwww.com]| [http://myalias.com] |
      [http://myalias.com:21/test/kkkk/mmm] >> [file://test.txt];

$my = [http://myalias.com];

$my ? /test | [http://myalias.com]

$my ? /test

"asdfasdfasdfsadfsadfsdafdsafsdafsdafsa" | [http://www.example.org/post]

"name=test"| [http://www.example.org/post]

[file://wwww.com], [file://wwww.com] ?/test != "kkk"

[https://www.webcomposite.com]  > [file:///a.txt]
[https://www.webcomposite.com] | [file:///b.txt]
```


## Usage


## Developing

### Build & Test

```
> cmake -DCMAKE_BUILD_TYPE=Debug
> make
> make test
> make install

```

Note that running cmake will pull down dependencies and requires access to internet.


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