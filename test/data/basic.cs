// GET and save
[http://localhost:81/uuid "Accept":"applicationxml"] > [file:///tmp/output.json]

// retrieve file
[file:///tmp/output.json]

// GET and post and save file
[http://localhost:81/get] | [http://localhost:81/post "Accept":"applicationxml"] > [/tmp/test1.json] ;

// GET and save file
[http://localhost:81/bytes/35] > [/tmp/test2.json]

// GET and append to file
[http://localhost:81/bytes/44] >> [/tmp/test2.json]

// GET and post and post and save file
[http://localhost:81/get] | [http://localhost:81/post] | [http://localhost:81/post] > [/tmp/test3.json]