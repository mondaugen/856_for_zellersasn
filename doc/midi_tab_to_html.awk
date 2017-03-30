BEGIN {FS="|"; print "<table>" }
{ 
    print "<tr>"
    print "<td>" $1 "</td>"
    print "<td>" $2 "</td>"
    print "<td>" $3 "</td>"
    print "</tr>"
}
END { print "</table>" }
