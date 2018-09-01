# Curlpipe - DSL for making http execution pipelines.

(WARNING - under development as in everything is probably broken!)

[![Build Status](https://travis-ci.org/xquery/curlpipe.svg?branch=develop)](https://travis-ci.org/xquery/curlpipe)
[![Coverage Status](https://coveralls.io/repos/github/xquery/curlpipe/badge.svg?branch=develop)](https://coveralls.io/github/xquery/curlpipe?branch=develop)
[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat-square)](COPYING)


Curlpipe is a little DSL making it easy to build up http execution pipelines. Curlpipe uses [libcurl](https://curl.haxx.se/) under the covers, curl is a great ~~swiss army knife~~ http client that does much more then just make it easy to work with HTTP, supporting a plethora of URI addressable protocols. 

Over the years, the curl command line interface has grown - exposing many options with most users only ever invoking a subset of features, learning more advanced features over time. [Curlpipe](https://github.com/xquery/curlpipe) is a utility that exposes much of curl's goodness via a [DSL](https://en.wikipedia.org/wiki/Domain-specific_language) making it easy to build up
pipelines of execution using a 'little language' with built in primitives and natural bias for working with http.  

[Getting Started](#getting-started) | [Usage](#usage) | [Language](#the-curlpipe-language) | [Examples](#examples) | [Points of Interest](#points-of-interests) | [Developing](#developing-curlpipe) | [License](#license)

## Getting started

[Download](https://github.com/xquery/curlpipe/releases) the latest release or build the software.

To try it out, define a file (example.cp) 

```$bash
[http://www.httpbin.org/get] > [/tmp/output.txt]
```
and invoke curlpipe supplying that file as its only argument.

```$bash
> curlpipe example.cp
```

If everything worked out allright you should now observe the output from the URI saved to a file.

## Usage

To get help run

```$bash
> curlpipe -h
```

```$bash
curlpipe 0.1.0 | ⓒ 2017-2018 James Fuller <jim.fuller@webcomposite.com> | https://github.com/xquery/curlpipe

> curlpipe mycurlpipe.cp 
    -h | --help   : Help.
    -d | --debug  : Emit debug info logging.
    -i | --info   : Emit info logging.
    -l | --log    : Enable logging to file.
    -q | --quiet  : Suppress output to console.
    -a | --auth   : Pass a username:password pair as the argument.
    -A | --auth-type : Specify the auth mechanism (basic|digest).
    -p | --params : Define set of parameters for token replacement (json|xml).
    -P | --param  : Define parameter(s) for token replacement.
    -o | --options : Define set of options controlling curlpipe behavior (default is ~/.curlpiperc).
    -O | --option  : Define option(s) controlling curlpipe behavior.
```

Curlpipe flags for controlling how much information is emitted during processing.

| short  | long     | description   |
|--------|----------|---------------|
| -h     | --help   | Obtain help   | 
| -d     | --debug  | Emit debug info logging.|
| -i     | --info   | Emit info logging.|
| -l     | --log    | Enable logging to file.|
| -q     | --quiet  | Suppress output to console.|

### Authentication
The following flags set the default auth credentials and auth-type used by curlpipe.

| short  | long        | description   |
|--------|-------------|---------------|
| -a     | --auth      |  Pass a username:password pair as the argument.  |  
| -A     | --auth-type |  Specify the auth mechanism (basic|digest). |

~/.netrc (on windows ~/_netrc) is also supported by curlpipe: 
```$bash
> cat ~/.netrc
machine example.org
login myuser
password mypassword
```

### Parameters

Parameters maybe passed to curlpipe, replacing tokens in the curlpipe script.

| short  | long     | description   |
|--------|----------|---------------|
| -p     | --params | set file containing params (xml|json format).| 
| -P     | --param  | set a param (ex. -Pid=1).|

Parameters can be passed in via a file, for example, using data.json:
```$json
{"id":1, "name":"Ali G"}
```

The call to curlpipe would be
```$bash
> curlpipe -p data.json example.cp
```

or data.xml looks like 
```$xml
<params>
<param>
<name>id</name>
<value>1</value>
</param>
<param>
<name>name</name>
<value>Ali G</value>
</param>
</params>
```
and similarly called
```$bash
> curlpipe -p data.xml example.cp
```

Paramaters may also be individually defined, overidding params if used in conjunction with above -p flag.

```$bash
> curlpipe -Pid=1 -Pname=Ali G example.cp
```
### Options

By default, curlpipe looks for a ~/.curlpiperc file (on windows _curlpiperc) containing options.

| short  | long     | description   |
|--------|----------|---------------|
| -o     | --options | set file containing curlpipe options (xml|json format).| 
| -O     | --option  | set an option (ex. -Pid=1).|

Alternately you can set the location of this file with the -o flag. Here is an example:

```$bash
default.headers.content-type= Application/xml
```

Options can be overriden at the command line using the -O flag.

The set of options curlpipe exposes are:

| option | example  | description   |
|--------|----------|---------------|
|        |          | a file containing curlpipe options.| 
|        |          | set an option (ex. -Pdefault.headers.content-type=Application/xml).|


## The curlpipe language

Curlpipe defines a series of statement(s). The simplest statement defines retrieval of a URI:
```$bash
[http://www.httpbin.org/get] ;
```
where the seperator (;) is optional.

When this is run the output of dereferencing the URI is sent to stdout (con).

### Data types

In addition to a URI, curlpipe also supports boolean, literal, binary, xml, json and null data types.

| data type  | example |description   |
|------------|---------|--------------|
| URI        | [http://www.example.org]   | used to POST, PUT or DELETE.| 
| literal    |  "name=value;name=value"   | literal data value.|
| binary     |  ex. zip file              | binary data value.|
| xml        | <person><name>Tommy</name></person>        | well formed xml data.|
| json       | {id:1,name:"Tommy"}        | json data.|
| boolean    |    ? ?                     | true or false.|
| null       |   []                       | an empty/null value.|

Literal string data can be used to pass in name value pairs (using application/x-www-form-urlencoded content type): 
```$bash
"name=value;name=value" ;
```
Binary data (for example, a zip file) is also supported.

Support for common formats, like XML and json. 
```$xml
<person><name>Tommy</name></person> ;
```

```$json
{id:1,name:"Tommy"};
```

### Statements

A single curlpipe statement can be comprised of either

* a datatype
* dataype operator datatype

Operators perform actions (or test conditions) on datatypes. The following example shows how the contents of
/temp/data.json is sent to a HTTP endpoint supporting POST method.
```$bash
[/tmp/data.json] | [http://httpbin.org/post] 
```
If the endpoint URI supports PUT it may opt to use that. 

The following would POST xml to the URI.

```$bash
<person><name>Tommy</name></person> | [http://httpbin.org/post] 
```
or now with a json datatype.

```$bash
{id:1,name:"Tommy"} | [http://httpbin.org/post] 
```

The pipe operator can be used to perform an HTTP DELETE
```$bash
[] | [http://httpbin.org/delete] 
```

Like the PUT the pipe operator will deduce if the endpoint supports DELETE. This built in behavior is enabled
by curlpipe options (which can be set when invoking curlpipe).

Otherwise one is always free to force a PUT
```$bash
{id:1,name:"Tommy"} =| [http://httpbin.org/put]  
```
To force a DELETE method
```$bash
[] =| [http://httpbin.org/delete] 
```

To force a HEAD method use a conditional (explained further down)
```$bash
[http://httpbin.org/get] != []
```

### Operators

Operators chain together to build execution pipeline of arbitrary length.

```$bash
[/tmp/data.json] | [http://httpbin.org/post] > [/tmp/output.txt] | [http://httpbin.org/post]
```

The set of processing operators are:

| operator  | description   |
|-----------|---------------|
| &#124;    | used to POST, PUT or DELETE.| 
| &#62;     | redirect output to file.|
| &#62;&#62;| append output to file.|
| =&#124;   | used to force PUT or DELETE.|


### Parameters

Parameters can be passed into curlpipe 
```$bash
> curlpipe -Pname=Tommy example.cp -Pid=1
```
and used for token replacement (ex. ${token}) in either data or URIs.
```$bash
{name:"${name}"} | [http://httpbin.org/get/${id}]
```

### Conditional Logic

curlpipe implements boolean and null datatype which can be used with conditional operators
to test data values.

```$bash
[http://www.httpbin.org/get] == "test"
```

```$bash
[http://www.httpbin.org/get] ~= "test"
```

Where conditionals can be composited up using AND(&&) or OR(||) operators.

```$bash
[http://www.httpbin.org/get] ~= "test" && [http://www.httpbin.org/get] != "not test"
```

curlpipe only supports trinary logic, in the following form.

```$bash
[http://www.httpbin.org/get] =~ "test"
       ? [/tmp/success.txt] 
       : "failure" > [/tmp/fail.txt]       
```

The set of conditional operators are:

| operator  | description        |
|-----------|--------------------|
| ==        |  equal             | 
| !=        |  does not equal    | 
| ~=        |  regex text        | 
| &&        |  AND chain condition    | 
| &#124;&#124; | OR chain condition    | 

### Options

Curlpipe defines the following options

| option  | description        |
|---------|--------------------|
|         |  tba               | 
|         |  tba               | 
|         |  tba               | 
 
## Examples

Find more examples [here](https://github.com/xquery/curlpipe/tree/develop/docs/examples).

##### Retrieve (GET) and save to file   
```$bash
[http://www.httpbin.org/get] > [/tmp/output.txt]
```
##### Retrieve (GET) and save to file   
```$bash
[http://www.httpbin.org/get auth=myser:password auth-type=digest] > [/tmp/output3.txt]
```

##### Retrieve (GET) setting headers
```$bash
[http://www.httpbin.org/get "Accept":"application/json"]
```

##### GET and append to file  
```$bash
[http://www.httpbin.org/get] >> [/tmp/response.txt]
```
```$bash
[http://www.httpbin.org/get],[http://www.httpbin.org/uuid] >> [/tmp/response.txt]
```
##### GET and narrow down result
```$bash
[http://www.httpbin.org/get].id  
```

##### Trinary operator
```$bash
[http://www.httpbin.org/get] =~ "test"
       ? [/tmp/success.txt] 
       : "failure" > [/tmp/fail.txt]
       
```

##### POST json
```$bash
{"id":1 , "name":"James Fuller" age="${age}"} | [http://www.httpbin.org/post] ;
```

```$bash
[/tmp/data.json] | [http://www.httpbin.org/post "Content-type":"application/json"] ;
```

##### POST xml
```$bash
<person id="1"><name>John Smith<name><age>${age}</age></person> | [http://www.httpbin.org/post] ;
```

```$bash
[/tmp/data.xml] | [http://www.httpbin.org/post "Content-type":"application/xml"] ;
```

##### POST name=value
```$bash
"id=1&name=James Fuller" | [http://www.httpbin.org/post] ;
```

##### POST zip file
```$bash
[/tmp/mydoc.zip] | [http://www.httpbin.org/post "Content-type":"application/zip"] ;
```

##### PUT some json

will deduce if endpoint supports PUT
```$bash
{"test:1} | [http://www.httpbin.org/put]
```

force an HTTP PUT
```$bash
{"test:1} =| [http://www.httpbin.org/put]
```

##### DELETE
will deduce if endpoint supports DELETE
```$bash
[] | [http://www.httpbin.org/delete]
```

force an HTTP DELETE
```$bash
[/dev/null] =| [http://www.httpbin.org/delete] 
```

##### Transclusion in URI
```
${mypayload} | [http://www.httpbin.org/delete/${myid}]
```

##### Transclusion in payload
```
{"id":${myid}} | [http://www.httpbin.org/delete/${myid}]
```

##### Comparison
```$bash
[http://www.httpbin.org/get].url == 'http://www.httpbin.org/get'
```
```$bash
[http://www.httpbin.org/get] .url != 'http://www.example.com'
```

## Points of Interests

curlpipe is by design a 'little language' and most likely missing features from your 'favourite' language. It is intended as an adjunct to your existing script processing or host language. 

To provide a framework for design thoughts, here are a few possibly non obvious gaps in the current codebase.

* curlpipe is not intended as a drop in replacement for the curl tool (which is already a great [CLI](https://en.wikipedia.org/wiki/Command-line_interface)).
* Intentionally [lazy](http://threevirtues.com/) defining internals or worrying too much about performance at this stage.
* curlpipe language is defined with an [EBNF](etc/csparser.ebnf) which is used to produce a strict parser.
* Designing a programming language is hard - coherence and an easy to run AST are the first goals, please do [raise an issue](https://github.com/xquery/curlpipe/issues) if you feel strongly where syntax could change.
* I find using [CMake](https://cmake.org/) non intuitive ... its enforced usage on this project is an attempt to learn more (otherwise you would see a Makefile here!). 
* Currently curlpipe is http centric in initial releases.
* Woefully ignorant of windows platform ... looking at [appveyor](https://www.appveyor.com/) to eventually help solve that ([issue #2](https://github.com/xquery/curlpipe/issues/2)).

I have blatantly stolen (and deformed) concepts from many places (bash, unix pipes, etc..). The following projects provide alternate approaches to solving similar problems that
curlpipe is trying to address and worth a mention.

* [httpie](https://github.com/jakubroztocil/httpie) 
* [curlrc](https://github.com/benwebber/curlrc)

## Developing curlpipe

Please [raise an issue](https://github.com/xquery/curlpipe/issues) or make a contribution by forking the repository and creating a [pr](https://github.com/xquery/curlpipe/pulls). 

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
those wishing to build on a standalone machine off the network (tracked as [issue 1](https://github.com/xquery/curlpipe/issues/1))

The build will complain if there are other common dependencies missing from your platform (ex. openssl).

### Testing

Tests require [httpbin](http://httpbin.org) - which should be setup on port 81 - running the following docker command
or however way you feel comfortable installing.

```$bash
> docker run -p 81:80 kennethreitz/httpbin
```

After building usually it is just a matter of running make test target.

```$bash
> make test
```

or you could run directly
```$bash
> cd test
> ./runAllTests
```

### Generating the Parser

lib/curlpipe/csparser.cpp is generated using [REx Parser Generator](http://www.bottlecaps.de/rex/) with the 
following flags.

```$bash
-name csparser -tree -cpp -faster
```

### Release Package

Release packages are built using CPack.

Make a release build, ensuring you make a 'release' build and disable tests.
```$bash
> cmake -DCMAKE_BUILD_TYPE=Release -Dcurlpipe_DISABLE_TESTS=true -DENABLE_MANUAL=OFF -DBUILD_TESTING=OFF 
> make
```
Then run cpack in cmake build directory
```
> cpack --config CPackConfig.cmake

````

### Dependencies
This project depends on the following external libs:
* [curl](https://curl.haxx.se/): libcurl (of course!). 
* [REx Parser Generator](http://www.bottlecaps.de/rex/): Gunther Rademacher <grd@gmx.net> excellant parser generator. 
* [loguru](https://github.com/emilk/loguru): no fuss logging.
* [cxxopt](https://github.com/jarro2783/cxxopts): parsing command args.
* [googletest](https://github.com/google/googletest): testing.
* [rapidjson](https://github.com/miloyip/rapidjson): json munging.
* [pugixml](https://github.com/zeux/pugixml): xml dancing.

Please review these individual projects for more details on their own dependencies.

## License

curlpipe is provided under the [MIT License](COPYING)

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