##################
# import options
##################
set opt = file:///.curlscript
set opt.conn = ~/.curlscript-connection

##################
# set and override options
##################
set opt.defaults.scheme = https
set opt.conn.auth.type = any
set opt.conn.auth.user = admin
set opt.conn.auth.pass = admin

set opt.defaults.headers.Accept = "application/json"
set opt.defaults.headers.Content-type = "application/json"

##################
# set variable
##################
$myvar = https://www.google.com
https://www.google.com > $myvar
https://www.google.com >> $myvar

##################
# selection semantics
##################
https://www.google.com ? /html/meta[1]
$myvar[1]  # gets first multipart
$myvar[1] ? /test/test/test

##################
# redirect to a file descriptor
##################
https://www.google.com 0>                                # to stdin
https://www.google.com 1>                                # to stdout
https://www.google.com > file:///tmp/test.txt            # to file
https://www.google.com 2> /errors.txt                    # write errors to file
[ www.google.com, bbc.com ] >> "file:///tmp/test.txt"    # append to file

##################
# POST method examples
##################
"name=John Wayne&age=24" | [https://test.com/post "Content-type":"plain/text"]  # POST
{"name":"John Wayne","age":24} | test.com                          # POST
<person><name>John Wayne</name><age>24</age></person> | test.com   # POST
$myvar | https://test.com/records

##################
# PUT method examples
##################
{"test:1} = https://test.com/resource/id

##################
# DELETE method examples
##################
/dev/null = http://test.com/resource/id      # if empty/null then this is a DELETE

##################
# HEAD method example
##################
www.google.com != /dev/null   # true if exists, HEAD under the covers

##################
# comparison tests
##################
www.google.com == {"test":1}    # true if equal
www.google.com != {<test>1</test>   # true if not equal
www.google.com =~ "regex"       # true if regex
$myvar =~ "regex"

##################
# control execution
##################
"my text" | test.com ; {""} | test.com        # both run irrespective, serially
"my text" | test.com & {""} | test.com        # both run irrespective, in parallel
"my text" | test.com && {""} | test.com       # 2nd part runs only if first is successful
"my text" | test.com || {""} | test.com       # 2nd part runs only if first is unsuccessful


##################
# advanced
##################
www.google.com >> "file:///tmp/test.txt" | test.com | test.com/test
( www.google.com, bbc.com ) >> "file:///tmp/test.txt"    # append to file

