options
* https vs http scheme
* relative vs absolute
* parallel vs serial

https://www.google.com
    > file:///tmp/test.txt
        | test.com
            | test.com/test

("element1" "element2" "element3") | test.com
"my text" | test.com ; {""} | test.com        # both run irrespective, serially
"my text" | test.com & {""} | test.com        # both run irrespective, in parallel

"my text" | test.com && {""} | test.com       # 2nd part runs only if first is successful
"my text" | test.com || {""} | test.com       # 2nd part runs only if first is unsuccessful

{""} | test.com

<test/> | test.com

${var} | test.com

www.google.com >> "file:///tmp/test.txt" | test.com | test.com/test

[ www.google.com, bbc.com ] >> "file:///tmp/test.txt"    # append to file

[ jim@test:www.google.com:9999, bbc.com ] &> "output.txt"   # copies output and error to output.txt