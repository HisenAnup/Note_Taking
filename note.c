#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

struct login {
	char name[20];
	char password[20];
} user_input,check;

struct system_data {
	char date[20];
	char title[50];
	char notes[1024];
} fetch_from_file,manual_input;


const char key = 'K';											// encrypt and decrypt key.


void field_position(char buffer[], int array[], char delim);	// checks at what posion '|' exist which separate each field
void file_data(char buffer[], int array[]); 					// take the data from login file and stores it in a variable.
void check_pointer(FILE *fp); 									// check if the file pointer is NULL
void xor_encrypt(char value[]);
void xor_decrypt(char value[]);
void note_data(char buffer[], int array[]);  					// take the data from note file and stores it in a variable.
void input_flush(); 											// flush leftover input
void check_empty_file(FILE *fp); 								// check if the file is empty

void welcome_message();											// welcome message function
void end_message();												// end message function
void login_system(FILE *file, int *authentication);				// login system function
void registration_system(FILE *fp, int *authentication);							// registration system function
void display_select();											// display the select menu
void view_all(FILE *fp);										// view all notes function
void add_note(FILE *fp, char filename[]);						// add note function
void edit_note(FILE *fp, char filename[]);						// edit note function
void delete_note(FILE *fp, char filename[]);					// delete note function
void search_note(FILE *fp, char filename[]);					// search note function


int main() {


	int mode;
	FILE *database;
	FILE *new_file;
	system("cls");

	up:															// label to go back to welcome message
			welcome_message();									// display welcome message
			scanf("%d",&mode);
			input_flush();										// flush leftover input

	switch(mode) {
	case 1:
		database = fopen("login_system.txt","r");
		if(database == NULL) {
			printf("\n ===================== No registration has been made yet ==========================");
			printf("\n =========================== Please register first ================================\n");
			printf("\n						Press any key to return to menu \n");
			getch();
			system("cls");
			goto up; 											// go back to welcome message
		}
		int authentication = 0;
		login_system(database, &authentication);
		fclose(database);
		system("cls");
		if(authentication == 1) {
			goto up; 											// go back to welcome message if not authenticated
		}
		break;
	case 2:
		database = fopen("login_system.txt","a");
		if(database == NULL) {
			new_file = fopen("login_system.txt","w"); 			// create a new file if it does not exist
			check_pointer(new_file);
			fclose(new_file);									// close the file pointer

		}
		int authentication_2 = 0;
		registration_system(database, &authentication_2);
		system("cls");
		if(authentication_2 == 1){
			goto up; 											// go back to welcome message if not authenticated
		}
		break;
	case 3:
		end_message();
		system("cls");
		exit(0);
	default:
		printf("Invaled option, please reopen the application \n");
		printf("         Press any key to exit                \n");
		getch();
		exit(1);
		break;
	}


	display_select:
			system("cls");
			display_select();

	int select;
	printf("\n Select any option from the above: ");
	scanf("%d",&select);
	input_flush(); 															// flush leftover input

	char filename[20];														// to store the file name
	snprintf(filename, sizeof(filename), "note_%s.txt",user_input.name); 	// use the username as a file name.

	switch(select) {
	case 1:
		database = fopen(filename,"r");
		check_pointer(database);
		view_all(database);
		fclose(database);
		goto display_select;												
	case 2:
		database = fopen(filename,"a");
		check_pointer(database);
		add_note(database, filename);
		fclose(database);
		goto display_select; 												
		break;
	case 3:
		database = fopen(filename, "r");
		check_pointer(database);
		search_note(database, filename);
		fclose(database);
		goto display_select; 
		break;
	case 4:
		database = fopen(filename,"r");
		check_pointer(database);
		edit_note(database, filename);
		goto display_select; 
		break;
	case 5:
		database = fopen(filename,"r");
		check_pointer(database);
		delete_note(database, filename);
		goto display_select; 
		break;
	case 6:
		end_message();
		exit(0);
		break;
	default:
		printf("\n Invalid option selected \n Press any Key to continue again");
		getch();
		goto display_select;
		break;

	}

	system("cls");
	end_message();
	getch();
	return 0;
}


