// *********************************************************************
// 
// Source File: a6_start.cpp
// 
// Purpose:
//  This program is a simplified, interactive inventory database
//  for a book warehouse.
//  
// Details:
//
// Entries in the directory have the following information:
//
//  author_last_name	a C-string of maximum length 12 characters
//
//  author_initial	a character
//
//  inventory number    an integer
//                      a unique identification number for this book
//
//  location		a C-string of 4 characters
//				(example: Shelf h, bin 03 would be h-03)
//  title               a C-string of maximum length 20 characters.
//
//  comment             a C-string of up to 24 characters
//
//  num			an integer
//          		this represents the number of that particular
//			book on hand. It may be negative, indicating
//			a back-order. A value of 0 indicates that
//			there are no available copies of that book.
//				
// This data is stored in an array of records in alphabetic order based
// on the author's last name field, which is called the key field.
// The data is read from a user-specified file at the start of the program
// and is written to a user-specified file as the program terminates.
// The program is menu-driven and the user is allowed to work with the
// book directory until they choose to terminate the program. The 
// selection of a valid choice results in the corresponding processing;
// whereas selection of an invalid choice results in an appropriate error
// message.
//
// The allowable operations on the inventory database are:
//
//    LIST ALL  	- displays all inventory entries
//    LIST BY NAME	- displays the inventory entries for all books
//			  that match author's last name
//    REMOVE		- allows the user to delete an existing entry in the
//                        inventory based on a book id.
//    QUIT    	        - to exit the program
//
// *********************************************************************

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
using namespace std;

const char EOLN          = '\n';  // end of line character

const int MAX_RECORDS     = 50;   // string lengths
const int MAX_AUTHOR_NAME = 12;
const int MAX_LOCATION    = 4;
const int MAX_TITLE       = 20;
const int MAX_COMMENT     = 24;


typedef char AUTHOR_STRING[MAX_AUTHOR_NAME+1];
typedef char LOCATION_STRING[MAX_LOCATION+1];
typedef char TITLE_STRING[MAX_TITLE+1];
typedef char COMMENT_STRING[MAX_COMMENT+1];

struct Entry
{
   AUTHOR_STRING   author_name;
   char            author_initial;
   int             inventory_number;
   LOCATION_STRING location;
   TITLE_STRING    title;
   COMMENT_STRING  comment;
   int             quantity;
};

void readfile (Entry [], int&, bool&);
                                  // reads the inventory database in from the
                                  // master file into an array
void process_menu (char&);        // display the menu and read user's choice
void list_all (const Entry [], int);    // print all entries in the database
void list_by_name (const Entry [], int);// find and display the entry for anyone
                                  // matching a specified author_name or name
				  // portion

void remove (Entry [], int&);     // find and remove a specifed book based on
                                  // inventory id
              
void writefile (Entry book[], int);
                                  // writes the entire inventory out to a
                                  // file specified by the user

void write_entry(const Entry [], int index); 	  // display a single record 
                                                  //from database 



int main ()
{
   Entry inventory[MAX_RECORDS];  // the array for the database
   int no_entries;                // number of entries in the database
   char choice;                   // menu selection
   bool success;                  // reading data success flag

   readfile (inventory, no_entries, success);

   if (!success)
   {
     cout << "unable to open inventory file -- program terminating " << endl;
   }
   else
   {
       process_menu (choice);
       while (choice != '4')
       {
           switch (choice)
           {
             case '1' : list_all (inventory, no_entries);
                        break;
             case '2' : list_by_name (inventory, no_entries);
                        break;
             case '3' : remove (inventory, no_entries);
                        break;
             default  : cout << "Illegal menu choice--try again" << endl;
                        break;
           }
           process_menu (choice);
       }
       writefile (inventory, no_entries);
   }

   return 0;
}


