File System Simulator 
Team Members: Kaitlyn Samuelian, Riley Hill

Steps to compile:
Option 1:
Open the integrated terminal by right clicking on final-project-kaitlyn. One in the terminal type: “g++ app_1/main_1.cpp code_1/fileSim.cpp -o my_program” and hit enter. Then type: “./my_program”. After this, the program gets launched, follow the steps below:
Option 2:
Open the build terminal by right clicking on the build directory. Once in the terminal type: “make”. Then type: “./run_app_1”. After this, the program gets launched, follow the steps below:

Program Description:
This program mimics a real world file system through command line interaction. First, compile the code with one of the options listed above. Next, you will type the size of the disk you wish to simulate. From there, typing “help” will allow you to see all of the valid commands that this program works with. Here's a quick description of the functionality of each command: 
-quit: exists menu loop and quits program
- touch: creates a new file within the current directory 
-edit: edits the file data in the desired file to new data determined by the user(note: only works to edit files(not directories)
-mkdir: creates a new directory
- ls: lists the contents of the current directory, including all the files and subdirectories
 -tree: starts with the current directory and prints the entire descendant subtree of files
and subdirectories
-rm: removes file or directory from the current directory. If you're removing a file, it will simply remove that file. If you choose to remove a directory, it will give you 3 options: 1. Delete the directory only (move its contents to root). 2. Cancel operation. 3. Delete the entire directory and its contents.
-cd: changes the current directory to the desired directory. 
-mv: can be used to change either:
 1) the name of a file/directory 
 2)change the location of the file/directory
 Mv will ask for 2 things, either the name of the existing file and the new file name, or the absolute path of an existing file and the absolute path of the file’s new location. (Note: by absolute path, we mean the path from the root directory to the given file.)
-pwd: Prints the path from the current directory to the root. 
-stat: Given a file name (make sure that the file name is in the current directory),prints all the information from the file including the file size, file type, creation and modified times, and its parent directory.
 -cat: Given a file name (make sure that the file is in the current directory), prints the fileData contents
 -search:Given a file or a directory name, a file/directory with the same name exists in the file system, will print the path from the root to the given file.