void search_note(FILE *fp, char filename[]) {
	system("cls");

	// Check if the file is empty
	char buffer[1000]; 												// buffer to read each line
	if (fgets(buffer, sizeof(buffer), fp) == NULL) {
		printf("\n ============================> The file is empty or reach the end of the file <==============================\n");
		printf("Press any key to return to main menu \n");
		getch(); 
		fclose(fp); 
		return;
	}

	rewind(fp); 														// reset file pointer to the beginning of the file


	int tracker = 0; 													// to track if any title matches
	printf("\n ========================= Search content from %s file ===========================\n",filename);
	printf("\n =======================All the existing title of this file=========================\n");
	char line[200]; 
	int field_pos[5]; 
	while(fgets(line, sizeof(line), fp)) {
			field_position(line, field_pos, '|');
			note_data(line, field_pos); 
			printf("%s\n",fetch_from_file.title); // print the title
	}
	printf("\n ===================================================================================\n");


	printf("\n Enter the title to search : ");
	fgets(manual_input.title, sizeof(manual_input.title), stdin); 		// get user input for title
	manual_input.title[strcspn(manual_input.title, "\n")] = '\0'; 		// remove newline character if present

	strlwr(manual_input.title); 										// convert to lowercase for case-insensitive comparison

	char local[1000]; 													// temp storage for each line data.
	int array[5]; 														// to store the position of each field in the line
	rewind(fp); 														// reset file pointer to the beginning of the file
	
	while(fgets(local, sizeof(local), fp)) {
		field_position(local,array,'|');  								// get the position of each field in the line
		note_data(local,array); 										// store the data in fetch_from_file struct

		if(strcmp(fetch_from_file.title, manual_input.title) == 0) {
			tracker++; 													// increment tracker if title matches
			xor_decrypt(fetch_from_file.notes); 						// decrypt the notes
			printf("\n Date: %s\n Title: %s\n content: %s\n",fetch_from_file.date, fetch_from_file.title, fetch_from_file.notes); // print the details
		}
	}
	if(tracker <= 0) {
		printf("\n ===============> The Following title does not exist <=================\n");
	}
		printf("\n ========================= End of Search ===========================\n");
	printf("Press any key to return to main menu \n");
	getch(); 															// wait for user input before returning to main menu
	system("cls");
}


void delete_note(FILE *fp, char filename[]) {
	system("cls");

	// Check if the file is empty
	char buffer[1000]; 												// buffer to read each line
	if (fgets(buffer, sizeof(buffer), fp) == NULL) {
		printf("\n ============================> The file is empty or reach the end of the file <==============================\n");
		printf("Press any key to return to main menu \n");
		getch(); 
		fclose(fp); 
		return;
	}
	rewind(fp); 													// reset file pointer to the beginning of the file


	printf("\n ============= deleting the content of %s file ==================\n",filename);
	int title_tracker = 0;											 // to track if any title matches
	char line[200]; 												// buffer to read each line
	int field_pos[5]; 												// to store the position of each field in the line
	printf("\n =========== All the existing title of this file =================\n");

	// display all the existing titles
	while(fgets(line, sizeof(line), fp)) {
			field_position(line, field_pos, '|');
			note_data(line, field_pos);
			printf("%s\n",fetch_from_file.title);

	}
	printf("\n ==================================================================\n");

	printf("\n Enter the Title you want to delete ==> ");
	fgets(manual_input.title, sizeof(manual_input.title), stdin);	// get user input for title
	manual_input.title[strcspn(manual_input.title, "\n")] = '\0'; 	// remove newline character if present
	strlwr(manual_input.title); 									// convert to lowercase for case-insensitive comparison

	// create a temporary file to store the remaining lines
	FILE *temp; 													// temporary file to store the remaining lines
	temp = fopen("temp.txt", "w");
	check_pointer(temp);

	
	rewind(fp); 													// reset file pointer to the beginning of the file
	while(fgets(line, sizeof(line), fp)) {

		field_position(line, field_pos, '|'); 						// get the position of each field in the line
		note_data(line, field_pos); 								// store the data in fetch_from_file struct

		if(strcmp(fetch_from_file.title, manual_input.title) != 0) {
			fputs(line, temp); 										// keep the line if title does not match
		}
		if(strcmp(fetch_from_file.title, manual_input.title) == 0 ) {
			title_tracker++;  										// increment tracker if title matches
		}
	}

	// display message based on whether the title was found and deleted
	if(title_tracker > 0) {
		printf("\n<================> The Title name %s content has successfully being deleted <=================>\n",manual_input.title);
	}else {
		printf("\n ======================> The Following title doesnot exist <===================================\n");
		}


	fclose(fp); 													// close the original file
	fclose(temp); 													// close the temporary file

	remove(filename);               								// delete old file
	rename("temp.txt", filename);   								// rename temp file to original

		printf("\n =============================== End of Deletion ===================================\n");
	printf("Press any key to return to main menu \n");
	getch();
	system("cls");
}


