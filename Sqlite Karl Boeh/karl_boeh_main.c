#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

/* Function Prototypes */

void draw_intro();                      /* Function prototype for intro print statements       */
void introduction();                    /* Function prototype for introduction to program      */
void create_first_database();           /* Function prototype for creating first database      */
void create_second_database();          /* Function prototype for creating second database     */
void create_cybersecurity_table();      /* Function prototype for creating first table         */
void create_computer_science_table();   /* Function prototype for creating second table        */
void insert_data_cybersecurity();       /* Function prototype for inserting first data set     */
void insert_data_computer_science();    /* Function prototype for inserting second data set    */
void print_records_cybersecurity();     /* Function prototype to print CYBERSECURITY records   */
void print_records_computer_science();  /* Function prototype to print COMPUTERSCIENCE records */
void update_records();                  /* Function prototype to update incorrect records      */
void print_updated_entries();           /* Function prototype to print updated records         */
void print_ics_records_cyber();         /* Function prototype to print ICS classes in cyber    */
void print_ics_records_cs();            /* Function prototype to print ICS classes in CS       */

/* This function declaration is publicly available code found on tutorialspoint.com */
/* It is used with the sqlite3_exec() library function for executing SQL statements */

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);

   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");
   return 0;
}

/* MAIN FUNCTION */

int main()
{
    draw_intro();
    introduction();
    printf("Press ENTER to create first database:");
    getchar();
    create_first_database();
    printf("%c", '\n');
    printf("Press ENTER to create second database:");
    getchar();
    create_second_database();
    printf("\n");
    printf("Both databases have been successfully created.\n");
    printf("Tables will now be inserted into our databases.\n");
    printf("%c", '\n');
    printf("Press ENTER and a table named CYBERSECURITY will\n");
    printf("be created in the first database:");
    getchar();
    create_cybersecurity_table();
    printf("%c", '\n');
    printf("Press ENTER and a table named COMPUTERSCIENCE will\n");
    printf("be created in the second database:");
    getchar();
    create_computer_science_table();
    printf("%c", '\n');
    printf("Both tables have been successfully created.\n");
    printf("The tables will now have data inserted.\n");
    printf("%c", '\n');
    printf("Press ENTER to create records for table CYBERSECURITY:");
    getchar();
    insert_data_cybersecurity();
    printf("%c", '\n');
    printf("Press ENTER to create records for table COMPUTERSCIENCE:");
    getchar();
    insert_data_computer_science();
    printf("%c", '\n');
    printf("ALL records that were just created in table CYBERSECURITY\n");
    printf("will now be displayed.\n");
    printf("Press ENTER to display CYBERSECURITY records:");
    getchar();
    printf("%c", '\n');
    print_records_cybersecurity();
    printf("ALL records that were just created in table COMPUTERSCIENCE\n");
    printf("will now be displayed.\n");
    printf("Press ENTER to display COMPUTERSCIENCE records:");
    getchar();
    printf("%c", '\n');
    print_records_computer_science();
    printf("%c", '\n');
    printf("Some entries in the CYBERSECURITY table are incorrect.\n");
    printf("These entries can be updated with a SQL UPDATE statment.\n");
    printf("Press ENTER to update these records:");
    getchar();
    printf("%c", '\n');
    update_records();
    print_updated_entries();
    printf("Two functions will now execute which use SQL SELECT statements\n");
    printf("to select only the entries from the CYBERSECURITY and COMPUTERSCIENCE\n");
    printf("tables where DEPARTMENT = ICS\n");
    printf("Press ENTER to execute these functions.\n");
    getchar();
    print_ics_records_cyber();
    print_ics_records_cs();
    printf("This is the end of the program. Thanks for watching.\n");
    return 0;

}



/* Function Declarations */

/* Function declaration for function that displays name, class, and project */

void draw_intro(){
printf("  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  \n");
printf(" @------------------------------------------------------------------@ \n");
printf("@-------------------------     KARL BOEH     ------------------------@\n");
printf("@--------------------     TERM PROJECT ICS 265     ------------------@\n");
printf("@--------------------------     SQLITE     --------------------------@\n");
printf("@--------------------------------------------------------------------@\n");
printf(" @------------------------------------------------------------------@ \n");
printf("  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  \n");
printf("\n");
}

/* Function declaration for introduction to program */

void introduction(){
printf("This program uses the sqlite3 C library and shows some ways\n");
printf("that it can be used in a C program. First, the program will\n");
printf("create two separate databases using an sqlite function. Next,\n");
printf("tables will be created in each database. After the tables are\n");
printf("created, data will be entered into each table and the data\n");
printf("will be printed to the screen. Some of this data will then be\n");
printf("edited and SQL queries will be used to search for specific\n");
printf("information in the databases.\n");
printf("%c", '\n');
}

