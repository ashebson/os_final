Aryeh Shebson 210033585 | 2020

commands:
PING - return 'PONG' from server
PRNT - prints text on server
RUN - runs terminal command in server
UPDATE - updates server's exe file
VERSION - returns version of currently running exe file 

notes:
- any command using a parameter must recive a parameter as a string directly after the command
- the exe files attached above only work if run on the same computer (ip: 127.0.0.1) 
inorder to run the program on 2 different computers you must edit the constants (DEFAULT_IP, DEFAULT_PORT) which are declared
in the source.cpp file in each project and recompile

the program runs automaticlly on startup
the registry key responsible in located at Software\\Microsoft\\Windows\\CurrentVersion\\Run
