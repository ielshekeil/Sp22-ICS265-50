#include <stdio.h>
#include "sqlite3.h"
#include <string.h>

void search(sqlite3 *db);
void insert(sqlite3 *db);
void updateOneColumn(sqlite3 *db, int);
void updateRow(sqlite3 *db, int);
void print(sqlite3 *db);
void deleteRow(sqlite3 *db, int);

int main(void){
    int returnValue;        //used to return value from sqlite functions
    int recordnumber;       //used to identify which record will be updated
    int input;              //used for user input;
    int selection;          //used when more than one input is required for an option
    int openTest;           //used for the result of opening the database to make sure it is opened properly
    char *errorMsg=0;       //used as return value for error message in sqlite3_exec command
    char *sqlraw;           //used for unprepared SQL statements
    sqlite3_stmt *command;  //used for prepared SQL statements
    sqlite3 *ab;            //The instance of the database used

    /*Open the database file and test to make sure it opened properly*/
    openTest = sqlite3_open("addressBook.db", &ab); //Opens the database, or creates one if 
                                                    //the database does not exist on the system.

    if( openTest !=0 ) {
        printf("Error opening database\n");
        return 0;
    }

    /*get input from the user whether they are creating a new table or using an existing table*/
    printf("Type 1 if you need to make a new table or 2 if you are using an existing table:");
    scanf("%d", &input);
    if(input==1){
        sqlraw = "DROP TABLE ADDRESSES;";
        if(sqlite3_prepare_v2(ab, sqlraw, -1, &command, 0)!= SQLITE_OK) printf("could not prepare\n");

        sqlraw = "CREATE TABLE ADDRESSES(ID INTEGER PRIMARY KEY, FIRSTNAME TEXT ,LASTNAME TEXT, ADDRESS TEXT, EMAIL TEXT, PHONE TEXT);";
        if(sqlite3_prepare_v2(ab, sqlraw, -1, &command, 0)!= SQLITE_OK) printf("could not prepare\n");
        sqlite3_step(command);
    }

    /*Loop for main user interaction, and a switch statement to flow to the correct function.*/
    printf("1 to search the databse, 2 to insert a row, 3 to update a row, 4 to delete a row, " \
    "5 to print the whole database, or -1 to quit: ");
    scanf("%d", &input);
    while (input != -1){
        switch (input){
        case 1:
            search(ab);
            break;
        case 2:
            insert(ab);
            break;
        case 3:
            printf("Enter record number of the record you wish to update: ");
            scanf("%d", &recordnumber);
            printf("1 to update one field or 2 to update the whole record: ");
            scanf("%d", &selection);
            if(selection==1) updateOneColumn(ab, recordnumber);
            else if(selection == 2) updateRow(ab, recordnumber);
            else printf("Invalid selection\n");
            break;
        case 4:
            printf("Enter the record number of the record you wish to delete: ");
            scanf("%d", &recordnumber);
            deleteRow(ab, recordnumber);
            break;
        case 5:
            print(ab);
            break;
        }
        printf("1 to search the databse, 2 to insert a row, 3 to update a row, 5 to print the whole database, or -1 to quit: ");
        scanf("%d", &input);
    }

    sqlite3_close(ab); //close the sqlite file.
}
/*This function searches the database. First it asks what parameter the user would like to search by, and then asks for the
    search terms and searches the database.
    Input parameter is a pointer do a database file
    There is no output.*/
