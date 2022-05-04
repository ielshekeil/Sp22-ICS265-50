/* Author: Kelly Skalsky
* Title: Police Report Database
* Class: ICS 265-50
* Description: This program creates a database for police reports.  Once a report is added to the database,
* it can be searched for or edited, or all reports in the database could be viewed.  Information for each report
* includes: (1) case number, (2) officer's last name, (3) date of report, (4) address of the incident,
* (5) problem nature, and (6) report status.
*/

/* Includes & Defines */ 
#include <stdio.h>
#include <string.h>
#include <C:\Users\kelly_mmtjklx\C Programs\sqlite3.h>
#define MAX 128
#define SQLCMDMAX 16384

/* Function Prototypes */
void add(sqlite3 *db);
void edit(sqlite3 *db);
void search(sqlite3 *db);
void view(sqlite3 *db);

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
       int i;
       for (i = 0; i < argc; i++) {
              printf("%s: %s   ", azColName[i], argv[i] ? argv[i] : "NULL");
       }
       printf("\n");
       return(0);
}

int main(void) {
       /* Variable Declarations */
       char command[10];            /* action to be executed - add, search, edit, view all, or quit */
       sqlite3 *db;                 /* database handle */
       sqlite3_stmt *ppStmt;        /* pointer to prepared statement */
       char *err_msg = 0;           /* pointer to error message (if any) */
       char *sql;                   /* pointer to SQL statement */
       int rc;                      /* integer to compare against SQLITE_OK */

       /* Open Database */
       rc = sqlite3_open(":memory:", &db);
       if (rc != SQLITE_OK) {
              fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
              sqlite3_close(db);
              return 1;
       } else {
              fprintf(stdout, "Opened database successfully\n");
       }

       /* Create SQL statement */
       sql = "CREATE TABLE REPORTS(" \
              "CN       INT     PRIMARY KEY     NOT NULL," \
              "NAME     TEXT                    NOT NULL," \
              "DATE     INT                     NOT NULL," \
              "ADDRESS  TEXT," \
              "PROBLEM  TEXT," \
              "STATUS   TEXT );";

       /* Execute SQL statement */
       rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
       
       if ( rc != SQLITE_OK ){
              fprintf(stderr, "SQL error: %s\n", err_msg);
              sqlite3_free(err_msg);
       } else {
              fprintf(stdout, "Table created successfully\n");
       }

       /* Welcome Message */
       printf("\nWelcome to the police report database.\n");
       printf("To add a new report to the database, enter 'Add'.\n");
       printf("To search for an existing report in the database, enter 'Search'.\n");
       printf("To edit an existing report in the database, enter 'Edit'.\n");
       printf("To view all reports in the datase, enter 'View All'.\n");
       printf("To quit, enter 'Quit'.\n");

       /* Get Command */
       printf("\nEnter a command: ");
       gets(command);
       while (strcmp (command, "Quit") != 0) {
              if (strcmp(command, "Add") == 0) {
                     add(db);
              } else if (strcmp(command, "Edit") == 0) {
                     edit(db);
              } else if (strcmp(command, "Search") == 0) {
                     search(db);
              } else if (strcmp(command, "View All") == 0) {
                     view(db);
              } else {
                     printf("\nInvalid command.  Please enter Add, Search, Edit, View All, or Quit");
              }
              printf("\nEnter a command: ");
              fflush(stdin);
              gets(command);
       }

       sqlite3_close(db);

       return(0);
}