//*********************************************************************
// Function:  readfile
// Purpose:   Loads a inventory database from a specified file into an
//            array.
// Details:
//            The name of the input file is read from the user. If
//            the file is successfully opened the array is loaded, the
//            number of entries is counted and a success flag is set
//            to true. If the file can't be opened the success flag is
//            set to false.
//
//            Reading of string data is done using the getline operation,
//            which allows the reading of a maximum length or eoln which
//            ever is encountered first. The numeric fields are read
//            using "cin >>" which terminates upon encountering the newline
//            character and leaves the eoln in the input stream. When a 
//            string read immediately follows an integer read the eoln is
//            left in the input stream. The eoln MUST be removed from the
//            input stream before getline is called; otherwise getline 
//            will read an empty line. This is the reason for the two 
//            reads involving the junk variable.
//
// Assumptions:
//            It is assumed that the file contains complete entries
//            and that the data in the file is in alphabetical order.
// Inputs:    none
// Outputs:   inventory - the loaded inventory database
//            no_entries - the number of entries that are loaded
//            success - whether or not the array was successfully loaded
//
//*********************************************************************

void readfile (Entry inventory [], int &no_entries, bool &success)
{
   const int FILE_LENGTH = 100;
   ifstream inp;
   char filename[FILE_LENGTH];
   char junk;

   success = false;
   cout << "Enter the name of the inventory file: ";
   cin >> filename;
   inp.open (filename);
   if (!inp.fail ())
   {
       success = true;
       no_entries = 0;
       inp.getline (inventory[no_entries].author_name, MAX_AUTHOR_NAME+1);
       while (!inp.eof () && no_entries < MAX_RECORDS)
       {
           inp.get (inventory[no_entries].author_initial);
           inp >> inventory[no_entries].inventory_number;
           inp.get (junk);   // needed to eliminate the unread newline
           inp.getline (inventory[no_entries].location, MAX_LOCATION+1);
           inp.getline (inventory[no_entries].title, MAX_TITLE+1);
           inp.getline (inventory[no_entries].comment, MAX_COMMENT+1);
           inp >> inventory[no_entries].quantity;
           inp.get (junk);  // needed to eliminate the unread newline
           no_entries++;
           inp.getline (inventory[no_entries].author_name, MAX_AUTHOR_NAME+1);
       }
       inp.close ();
   }
   return;
}

//*********************************************************************
// Function:  process_menu
// Purpose:   displays a menu listing the allowable operation choices,
//            prompts for user input and reads input until the end of
//            line is encountered.
// Details:   Returns the first non-whitespace character encountered.
//            Any non-whitespace between the menu choice and the 
//            end of line is eliminated.
// Inputs:    none
// Outputs:   choice - the first non-whitespace character
//*********************************************************************

void process_menu (char &choice)
{
  char junk;

   cout << "********************************************************" << endl
        << "*                                                      *" << endl
        << "*               1631 Book Inventory DB                 *" << endl
        << "*                                                      *" << endl
        << "*    1 - list ALL entries                              *" << endl
        << "*    2 - list all entries matching author_name portion *" << endl
        << "*    3 - remove an entry by inventory number           *" << endl
        << "*    4 - to exit the program                           *" << endl
        << "*                                                      *" << endl
        << "********************************************************" << endl
        << endl
        << "Enter menu choice:  ";

   cin >> choice;
   do
   {
     cin.get (junk);
   } while (junk != EOLN);

   return;
}

//*********************************************************************
// Function:  list_all
// Purpose:   list all the entries in the inventory
//
// Details:   loops write_entry for each value in the inventory array
//            for how many entries there are based on no_entries
// Inputs:    inventory - the database array
//            no_entries - the number of entries 
//
//*********************************************************************

void list_all (const Entry inventory[], int no_entries)
{
    int loop = 0, count = 1;
    while(loop < no_entries)
    {
	cout << "# " << count << endl;
	write_entry(inventory, loop);
	count++;
	loop++;
    }
    return;
}

//*********************************************************************
// Function:  list_by_name
// Purpose:   to find any entries with the matching last name the user
//            is looking for and print them out
// Details:   compares the entered last name with the last name of every
//            entry until a successful comparison is found. Loop also ends
//            if the value of strcmp exceeds 0, which indicates that the
//            value is higher than the inputted last name
// Inputs:    inventory - the database array
//            no_entries - the number of entries 
//
//*********************************************************************