void search(sqlite3 *db){
    int selection;
    char input[100];
    sqlite3_stmt *command;

    printf("What field would you like to search by?\n\t1: First Name\n\t2: Last Name\n\t3:" \
            " Address\n\t4: Email\n\t5: Phone Number\n\tSelection:");
    scanf("%d", &selection);
    getchar();
    printf("Enter search term: ");
    gets(input);
    printf("%s %d\n", input, strlen(input));

    switch (selection){
    case 1:
        if(sqlite3_prepare_v2(db, "SELECT * FROM ADDRESSES WHERE FIRSTNAME = ?;", -1, &command, 0) != SQLITE_OK) printf("could not prepare\n");
        printf("%d\n", sqlite3_bind_parameter_count(command));
        if(sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC) != SQLITE_OK) printf("\ncould not bind\n");
        break;
    case 2:
        sqlite3_prepare_v2(db, "SELECT * FROM ADDRESSES WHERE LASTNAME = ?;", -1, &command, 0);
        if(sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC) != SQLITE_OK) printf("\ncould not bind\n");
        break;
    case 3:
       sqlite3_prepare_v2(db, "SELECT * FROM ADDRESSES WHERE ADDRESS = ?;", -1, &command, 0);
        sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC);
        break;
    case 4:
        sqlite3_prepare_v2(db, "SELECT * FROM ADDRESSES WHERE EMAIL = ?;", -1, &command, 0);
        sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC);
        break;
    case 5:
        sqlite3_prepare_v2(db, "SELECT * FROM ADDRESSES WHERE PHONE = ?;", -1, &command, 0);
        sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC);
        break;
    default:
        printf("Invalid selection.\n");
        break;
    }
    while(sqlite3_step(command) == SQLITE_ROW){
        printf("%d: %s %s\nAddress: %s\nEmail: %s\nPhone Number: %s\n\n", sqlite3_column_int(command, 0), 
            sqlite3_column_text(command, 1), sqlite3_column_text(command, 2), 
            sqlite3_column_text(command, 3), sqlite3_column_text(command, 4), sqlite3_column_text(command, 5));

    }
}


/*This function updates only one column in a row. Similarly to the search function it asks for what parameter the user would
    like to upddate and then takes input for the updated information.
    Input parameters are a pointer to a database file, and the record number (ID) of the record to be updated.*/
void updateOneColumn(sqlite3 *db, int recnum){
    int selection;
    char input[100];
    
    sqlite3_stmt *command;
    printf("What field would you like to search by?\n\t1: First Name\n\t2: Last Name\n\t3: Address\n\t4: Email\n\t5: Phone Number\n\tSelection: ");
    scanf("%d", &selection);
    getchar();
    switch (selection){
        case 1:
            sqlite3_prepare_v2(db, "UPDATE ADDRESSES SET ? = ? WHERE ID = ?", -1, &command, 0);
            sqlite3_bind_int(command, 2, recnum);
            printf("What would you like to update first name to: ");
            gets(input);
            sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC);
            break;
        case 2:
            sqlite3_prepare_v2(db, "UPDATE ADDRESSES SET LASTNAME = ? WHERE ID = ?", -1, &command, 0);
            sqlite3_bind_int(command, 2, recnum);
            printf("What would you like to update last name to: ");
            gets(input);
            sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC);
            break;
        case 3:
            sqlite3_prepare_v2(db, "UPDATE ADDRESSES SET ADDRESS = ? WHERE ID = ?", -1, &command, 0);
            sqlite3_bind_int(command, 2, recnum);
            printf("What would you like to update address to: ");
            gets(input);
            sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC);
            break;
        case 4:
            sqlite3_prepare_v2(db, "UPDATE ADDRESSES SET EMAIL = ? WHERE ID = ?", -1, &command, 0);
            sqlite3_bind_int(command, 2, recnum);
            printf("What would you like to update email to: ");
            gets(input);
            sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC);
            break;
        case 5:
            sqlite3_prepare_v2(db, "UPDATE ADDRESSES SET PHONE = ? WHERE ID = ?", -1, &command, 0);
            sqlite3_bind_int(command, 2, recnum);
            printf("What would you like to update phone number to: ");
            gets(input);
            sqlite3_bind_text(command, 1, input, strlen(input), SQLITE_STATIC);
            break;
    }
    sqlite3_step(command);
}


/*This function updates every field in a row. It asks for updated information for every parameter in the row and updates
    accordingly.
    Input parameterse are a pointer do a database file, and the record number to be updated*/
