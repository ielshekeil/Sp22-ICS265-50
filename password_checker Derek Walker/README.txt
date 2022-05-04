For proper funtion, the password_checker.c program assumes the following:

1.) All required sqlite3 files/libraries have been properly installed.
On Ubuntu/debian based linux distros that utilize the apt package manager, this can be acheived by the running the included 'setup.sh' script.
	
2.) The logins.csv file containing the credentials stored in the browser resides in the same directory as password_checker.c.
On Firefox:
	click the 3 horizontal lines in the upper right corner
	click passwords
	click the 3 horizontal dots in the upper right corner
	click "Export Logins", then "Export..."
	save the file as "logins.csv" in the same directory as password_checker.c
Alternativly, using the included logins.csv for proof of concept will work
	
3.) The rockyou.txt file resides in the same directory as password_checker.c. This has been included for convenience.
Theorectically, any newline separated wordlist should work, provided it is named rockyou.txt and resides in the same directory as password_checker.c
	
4.) To compile password_checker.c, be sure to include -lsqlite3.
Using gcc:	gcc password_checker.c -lsqlite3 -o password_checker

https://www.kaggle.com/datasets/wjburns/common-password-list-rockyoutxt


