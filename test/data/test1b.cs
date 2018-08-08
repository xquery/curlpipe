$opt = [/.curlscriptrc]
$opt.defaults.header.Accept = "applicationxml"

[https://www.google.com] | [http://test.com] | [http://test.com]

[https://www.google.com],[https://www.google.com]

[file://wwww.com] | [http://myalias.com] ;

[file://wwww.com], [file://wwww.com] | [http://myalias.com] ;

[file://wwww.com]| [http://myalias.com] |
      [http://myalias.com:21/test/kkkk/mmm] >> [file://test.txt];

$my = [http://myalias.com];

$my ? [/test]