void add(sqlite3 *db) {
       /* Variable Declarations */
       int case_number = 0;                             
       char last_name[MAX] = {"\0"};
       int date = 0;
       char address[MAX] = {"\0"};
       char problem_nature[MAX] = {"\0"};
       char report_status[MAX] = {"\0"};
       char sql_command[SQLCMDMAX];
       char *err_msg = 0;                        /* pointer to error message - if any */
       sqlite3_stmt *ppStmt;                     /* pointer to prepared statement */
       int rc;                                   /* integer to check against SQLITE_OK */

       /* Instructions */
       printf("\n\nYou are entering a new police report.  The following information will be needed:\n");
       printf("- Case Number (for calendar year 2022, case numbers should be between 22000001 and 22999999)\n");
       printf("- Reporting Officer's last name\n");
       printf("- Date of Report (date should be entered in yyyymmdd format)\n");
       printf("- Address of the incident\n");
       printf("- Problem Nature (Accident, Alarm, Assault, Burglary, Damage to Property, Fraud, Harassment, Missing Person,\n");
       printf("\t\t\tRobbery, Suspicious Activity, Theft, Traffic Stop)\n");
       printf("- Report Status (Submitted, Under Review, Approved, Complete)\n\n");
       
       /* Get Report Info */
       printf("Enter the case number of the report: ");
       scanf("%d", &case_number);
       printf("Enter the reporting officer's last name: ");
       scanf("%s", last_name);
       printf("Enter the date of the report: ");
       scanf("%d", &date);
       fflush(stdin);
       printf("Enter the address of the incident: ");
       gets(address);
       printf("Enter the problem nature: ");
       gets(problem_nature);
       printf("Enter the status of the report: ");
       gets(report_status);

       /* Create SQL statement */
       snprintf(sql_command, SQLCMDMAX, 
                     "Insert into REPORTS VALUES(%d, \"%s\", %d, \"%s\", \"%s\", \"%s\")",
                     case_number, last_name, date, address, problem_nature, report_status);
       printf("SQL Command: %s\n", sql_command);

       /* Prepared Statement */
       sqlite3_prepare_v2(db, sql_command, SQLCMDMAX, &ppStmt, NULL);

       /* Bind Statements */
       sqlite3_bind_int(ppStmt, 1, case_number);
       sqlite3_bind_text(ppStmt, 2, last_name, strlen(last_name), SQLITE_TRANSIENT);
       sqlite3_bind_int(ppStmt, 3, date);
       sqlite3_bind_text(ppStmt, 4, address, strlen(address), SQLITE_TRANSIENT);
       sqlite3_bind_text(ppStmt, 5, problem_nature, strlen(problem_nature), SQLITE_TRANSIENT);
       sqlite3_bind_text(ppStmt, 6, report_status, strlen(report_status), SQLITE_TRANSIENT);

       /* Step & Finalize Statments */
       sqlite3_step(ppStmt);
       rc = sqlite3_finalize(ppStmt);
       
       if ( rc != SQLITE_OK ) {
              fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
              sqlite3_free(err_msg);
       } else {
              fprintf(stdout, "Records created successfully\n");
       }
}

void search(sqlite3 *db) {
       /* Variable Declarations */
       int case_number = 0;                             
       char sql_command[SQLCMDMAX];
       sqlite3_stmt *ppStmt;                     /* pointer to prepared statement */
       int rc = 20;                              /* integer to check against SQLITE_OK */
       int step = 55;                            /* integer to check against SQLITE_ROW */

       /* Instructions */
       printf("\n\nYou are searching for an existing report.\n");
       printf("Enter the case number of the report you wish to view: ");
       scanf("%d", &case_number);
       
       /* Create SQL statement & Prepared Statement */
       snprintf(sql_command, SQLCMDMAX, "SELECT * FROM REPORTS WHERE CN = %d", case_number);
       printf("SQL Command: %s\n", sql_command);
       rc = sqlite3_prepare_v2(db, sql_command, SQLCMDMAX, &ppStmt, NULL);
       
       /* Step & Finalize Statements */
       step = sqlite3_step(ppStmt);
       
       if (step == SQLITE_ROW) {
              printf("Case Number: %d\n", sqlite3_column_int(ppStmt, 0));
              printf("Reporting Officer's Last Name: %s\n", sqlite3_column_text(ppStmt, 1));
              printf("Date of Report: %d\n", sqlite3_column_int(ppStmt, 2));
              printf("Address of Incident: %s\n", sqlite3_column_text(ppStmt, 3));
              printf("Problem Nature: %s\n", sqlite3_column_text(ppStmt, 4));
              printf("Report Status: %s\n", sqlite3_column_text(ppStmt, 5));
       }
       sqlite3_finalize(ppStmt);    
}

