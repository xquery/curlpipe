$opt = [/wwww.com]
set opt.defaults.header.Accept = "applicationxml"

[https://www.google.com]

[https://www.google.com],[https://www.google.com]

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
