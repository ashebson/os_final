# os_final


Aryeh Shebson 210033585 | 2020
### Commands: 
---------------
- PING - return 'PONG' from server 
- PRINT - prints text on server 
- RUN - runs terminal command in server 
- UPDATE - updates server's exe file 
- VERSION - returns version of currently running exe file 
- INJ - injects local dll into process on server 
- SHOW - shows terminal windows on server 
- HIDE - hides terminal windows on server 
- GRAB - sends file from server 
- DELETE - deletes file on server
### Notes:
--------------
* any command using a parameter must recive a parameter as a string directly after the command
* if two paramaters are necicery enter "-" between paramaters (eg. INJ c:/mydll.dll-c:/windows/system32/notepad.exe)
* if both server and client are on the same host, ip may be entered as '/'
* the program runs automaticlly on startup the registry key responsible in located at Software\Microsoft\Windows\CurrentVersion\Run