/* Function Declaration for function that creates the first database */

void create_first_database() {
   sqlite3 *db_handle_1;
   int rc;
   rc = sqlite3_open("cybersecurity_classes.db", &db_handle_1);
   if (!rc){
        printf("First database created successfully\n");
   }
    else {
        printf("Error creating first database.");
    }
   sqlite3_close(db_handle_1);
}

/* Function Declaration for function that creates the second database */

void create_second_database() {
   sqlite3 *db_handle_2;
   int rc2;
   rc2 = sqlite3_open("computer_science_classes.db", &db_handle_2);
   if (!rc2){
        printf("Second database created successfully\n");
   }
    else {
        printf("Error creating second database.");
    }
   sqlite3_close(db_handle_2);
}

/* Function declaration for function that creates first table*/

void create_cybersecurity_table(){
   sqlite3 *db_handle_1;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   rc = sqlite3_open("cybersecurity_classes.db", &db_handle_1);

   sql = "CREATE TABLE CYBERSECURITY("
         "ID INT PRIMARY KEY                  NOT NULL,"
         "COURSENAME             CHAR(100)    NOT NULL,"
         "DEPARTMENT             NAME         NOT NULL,"
         "COURSENUMBER           INT          NOT NULL,"
         "CREDITS                INT          NOT NULL);";

   rc = sqlite3_exec(db_handle_1, sql, callback, 0, &zErrMsg);
   if (rc){
       printf("Error Creating Table\n");
   } else {
       printf("Table created successfully\n");
   }
   sqlite3_close(db_handle_1);
}

/* Function declaration for function that creates second table*/

void create_computer_science_table(){
   sqlite3 *db_handle_2;
   char *zErrMsg = 0;
   int rc2;
   char *sql_2;
   rc2 = sqlite3_open("computer_science_classes.db", &db_handle_2);

   sql_2 = "CREATE TABLE COMPUTERSCIENCE("
         "ID INT PRIMARY KEY                  NOT NULL,"
         "COURSENAME             CHAR(100)    NOT NULL,"
         "DEPARTMENT             NAME         NOT NULL,"
         "COURSENUMBER           INT          NOT NULL,"
         "CREDITS                INT          NOT NULL);";

   rc2 = sqlite3_exec(db_handle_2, sql_2, callback, 0, &zErrMsg);
   if(rc2){
       printf("Error Creating Table\n");
   } else {
       printf("Table created successfully\n");
   }
   sqlite3_close(db_handle_2);
}

/* Function declaration for function that inserts data into CYBERSECURITY table */

void insert_data_cybersecurity(){
   sqlite3 *db_handle_1;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   rc = sqlite3_open("cybersecurity_classes.db", &db_handle_1);

   sql =
         "INSERT INTO CYBERSECURITY (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (1, 'Introduction to Computer Forensics', 'WRIT', '280', 4 ); "
         "INSERT INTO CYBERSECURITY (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (2, 'Computer Security', 'CYBR', '332', 4 ); "
         "INSERT INTO CYBERSECURITY (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (3, 'Networking Protocols and Analysis', 'CYBR', '362', 4 ); "
         "INSERT INTO CYBERSECURITY (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (4, 'Vulnerability Assessment and Penetration Testing', 'CYBR', '412', 4 ); "
         "INSERT INTO CYBERSECURITY (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (5, 'Cryptography for Cybersecurity Practitioners', 'CYBR', '432', 4 ); "
         "INSERT INTO CYBERSECURITY (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (6, 'Cyber Operations', 'CYBR', '442', 4 ); "
         "INSERT INTO CYBERSECURITY (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (7, 'Computer Organization and Architecture', 'ICS', '232', 16 ); "
         "INSERT INTO CYBERSECURITY (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (8, 'Database Management Systems', 'ICS', '311', 4 ); ";

   rc = sqlite3_exec(db_handle_1, sql, callback, 0, &zErrMsg);
   if(rc){
      printf("Error inserting records into table CYBERSECURITY");
   } else {
      printf("Records created successfully for table CYBERSECURITY\n");
   }
   sqlite3_close(db_handle_1);
}

/* Function declaration for function that inserts data into COMPUTERSCIENCE table */