void edit_note(FILE *fp, char filename[]) {
	system("cls");
	// Check if the file is empty
	char buffer[1000]; // buffer to read each line
	if (fgets(buffer, sizeof(buffer), fp) == NULL) {
		printf("\n ============================> The file is empty or reach the end of the file <==============================\n");
		printf("Press any key to return to main menu \n");
		getch(); 
		fclose(fp); 
		return;
	}
	rewind(fp); // reset file pointer to the beginning of the file


	printf("\n ====================Editing the %s file ======================\n",filename);
	char line[1000];
	int field_pos[5];

	printf("\n ============All the existing title of this file================\n");
	while(fgets(line, sizeof(line), fp)) {

		field_position(line, field_pos, '|');
		note_data(line, field_pos);
		printf("%s\n",fetch_from_file.title);
	}
	printf("\n ==============================================================================\n");

	printf("\n Enter the Title you want to see its content ==> ");
	fgets(manual_input.title, sizeof(manual_input.title), stdin);
	manual_input.title[strcspn(manual_input.title, "\n")] = '\0'; // remove newline character if present

	strlwr(manual_input.title);

	// Check if the title exists
	int Title_tracker = 0;
	rewind(fp); // reset file pointer to the beginning of the file
	while(fgets(line,sizeof(line), fp)){
		field_position(line, field_pos, '|');
		note_data(line, field_pos);
		
		if(strcmp(fetch_from_file.title, manual_input.title) == 0) {
			Title_tracker++; // increment tracker if title matches
			}
	}

	if(Title_tracker <= 0) {
			printf("\n ============================> The Following title does not exist <==============================\n");
			printf("Press any key to return to main menu \n");
			getch();
			system("cls");
			return; // exit the function if title does not exist
	}

	// create a temporary file to store the updated lines
	FILE *temp;
	temp = fopen("temp.txt", "w");
	check_pointer(temp);

	rewind(fp);

	// read each line from the original file
	while(fgets(line, sizeof(line), fp)) {
			field_position(line, field_pos, '|');
			note_data(line, field_pos);

			if(strcmp(fetch_from_file.title, manual_input.title) != 0) {
						fputs(line, temp); // keep the line
			} else {
					printf("Enter The content ==> ");
					fgets(manual_input.notes, sizeof(manual_input.notes), stdin);
					manual_input.notes[strcspn(manual_input.notes, "\n")] = '\0';
					strlwr(manual_input.notes); // convert to lowercase for case-insensitive comparison

					// Get current time
					time_t now = time(NULL);
					struct tm *local = localtime(&now);

					// Format timestamp
					strftime(manual_input.date, sizeof(manual_input.date), "%Y-%m-%d", local);

					xor_encrypt(manual_input.notes);

					fseek(temp, 0, SEEK_END); // move the file pointer to the end of the file
					fprintf(temp,"|%s|%s|%s|\n",manual_input.date,manual_input.title,manual_input.notes);
					printf("\n<===================> The following content has successfully being edited <==========================>\n");
					printf("press any key to continue \n");
					getch(); // wait for user input
					system("cls");
			}

	}
	fclose(fp);
	fclose(temp);

	remove(filename);               // delete old file
	rename("temp.txt", filename);   // rename temp file to original
}