void updateRow(sqlite3*db, int recnum){
    char* sqlraw;
    sqlite3_stmt *command;
    char fname[20];
    char lname[20];
    char address[100];
    char email[100];
    char phone[15];

    sqlraw = "UPDATE ADDRESSES SET FIRSTNAME=?, LASTNAME=?, ADDRESS=?, EMAIL=?, PHONE=? WHERE ID=?;";
    sqlite3_prepare_v2(db, sqlraw, -1, &command, 0);
    sqlite3_bind_int(command, 6, recnum);

    getchar();

    printf("Enter first name or NULL: ");
    getchar();
    gets(fname);
    sqlite3_bind_text(command, 1, fname, strlen(fname), SQLITE_STATIC);
    printf("Enter last name or NULL: ");
    gets(lname);
    sqlite3_bind_text(command, 2, lname, strlen(lname), SQLITE_STATIC);
    printf("Enter address or NULL: ");
    gets(address);
    sqlite3_bind_text(command, 3, address, strlen(address), SQLITE_STATIC);
    printf("Enter email or NULL: ");
    gets(email);
    sqlite3_bind_text(command, 4, email, strlen(email), SQLITE_STATIC);
    printf("Enter phone number or NULL: ");
    gets(phone);
    sqlite3_bind_text(command, 5, phone, strlen(phone), SQLITE_STATIC);

    sqlite3_step(command);
}


/*This funciton inserts a new row at the bottom of the table. It asks for inputs for all the fields, any can be left blank, and inserts
    a new row to the bottom of the table.
    Input parameter is a pointer to a database file*/
void insert(sqlite3 *db){
    char* sqlraw;
    sqlite3_stmt *command;
    char fname[20];
    char lname[20];
    char address[100];
    char email[100];
    char phone[15];
    sqlraw = "INSERT INTO ADDRESSES(FIRSTNAME, LASTNAME, ADDRESS, EMAIL, PHONE) VALUES(?, ?, ?, ?, ?);";
    if(sqlite3_prepare_v2(db, sqlraw, -1, &command, 0)!= SQLITE_OK) printf("could not prepare\n");
    printf("Enter first name or NULL: ");
    getchar();
    gets(fname);
    sqlite3_bind_text(command, 1, fname, strlen(fname), SQLITE_STATIC);
    printf("Enter last name or NULL: ");
    gets(lname);
    sqlite3_bind_text(command, 2, lname, strlen(lname), SQLITE_STATIC);
    printf("Enter address or NULL: ");
    gets(address);
    sqlite3_bind_text(command, 3, address, strlen(address), SQLITE_STATIC);
    printf("Enter email or NULL: ");
    gets(email);
    sqlite3_bind_text(command, 4, email, strlen(email), SQLITE_STATIC);
    printf("Enter phone number or NULL: ");
    gets(phone);
    sqlite3_bind_text(command, 5, phone, strlen(phone), SQLITE_STATIC);

    sqlite3_step(command);
}


/*This function prints out the whole database in the format
        ID: FIRSTNAME LASTNAME
        Address: ADDRESS
        Email: EMAIL
        Phone Number: PHONE
    where the labels in all caps are what is stored in that column for the record.
    Input parameter is the pointer to the database file.*/
void print(sqlite3 *db){
    sqlite3_stmt *command;
    sqlite3_prepare_v2(db, "SELECT * FROM ADDRESSES;", -1, &command, 0);
    while(sqlite3_step(command)==SQLITE_ROW){
        printf("%d: %s %s\nAddress: %s\nEmail: %s\nPhone Number: %s\n\n", sqlite3_column_int(command, 0), 
        sqlite3_column_text(command, 1), sqlite3_column_text(command, 2), sqlite3_column_text(command, 3),
        sqlite3_column_text(command, 4), sqlite3_column_text(command, 5));
    }
}


/*This function deletes the row indicated by the recnum parameter.
    Input parameters are the pointer to the database file and the record number to be deleted.*/
void deleteRow(sqlite3 *db, int recnum){
    sqlite3_stmt *command;
    if(sqlite3_prepare_v2(db, "DELETE FROM ADDRESSES WHERE ID = ?", -1, &command, 0)!=SQLITE_OK) printf("could not prepare)");
    if(sqlite3_bind_int(command, 1, recnum)!=SQLITE_OK) printf("could not bind");
    sqlite3_step(command);
}