void insert_data_computer_science(){
   sqlite3 *db_handle_2;
   char *zErrMsg = 0;
   int rc2;
   char *sql_2;
   rc2 = sqlite3_open("computer_science_classes.db", &db_handle_2);

   sql_2 =
         "INSERT INTO COMPUTERSCIENCE (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (1, 'Database Management Systems', 'ICS', '311', 4 ); "
         "INSERT INTO COMPUTERSCIENCE (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (2, 'Algorithms and Data Structures', 'ICS', '340', 4 ); "
         "INSERT INTO COMPUTERSCIENCE (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (3, 'Organization of Programming Languages', 'ICS', '365', 4 ); "
         "INSERT INTO COMPUTERSCIENCE (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (4, 'Object-Oriented Design and Implementation', 'ICS', '372', 4 ); "
         "INSERT INTO COMPUTERSCIENCE (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (5, 'Parallel and Distributed Algorithms', 'ICS', '440', 4 ); "
         "INSERT INTO COMPUTERSCIENCE (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (6, 'Networks and Security', 'ICS', '460', 4 ); "
         "INSERT INTO COMPUTERSCIENCE (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (7, 'Operating Systems', 'ICS', '462', 4 ); "
         "INSERT INTO COMPUTERSCIENCE (ID, COURSENAME, DEPARTMENT, COURSENUMBER, CREDITS) "
         "VALUES (8, 'Software Engineering and Capstone Project', 'ICS', '499', 4 ); ";

   rc2 = sqlite3_exec(db_handle_2, sql_2, callback, 0, &zErrMsg);
   if(rc2){
      printf("Error inserting records into table COMPUTERSCIENCE\n");
   } else {
      printf("Records created successfully for table COMPUTERSCIENCE\n");
   }
   sqlite3_close(db_handle_2);
}

/* Function declaration for function that prints records from CYBERSECURITY */

void print_records_cybersecurity(){
   sqlite3 *db_handle_1;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called";
   rc = sqlite3_open("cybersecurity_classes.db", &db_handle_1);

   sql = "SELECT * from CYBERSECURITY";

   rc = sqlite3_exec(db_handle_1, sql, callback, (void*)data, &zErrMsg);
   sqlite3_close(db_handle_1);
}

/* Function declaration for function that prints records from COMPUTERSCIENCE */

void print_records_computer_science(){
   sqlite3 *db_handle_2;
   char *zErrMsg = 0;
   int rc2;
   char *sql_2;
   const char* data = "Callback function called";
   rc2 = sqlite3_open("computer_science_classes.db", &db_handle_2);

   sql_2 = "SELECT * from COMPUTERSCIENCE";

   rc2 = sqlite3_exec(db_handle_2, sql_2, callback, (void*)data, &zErrMsg);
   sqlite3_close(db_handle_2);
}

/* Function declaration to update incorrect record in CYBERSECURITY table */

void update_records(){
sqlite3 *db_handle_1;
char *zErrMsg = 0;
int rc;
char *sql;
const char* data = "Callback function called";
rc = sqlite3_open("cybersecurity_classes.db", &db_handle_1);

sql = "UPDATE CYBERSECURITY SET DEPARTMENT = 'CFS' WHERE ID = 1;"
      "UPDATE CYBERSECURITY SET CREDITS = '4' WHERE ID = 7;";

rc = sqlite3_exec(db_handle_1, sql, callback, (void*)data, &zErrMsg);
sqlite3_close(db_handle_1);
}

void print_updated_entries(){
   sqlite3 *db_handle_1;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called";
   rc = sqlite3_open("cybersecurity_classes.db", &db_handle_1);

   sql = "SELECT * from CYBERSECURITY WHERE ID = 1;"
         "SELECT * from CYBERSECURITY WHERE ID = 7;";

   rc = sqlite3_exec(db_handle_1, sql, callback, (void*)data, &zErrMsg);
   sqlite3_close(db_handle_1);
}

/* Function declaration for selecting entries in CYBERSECURITY table where */
/* Department equals ICS                                                   */

void print_ics_records_cyber(){
   sqlite3 *db_handle_1;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called";
   rc = sqlite3_open("cybersecurity_classes.db", &db_handle_1);

   sql = "SELECT * FROM CYBERSECURITY WHERE DEPARTMENT = 'ICS';";

   rc = sqlite3_exec(db_handle_1, sql, callback, (void*)data, &zErrMsg);
   sqlite3_close(db_handle_1);
}

/* Function declaration for selecting entries in COMPUTERSCIENCE table where */
/* Department equals ICS                                                     */

void print_ics_records_cs(){
   sqlite3 *db_handle_2;
   char *zErrMsg = 0;
   int rc2;
   char *sql_2;
   const char* data = "Callback function called";
   rc2 = sqlite3_open("computer_science_classes.db", &db_handle_2);

   sql_2 = "SELECT * FROM COMPUTERSCIENCE WHERE DEPARTMENT = 'ICS';";

   rc2 = sqlite3_exec(db_handle_2, sql_2, callback, (void*)data, &zErrMsg);
   sqlite3_close(db_handle_2);
}
