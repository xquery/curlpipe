// GET and save
[http://localhost:81/get "User-Agent":"curlpipe" "Accept":"text/plain"] > [file://tmp/output.json]
[http://localhost:81] > [file://tmp/output1.json]

// GET multiple and save
[http://localhost:81/uuid "Accept":"applicationxml"], [http://localhost:81/uuid] > [/tmp/output2.json]

// retrieve file
[http://localhost:81/get]
[http://localhost:81/get#test]
[http://localhost:81/get#test?test=1]

// GET and post and save file
[http://localhost:81/get] | [http://localhost:81/post "Accept":"application/xml"] > [file:///tmp/test1.json] ;

// GET and save file
[http://localhost:81/drip] > [/tmp/test2.json]

// GET and append to file
[http://localhost:81/bytes/44] >> [file:///tmp/test2.json]

// POST data
"test=1&a=2" | [http://localhost:81/post]
"<test>1</test>" | [http://localhost:81/post]
"{'test':1}" | [http://localhost:81/post]

// GET and post and post and save file
[http://localhost:81/get] | [http://localhost:81/post] | [http://localhost:81/post] > [/tmp/test3.json]

// perform DELETE
[] | [http://localhost:81/delete]
[] =| [http://localhost:81/delete]

// perform PUT

[http://localhost:81/get] =| [http://localhost:81/put]