void edit(sqlite3 *db) {
       /* Variable Declarations */
       int case_number = 0;
       char last_name[MAX] = {"\0"};
       int date = 0;
       char address[MAX] = {"\0"};
       char problem_nature[MAX] = {"\0"};
       char report_status[MAX] = {"\0"};
       char response[4];                         /* yes or no reponse to whether user wants item edited */
       char sql_command[SQLCMDMAX] = {"\0"};
       char *err_msg = 0;                        /* pointer to error message - if any */
       sqlite3_stmt *ppStmt;                     /* pointer to prepared statement */
       int rc = 20;                              /* integer to check against SQLITE_OK */
       int step = 55;                            /* integer to check against SQLITE_ROW */

       /* Instructions */
       printf("\n\nYou are editing an existing report.\n");
       printf("Fields that can be edited are:\n");
       printf("- Reporting Officer's last name\n");
       printf("- Date of Report (date should be entered in yyyymmdd format)\n");
       printf("- Address of the incident\n");
       printf("- Problem Nature (Accident, Alarm, Assault, Burglary, Damage to Property, Fraud, Harassment, Missing Person,\n");
       printf("\t\t\tRobbery, Suspicious Activity, Theft, Traffic Stop)\n");
       printf("- Report Status (Submitted, Under Review, Approved, Complete)\n\n");
       printf("Enter the case number of the report you wish to edit: ");
       scanf("%d", &case_number);
       
       /* Search Record in SQL */
       snprintf(sql_command, SQLCMDMAX, "SELECT * FROM REPORTS WHERE CN = %d", case_number);
       printf("SQL Command: %s\n", sql_command);
       rc = sqlite3_prepare_v2(db, sql_command, SQLCMDMAX, &ppStmt, NULL);
       
       step = sqlite3_step(ppStmt);
       
       if (step == SQLITE_ROW) {
              printf("Case Number: %d\n", sqlite3_column_int(ppStmt, 0));
              printf("Reporting Officer's Last Name: %s\n", sqlite3_column_text(ppStmt, 1));
              printf("Date of Report: %d\n", sqlite3_column_int(ppStmt, 2));
              printf("Address of Incident: %s\n", sqlite3_column_text(ppStmt, 3));
              printf("Problem Nature: %s\n", sqlite3_column_text(ppStmt, 4));
              printf("Report Status: %s\n", sqlite3_column_text(ppStmt, 5));
       }
       sqlite3_finalize(ppStmt); 
       
       /* Edit Desired Fields */
       strncpy(sql_command, "\0", SQLCMDMAX);
       printf("\n\nDo you wish to edit the reporting officer's last name? Enter Yes or No: ");
       fflush(stdin);
       gets(response);
       while ( (strcmp(response, "Yes") != 0) && (strcmp(response, "No") != 0) ) {
              printf("Invalid response.  Please enter 'Yes' or 'No'\n");
              printf("Do you wish to edit the reporting officer's last name? Enter Yes or No: ");
              gets(response);
       } 
       if (strcmp(response, "Yes") == 0) {
              printf("Enter the new last name: ");
              fflush(stdin);
              gets(last_name);
              snprintf(sql_command, SQLCMDMAX, "UPDATE REPORTS SET NAME = '%s' WHERE CN = %d; ", last_name, case_number);
              sqlite3_prepare_v2(db, sql_command, SQLCMDMAX, &ppStmt, NULL);
              sqlite3_step(ppStmt);
              rc = sqlite3_finalize(ppStmt);
              if ( rc != SQLITE_OK ) {
                     fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                     sqlite3_free(err_msg);
              } else {
                     fprintf(stdout, "Records created successfully\n");
              }
       }

       strncpy(sql_command, "\0", SQLCMDMAX);
       printf("Do you wish to edit the date of report? Enter Yes or No: ");
       gets(response);
       while((strcmp(response, "Yes") != 0) && (strcmp(response, "No") != 0)) {
              printf("Invalid response.  Please enter 'Yes' or 'No'\n");
              printf("Do you wish to edit the date of report? Enter Yes or No: ");
              fflush(stdin);
              gets(response);
       }
       if (strcmp(response, "Yes") == 0) {
              printf("Enter the new date: ");
              fflush(stdin);
              scanf("%d", &date);
              snprintf(sql_command, SQLCMDMAX, "UPDATE REPORTS SET DATE = %d WHERE CN = %d; ", date, case_number);
              sqlite3_prepare_v2(db, sql_command, SQLCMDMAX, &ppStmt, NULL);
              sqlite3_step(ppStmt);
              rc = sqlite3_finalize(ppStmt);
              if ( rc != SQLITE_OK ) {
                     fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                     sqlite3_free(err_msg);
              } else {
                     fprintf(stdout, "Records created successfully\n");
              }
       }

       strncpy(sql_command, "\0", SQLCMDMAX);
       printf("Do you wish to edit the address of the incident? Enter Yes or No: ");
       fflush(stdin);
       gets(response);
       while((strcmp(response, "Yes") != 0) && (strcmp(response, "No") != 0)) {
              printf("Invalid response.  Please enter 'Yes' or 'No'\n");
              printf("Do you wish to edit the address of the incident? Enter Yes or No: ");
              gets(response);
       }
       if (strcmp(response, "Yes") == 0) {
              printf("Enter the new address: ");
              fflush(stdin);
              gets(address);
              snprintf(sql_command, SQLCMDMAX, "UPDATE REPORTS SET ADDRESS = '%s' WHERE CN = %d; ", address, case_number);
              sqlite3_prepare_v2(db, sql_command, SQLCMDMAX, &ppStmt, NULL);
              sqlite3_step(ppStmt);
              rc = sqlite3_finalize(ppStmt);
              if ( rc != SQLITE_OK ) {
                     fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                     sqlite3_free(err_msg);
              } else {
                     fprintf(stdout, "Records created successfully\n");
              }
       }
       
       strncpy(sql_command, "\0", SQLCMDMAX);
       printf("Do you wish to edit the problem nature? Enter Yes or No: ");
       fflush(stdin);
       gets(response);
       while((strcmp(response, "Yes") != 0) && (strcmp(response, "No") != 0)) {
              printf("Invalid response.  Please enter 'Yes' or 'No'\n");
              printf("Do you wish to edit the problem nature? Enter Yes or No: ");
              gets(response);
       }
       if (strcmp(response, "Yes") == 0) {
              printf("Enter the new problem nature: ");
              fflush(stdin);
              gets(problem_nature);
              snprintf(sql_command, SQLCMDMAX, "UPDATE REPORTS SET PROBLEM = '%s' WHERE CN = %d; ", problem_nature, case_number);
              sqlite3_prepare_v2(db, sql_command, SQLCMDMAX, &ppStmt, NULL);
              sqlite3_step(ppStmt);
              rc = sqlite3_finalize(ppStmt);
              if ( rc != SQLITE_OK ) {
                     fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                     sqlite3_free(err_msg);
              } else {
                     fprintf(stdout, "Records created successfully\n");
              }
       }
       
       strncpy(sql_command, "\0", SQLCMDMAX);
       printf("Do you wish to edit the report status? Enter Yes or No: ");
       fflush(stdin);
       gets(response);
       while((strcmp(response, "Yes") != 0) && (strcmp(response, "No") != 0)) {
              printf("Invalid response.  Please enter 'Yes' or 'No'\n");
              printf("Do you wish to edit the report status? Enter Yes or No: ");
              gets(response);
       }
       if (strcmp(response, "Yes") == 0) {
              printf("Enter the new report status: ");
              fflush(stdin);
              gets(report_status);
              snprintf(sql_command, SQLCMDMAX, "UPDATE REPORTS SET STATUS = '%s' WHERE CN = %d; ", report_status, case_number);
              sqlite3_prepare_v2(db, sql_command, SQLCMDMAX, &ppStmt, NULL);
              sqlite3_step(ppStmt);
              rc = sqlite3_finalize(ppStmt);
              if ( rc != SQLITE_OK ) {
                     fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                     sqlite3_free(err_msg);
              } else {
                     fprintf(stdout, "Records created successfully\n");
              }
       }  
}

void view(sqlite3 *db) {
       /* Variable Declarations */
       const char* data = "Callback function called";
       char *sql;                                       /* pointer to SQL statement */
       char *err_msg = 0;                               /* pointer to error message - if any */
       int rc = 20;                                     /* integer to check against SQLITE_OK */

       /* Create SQL Statement */
       sql = "SELECT * from REPORTS";

       /* Execute SQL Statement */
       rc = sqlite3_exec(db, sql, callback, (void*)data, &err_msg);
       if( rc != SQLITE_OK ) {
              fprintf(stderr, "SQL error: %s\n", err_msg);
              sqlite3_free(err_msg);
       } else {
              fprintf(stdout, "Operation done successfully\n");
       }
       
}