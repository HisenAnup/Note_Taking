# Encrypted Notes & Task Manager System

A simple, console-based (CLI) application written in **C** that allows users to create accounts, log in, and securely store their personal notes. 

This project was built to demonstrate how to manage user data, handle file input/output (I/O) operations, and apply basic encryption to keep text files secure. It's a great reference for anyone learning C programming who wants to see how a complete, interactive terminal application is built from scratch.

## Features

* **User Authentication:** Secure registration and login system. Each user gets their own isolated note file (e.g., `note_username.txt`).
* **CRUD Operations:** Users can **C**reate, **R**ead, **U**pdate, and **D**elete their notes.
* **Search Functionality:** Quickly find specific notes by searching for their titles.
* **Automatic Timestamps:** The system automatically tags every new or edited note with the current date.
* **Basic Encryption:** Note contents are encrypted using a lightweight XOR cipher before being saved to the text file, ensuring they can't be easily read by just opening the text file.

## What You Will Learn from This Project

If you are a beginner looking at the source code, this project is a practical way to learn:

* **File Handling in C:** How to open, read, write, append, and rewind text files using `fopen()`, `fgets()`, `fprintf()`, and temporary files for safe deletion/editing.
* **Data Structuring:** Using `struct` to group related variables together (like grouping a username and password, or a note's date, title, and content).
* **String Manipulation:** Using `<string.h>` functions like `strcmp()` (to check passwords), `strlen()`, and `strlwr()` (for case-insensitive searching).
* **Basic Cryptography:** How an XOR bitwise operation (`^`) works to scramble text into gibberish and unscramble it back to readable text.
* **Menu-Driven UI:** How to use `switch` cases and loops to create an interactive terminal menu that keeps running until the user decides to exit.

## How It Works

1. **Data Storage:** The system uses plain text files as a database. User credentials are saved in `login_system.txt`.
2. **Delimiters:** Data is separated by the pipe character (`|`). For example, a note is saved as `|Date|Title|EncryptedContent|`. The program reads the file and parses these segments to display them cleanly.
3. **Encryption:** When a note is saved, a key (the letter `'K'`) is applied to every character of the note using an XOR operation. When the note is viewed in the console, the same key is applied again to decrypt it.

## How to Run the Project

1. Clone the repository to your local machine.
2. Compile the `note.c` file using a standard C compiler (like GCC):
   ```bash
   gcc note.c -o secure_notes

## Run the executable

    ./secure_notes    # On Linux/Mac
    
    secure_notes.exe  # On Windows


## Note to take
This project uses `<conio.h>` for `getch()` and `system("cls")`, which are specific to Windows compilers like MinGW. If you are on Linux/Mac, you may need to tweak these functions.