void registration_system(FILE *fp, int *authentication) {

	again:

	system("cls");
	printf("\n ===================== Registration System ========================== \n");
	printf(" ===================== Enter your registration details ========================== \n");
	printf("Enter your username:---> ");
	fgets(user_input.name, sizeof(user_input.name), stdin);
	user_input.name[strcspn(user_input.name, "\n")] = '\0'; // remove newline character if present

	printf("Enter your password:---> ");
	fgets(user_input.password, sizeof(user_input.password), stdin);
	user_input.password[strcspn(user_input.password, "\n")] = '\0'; // remove newline character if present


	char local[30]; 												// temp storage for each line data.
	check.name[0] = '\0'; 											// initialize to empty string
	check.password[0] = '\0'; 										// initialize to empty string

	// Check if the username and password are empty
	if(strlen(user_input.name) == 0 || strlen(user_input.password) == 0) {
		system("cls");
		printf("\n ===================== Please enter a valid username and password ========================== \n");
		printf("Press any key to continue \n");
		getch();
		fclose(fp);
		*authentication = 1; // set authentication to 1 to indicate failed registration
		return;
	}

	int array[3]; 													// to store the position of each field in the line
	FILE *file;
	file = fopen("login_system.txt","r");
	check_pointer(file);
	while(fgets(local,sizeof(local),file)) {

		field_position(local,array,'|');
		file_data(local,array);

		// Check if the username already exists
		for(int i= array[0] + 1; i < array[1]; i++) {
			if(strcmp(user_input.name,check.name) == 0) {
				system("cls");
				printf("\n ==============The username already exist ======= please use another username =========");
				printf("\n ======================= Press any key to continue ========================\n");
				getch();
				fclose(fp);
				*authentication = 1;
				return;
			}
		}
	}
	fclose(file);
	
	char filename[50];
	snprintf(filename, sizeof(filename), "note_%s.txt",user_input.name); // use the username as a file name.


	FILE *note_fp = fopen(filename, "w");	 							// create a new file for the user notes
	check_pointer(note_fp); 
	fclose(note_fp);

	fprintf(fp,"|%s|%s|\n",user_input.name,user_input.password);
	printf("\n =========================== Registration successful ========================== \n");
	printf("\n ===================== You can now login with your username ========================== \n");
	printf("Press any key to continue \n");
	getch(); 
	fclose(fp);
	system("cls");
}


void login_system(FILE *file, int *authentication) {


	char local[50]; // temp storage for each line data.
			system("cls");
			printf("\n ===================== Welcome to the Login System ========================== \n");
			printf("\n =================== Please enter your login details ========================== \n");

	// Get user input for username and password
	printf("Enter your username:---> ");
	fgets(user_input.name, sizeof(user_input.name), stdin);
	user_input.name[strcspn(user_input.name, "\n")] = '\0'; // remove newline character if present

	printf("Enter your password:---> ");
	fgets(user_input.password, sizeof(user_input.password), stdin);
	user_input.password[strcspn(user_input.password, "\n")] = '\0'; // remove newline character if present


	int tracker = 0;
	int check_array[3]; // to store the position of each field in the line
	check.name[0] = '\0'; // initialize to empty string
	check.password[0] = '\0'; // initialize to empty string


	rewind(file); // reset file pointer to the beginning of the file

	// Read the file line by line
	while(fgets(local, sizeof(local), file)) {
		field_position(local,check_array,'|');
		file_data(local,check_array);

		if(strcmp(user_input.name,check.name) == 0 && strcmp(user_input.password,check.password) == 0) {
			tracker++;
		}
	}
	if(tracker == 1) {
		system("cls");
		printf("\n ============================= Welcome %s ========================== \n",user_input.name);
		printf("You have successfully logged in \n");
		printf("Press any key to continue \n");
		getch();
		system("cls");
	} else {
		system("cls");
		printf("\n ===================== Invalid username or password ========================= \n");
		printf("Please check your username and password again \n");
		printf("Press any key to return to registration page and try again \n");
		getch();
		*authentication = 1; // set authentication to 1 to indicate failed login
	}
}


