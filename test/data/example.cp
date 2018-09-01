// set variable
$myvar = [https://www.google.com]
[https://www.google.com] > $myvar
[https://www.google.com] >> $myvar

// selection semantics
https://www.google.com . /html/meta[1]
$myvar[1]  # gets first multipart
$myvar[1] . /test/test/test

##################
# redirect to a file descriptor
##################
https://www.google.com 0>                                # to stdin
https://www.google.com 1>                                # to stdout
https://www.google.com > file:///tmp/test.txt            # to file
https://www.google.com 2> /errors.txt                    # write errors to file
[ www.google.com, bbc.com ] >> "file:///tmp/test.txt"    # append to file

##################
# HEAD method example
##################
www.google.com != []   # true if exists, HEAD under the covers

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



