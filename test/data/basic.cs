[file:///tmp/output.txt]

[http://localhost:81/uuid]

[http://httpbin.org/get] | [http://localhost:81/post] > [/tmp/test1.json] ;

[http://localhost:81/bytes/35] > [/tmp/test2.json]

[http://localhost:81/bytes/44] >> [/tmp/test2.json]

[http://localhost:81/get] | [http://localhost:81/post] | [http://localhost:81/post] > [/tmp/test3.json]