void field_position(char buffer[],int array[],char delim) {
	int x = 0, i = 0;
	while(buffer[i] != '\0') {

		if(buffer[i] == delim) {
			array[x++] = i;
		}
		i++;

	}
	// printf("\n value of i = %d \n", i); // For testing
}


void file_data(char buffer[],int array[]) {

	int x = 0, y = 0, i = 0;
	for(i = array[0] + 1; i < array[1]; i++) {
		check.name[x++] = buffer[i];
	}
	check.name[x] = '\0'; // null terminator

	for(i = array[1] + 1; i < array[2]; i++) {
		check.password[y++] = buffer[i];
	}
	check.password[y] = '\0';  //null terminator
}


void note_data(char buffer[],int array[]) {
	int x = 0, y = 0, z = 0, i = 0;

	for(i = array[0] + 1; i < array[1]; i++) {
		fetch_from_file.date[x++] = buffer[i];
	}
	fetch_from_file.date[x] = '\0'; // null terminator

	for(i = array[1] + 1; i < array[2]; i++) {
		fetch_from_file.title[y++] = buffer[i];
	}
	fetch_from_file.title[y] = '\0';  //null terminator

	for(i = array[2] + 1; i < array[3]; i++) {
		fetch_from_file.notes[z++] = buffer[i];
	}
	fetch_from_file.notes[z] = '\0';  //null terminator


}


void view_all(FILE *fp) {
	system("cls");

	// Check if the file is empty
	char buffer[1000]; // buffer to read each line
	if (fgets(buffer, sizeof(buffer), fp) == NULL) {
		printf("\n ============================> The file is empty or reach the end of the file <==============================\n");
		printf("Press any key to return to main menu \n");
		getch(); 
		fclose(fp); 
		return;
	}
	rewind(fp); // reset file pointer to the beginning of the file

		printf("\n ====================== Viewing All Notes ====================== \n");
		printf("\n ============== All the existing notes are listed below ==================== \n");
	char local[1000]; // temp storage for each line data.
	int position[5]; // to store the position of each field in the line
	rewind(fp); // reset file pointer to the beginning of the file

	while(fgets(local, sizeof(local), fp)) {
		field_position(local, position,'|');
		note_data(local,position);
		xor_decrypt(fetch_from_file.notes);
		printf("\n Date = %s\n Title: %s\n note: %s\n",fetch_from_file.date,fetch_from_file.title,fetch_from_file.notes);
		printf("---------------------------------------------------------------------------------\n");

	}
		printf("\n ======================== End of Notes ========================== \n");
	printf("Press any key to return to main menu \n");
	getch(); // wait for user input before returning to main menu
}


