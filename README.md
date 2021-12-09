# Client---Server-Database
*Client - Server Database using Socket API in C*

I coded a client - server database that uses the Socket API in C as my first project on Networking. 
The client will send the server different commands given below and the server will read and write from a binary file that serves as DB.

The server will only allow for one client at a time. If the client closes connection, the server will still run until it is manually closed.
The server and client uses PORT number 20047 and the host name is localhost.


DATABASE COMMANDS
1: GET (sort by):
get fname 
get lname 
get SID
get gpa

2: PUT:
put lname,fname,initial(char),SID,gpa

3: SAVE:
save

4: DELETE:
delete SID (5-digit integer)

5: INFORMATION:
info

6: CLOSE CLIENT:
close