void list_by_name (const Entry inventory[], int no_entries)
{
    char lastName[MAX_AUTHOR_NAME];
    int loop = 0, count = 1;
    int compare;
    bool found = false;
    cout << "Please enter the last name of the author" <<
             "you wish to search for : ";
    cin >> lastName;
    
    while(loop < no_entries)
    {
	compare = strncmp(inventory[loop].author_name, lastName, 
			  strlen(lastName));
	if(compare == 0)
	{
	    cout << "# " << count << endl;
	    write_entry(inventory, loop);
	    count++;
	    found = true;
	}
	else if(compare > 0)
	{
	    loop = no_entries;
	}
	loop++;
    }

    if(!found)
    {
	cout << endl << lastName << " not found in inventory." << endl;
    }

    return;
}

//*********************************************************************
// Function:  remove
// Purpose:   to remove entries the user wishes to remove
//
// Details:   loops until the inventory number inputted is found 
//            in the entry array. If found, the user confirms if it is
//            the correct entry to be deleted. It then shifts down all
//            the values above the selected entry which is overwritten.
// Inputs:    inventory - the database array
//            no_entries - the number of entries 
// Outputs:   inventory - with a single entry deleted
//            no_entries - altered if an entry is deleted
//
//*********************************************************************

void remove (Entry inventory[], int &no_entries)
{
    char confirm;
    int invNum;
    int loop = 0;
    bool found = false;
    cout << "Enter the inventory number of the book" <<
            "record you wish to remove: ";
    cin >> invNum;
    
    while(loop < no_entries)
    {
	if(invNum == inventory[loop].inventory_number)
	{
	    write_entry(inventory, loop);
	    cout << endl << "Are you sure you wish to delete " <<
                             "this record? (y/n) ";
	    cin >> confirm;
	    if(confirm == 'y')
	    {
		while(loop < no_entries)
		{
		    inventory[loop] = inventory[loop + 1];
		    loop++;
		}
		cout << endl << "Record Deleted" << endl;
		no_entries--;
	    }
	    else
	    {
		cout << endl << "Record NOT Deleted" << endl;
	    }
	    found = true;
	    loop = no_entries;
	}
	loop++;
    }

    if(!found)
    {
	cout << endl << "Record " << invNum << " not found. " << endl;
    }
    return;
}

//*********************************************************************
// Function:  writefile
// Purpose:   to save the inventory to a file.
//
// Details:   The file name will be read from the user.
//            Each entry field is written to a separate line of 
//            the file.
//
// Inputs:    inventory - the database array
//            no_entries - the number of entries
//
//*********************************************************************

void writefile (Entry inventory[], int no_entries)
{
  ofstream outp;
  int i;
  AUTHOR_STRING filename;
 
  cout << "Enter the name of the inventory file: ";
  cin >> filename;
  outp.open (filename);
  if (outp.fail ())
  {
    cout << "Unsuccessful trying to open file " << filename << endl;
  }
  else
  {
    for (i = 0; i < no_entries; i++)
    {
      outp << inventory[i].author_name << EOLN;
      outp << inventory[i].author_initial << EOLN;
      outp << inventory[i].inventory_number << EOLN;
      outp << inventory[i].location << EOLN;
      outp << inventory[i].title << EOLN;
      outp << inventory[i].comment << EOLN;
      outp << inventory[i].quantity << EOLN;
    }

    outp.close ();
  }    
  return;
}

//*********************************************************************
// Function:  writefile
// Purpose:   to output a single entry
//
// Details:   The inputted index will be used to find the values held
//            in the entry array. Each value will be outputted to the user
//
// Inputs:    inventory - the database array
//            no_entries - the number of entries
//
//*********************************************************************

void write_entry(const Entry inventory[], int index)
{
    cout << left;
    cout << setw(20) << "Author Last Name" << inventory[index].author_name 
	 << endl;
    cout << setw(20) << "Author Initial" << inventory[index].author_initial 
	 << endl;
    cout << setw(20) << "Inventory Number" << inventory[index].inventory_number 
	 << endl;
    cout << setw(20) << "Location" << inventory[index].location << endl;
    cout << setw(20) << "Book Title" << inventory[index].title << endl;
    cout << setw(20) << "Comments" << inventory[index].comment << endl;
    cout << setw(20) << "Quantity" << inventory[index].quantity << endl;
}