void add_note(FILE *fp, char filename[]) {

	system("cls");
		printf("\n ===================== Writing On %s File ==========================\n",filename);
		printf("\n ============== Enter the details of the note ======================\n");

	printf("\n Enter the Title of the note ==> ");
	fgets(manual_input.title, sizeof(manual_input.title), stdin);
	manual_input.title[strcspn(manual_input.title, "\n")] = '\0';
	strlwr(manual_input.title);

	// Check if the title is empty
	if(strlen(manual_input.title) == 0) {
		printf("\n=================> The title cannot be empty <=======================\n");
		printf("Press any key to return to main menu \n");
		getch(); // wait for user input before returning to main menu
		return; // exit the function if title is empty
	}

	// Check if the title already exists
	char line[1000];
	int field_pos[5];
	while(fgets(line, sizeof(line), fp)) {
		field_position(line, field_pos, '|');
		note_data(line, field_pos);

		if(strcmp(fetch_from_file.title, manual_input.title) == 0) {
			printf("\n ==============> The title already exists <==========================\n");
			printf("Press any key to return to main menu \n");
			getch(); // wait for user input before returning to main menu
			return; // exit the function if title already exists
		}

	}
	printf("\n write the content ==> ");
	fgets(manual_input.notes, sizeof(manual_input.notes), stdin);
	manual_input.notes[strcspn(manual_input.notes, "\n")] = '\0';
	strlwr(manual_input.notes); // convert to lowercase for case-insensitive comparison

	// Check if the notes is empty
	if(strlen(manual_input.notes) == 0) {
		printf("\n ===============> The content cannot be empty <===========================\n");
		printf("Press any key to return to main menu \n");
		getch(); // wait for user input before returning to main menu.
		return; // exit the function if notes is empty
	}

	// Get current time
	time_t now = time(NULL);
	struct tm *local = localtime(&now);

	// Format timestamp
	strftime(manual_input.date, sizeof(manual_input.date), "%Y-%m-%d", local);

	xor_encrypt(manual_input.notes);
	fprintf(fp,"|%s|%s|%s|\n",manual_input.date,manual_input.title,manual_input.notes);
	printf("\n<======> The following content has successfully being added <============>\n");
	printf("Press any key to return to main menu \n");
	getch(); // wait for user input before returning to main menu
}


void xor_encrypt(char value[]) {
	for(int i = 0; value[i] != '\0'; i++) {
		value[i] ^= key;
	}
}


void xor_decrypt(char value[]) {
	for(int i = 0; value[i] != '\0'; i++) {
		value[i] ^= key;
	}
}


void input_flush() {
	int ch,n;
	  // Flush the input buffer
	while ((ch = getchar()) != '\n' && ch != EOF);
   // No action is needed here

}


void display_select() {
	 printf("\n");
    printf("=====================================================================================\n");
    printf("                  	 Encrypted Notes & Task Manager System 	                       \n");
    printf("=====================================================================================\n");
    printf("| %-3s | %-60s |\n", "1", "View All Notes");
    printf("| %-3s | %-60s |\n", "2", "Add Notes");
    printf("| %-3s | %-60s |\n", "3", "Search Notes");
    printf("| %-3s | %-60s |\n", "4", "Edit Notes");
    printf("| %-3s | %-60s |\n", "5", "Delete Notes");
    printf("| %-3s | %-60s |\n", "6", "Exit");
    printf("=====================================================================================\n\n");
}


void welcome_message() {
	printf("\n");
    printf("=====================================================================================\n");
    printf("             		 Welcome to the Encrypted Notes & Task Manager System              		\n");
    printf("=====================================================================================\n");
    printf("| %-3s | %-60s |\n", "1", "Login");
    printf("| %-3s | %-60s |\n", "2", "New Registration");
    printf("| %-3s | %-60s |\n", "3", "Exit");
    printf("=====================================================================================\n");
    printf(">> Please select your mode of operation: ");
}


void end_message() {
	system("cls");
	printf("\n");
	printf("=====================================================================================\n");
	printf("                  Thank you for using the Encrypted Notes System                     \n");
	printf("=====================================================================================\n");
	printf("                Your data is safe. Have a productive and secure day                  \n");
	printf("=====================================================================================\n\n");
	printf("                         Press any key to exit                                       \n");
	getch();
	system("cls");
}


void check_pointer(FILE *fp) {
	if(fp == NULL) {
		printf("\n==================== Error while opening the file ============================= \n");
		printf("\n================== Please check if the file exists =============================\n");
		printf("Exiting the program...\n");
		printf("Press any key to exit \n");
		getch(); // wait for user input before exiting
		exit(1);
	}

}