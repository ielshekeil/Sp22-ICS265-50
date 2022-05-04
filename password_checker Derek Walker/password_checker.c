/*
The purpose of this program is to test the functionality of 
sqlite3_bind_text() functions

as shown on:
blog.quibb.org/2010/08/fast-bulk-inserts-into-sqlite/
*/

/*
###############################################################################
## Written by:	Derek Walker                                                 ##
## For: ICS 265-50 C Programming			                     ##
## Assignment name: Term Project 					     ##
## Purpose: The purpose of this program is to inspect the contents of a	     ##
## logins.csv file created by exporting the saved credentials from Firefox.  ##
## This program will check the password field of each set of credentials     ##
## against the rockyou.txt file and note if is found. This program also	     ##
## checks the length of the password and notes if it is at least 16 	     ##
## characters long. Using these two critera, the program then advise which   ##
## passwords should be changed						     ##
###############################################################################
*/

/*
Step 1: Export credentials saved in the browser
Step 2: Import credentials into an SQL database
Step 3: Check the password for each set of credentials against rockyou.txt
Step 4: If the password is found within the rockyou list, a flag is set for 
	those credentials.
Step 5: The user is displayed all credentials flagged and advises them to
	change those passwords immediatly.

logins.csv filepath:	/home/k7-avenger/Project_Files/logins.csv
rockyou.txt filepath:	/usr/share/wordlists/rockyou.txt

***** Be sure to include sqlite3.c in compilation of final code *****
*/


//Uncomment preprocessor directives as required
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	//required for using terminal cmds with system("")
#include <sqlite3.h>	//required for using SQLite with C
//#include <math.h>
#define MAX 500
#define BIG_MAX 2000
#define MIN_LENGTH 16
//Function Prototypes
	
int error_check(int rc, sqlite3 *database){
	if(rc != SQLITE_OK){
		printf("Error: %s\n", sqlite3_errmsg(database));
		sqlite3_close(database);
		return 1;
	}
}/*The purpose of this function is to check for and print errors
generated during sqlite3 functions*/

char in_list(char password[MAX]){
	FILE *rockyou;
	char *token_1;
	char pw[MAX];
	
	rockyou = fopen("rockyou.txt", "r");
	if(rockyou == NULL){
		printf("Error opening file...terminating program");
		return 'e';
	}
	
	while(fgets(pw, MAX, rockyou)){
		token_1 = strtok(pw, "\n");
		
		if(token_1 == NULL){
			return 'n';
		}
		
		if(strcmp(token_1, password) == 0){
			//printf("%s found\n", token_1);
			fclose(rockyou);
			return 'y';
		}
	}
	fclose(rockyou);
	//return 'n';

}/*The purpose of the funtion is to check the password against the rockyou
password list. If the password is found within the list, 'y' is returned
otherwise a 'n' is returned*/

//Main
int main(){
	char buffer[BIG_MAX];
	char sentence[MAX];
	char words[MAX][MAX];
	char pw_change_flag = 'n', pw_found = 'n';
	
	sqlite3 *db;				//sqlite database pointer
	sqlite3_stmt *statement = NULL;		//used for prepairing SQL
	char *sql_stmnt;			//statements
						
	int return_code;			//used for error checking
	int pw_length;
						
	FILE *fp; 				//file pointer
	
	char *token_0;				//used for string parsing

/******************************************************************************
                            Open file and database
******************************************************************************/	
	fp = fopen("logins.csv", "r");		//opens file with read
	if(fp == NULL){				
		printf("Error opening file");	
		return 1;
	}//Terminates program if logins.csv isn't opened
	
	error_check(sqlite3_open("prototype.db", &db), db);
	//opens database
	
	sql_stmnt = 	"DROP TABLE IF EXISTS Creds;"	//del table if exists
			"CREATE TABLE Creds("		//create new table
			"url TEXT, "			//column for url
			"username TEXT, "		//column for uname
			"password TEXT, "		//column for pw
			"password_length INT, "		//column for pw_length
			"is_breached CHAR, "		//flag if pw found
			"change_required TEXT);";	//flag for pw change
			
	error_check(sqlite3_prepare_v2(db, sql_stmnt, strlen(sql_stmnt)+1,
		&statement, NULL), db);
	error_check(sqlite3_exec(db, sql_stmnt, 0, 0, NULL), db);
	
	while(fgets(sentence, MAX, fp)){
				
		token_0 = strtok(sentence, "\",\"");	//removes " and ,
		
		error_check(sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL),
			db);
		
		for(int i = 0; i < 3; i++){		
			strcpy(words[i], token_0);	
			token_0 = strtok(NULL, "\",\"");	
			
			if(i == 2){
				pw_found = in_list(words[i]);
				pw_length = strlen(words[i]);
				if((pw_length < MIN_LENGTH) || (pw_found == 'y')){
					pw_change_flag = 'y';	
				} else pw_change_flag = 'n';
				if(pw_change_flag == 'y'){
					printf("Password of '%s' for '%s' is too "
						"short,\nor has been part of a "
						"data breach.\nPlease update "
						"to a more secure password\n\n",
						words[2], words[0]);
				}
			}
			//printf("%s ", words[i]);
			
		}//loop to remove remaining " and , from lines
		//stopping at 3 for URL, uname, and pw	
		
		sprintf(buffer, "INSERT INTO creds VALUES ('%s', '%s', '%s', '%d', '%c', '%c')",
			words[0],		//url
			words[1],		//uname
			words[2],		//pw
			pw_length,		//lenth of pw
			pw_found,		//flag to indicate pw was found in list
			pw_change_flag);	//flag to indicate pw must change
			
			
		error_check(sqlite3_exec(db, buffer, NULL, NULL, NULL), db);
		error_check(sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL,
			NULL), db);
		
		//printf("\n"); //used to improve read-ability during testing
		
	}
	
	
	fclose(fp);
	error_check(sqlite3_exec(db, "PRAGMA table_info('Creds')", NULL, NULL, NULL), db);
	error_check(sqlite3_finalize(statement), db);
	error_check(sqlite3_close(db), db);
	return 0;
}
