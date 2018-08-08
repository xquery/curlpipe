# set
$opt = [/.curlscriptrc]
$opt.defaults.header.Accept = "applicationxml"

# http
[https://www.google.com/airplanes/1] = []

[https://www.google.com] | [http://test.com] | [http://test.com]

[https://www.google.com],[https://www.google.com]

[file://wwww.com] | [http://myalias.com] ;

[file://wwww.com], [file://wwww.com] | [http://myalias.com] ;

[file://wwww.com]| [http://myalias.com] |
      [http://myalias.com:21/test/kkkk/mmm] >> [file://test.txt];

$my = [http://myalias.com];

$my ? [/test]  # selector

$my ? [.test]   # selector

$my.test | [http://www.example.org/post]   # selector

"asdfasdfasdfsadfsadfsdafdsafsdafsdafsa" | [http://www.example.org/post]

"name=test" | [http://www.example.org/post]

[file://wwww.com], [file://wwww.com] ?/test != "kkk"

[https://www.webcomposite.com]  > [file:///a.txt]
[https://www.webcomposite.com] | [file:///b.txt]
