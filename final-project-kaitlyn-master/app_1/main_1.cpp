#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "../code_1/fileSim.hpp"
using namespace std;
//where you at 
bool validateArgs(std::string command, std::vector<string>& args) {
    // Validates the number of args for a command

    if (command == "rm") {
        if (args.size() != 2 && args.size() != 3) {
            cout << "rm requires 1 or 2 arguments!" << endl;
            return false;
        }
        return true;
    }

    std::vector<std::string>::size_type desiredArgs;
    if (command == "ls" || command == "tree" || command == "pwd") {
        desiredArgs = 0;
    } else if (command == "touch" || command == "mkdir" || command == "cd" || command == "stat" || command == "cat" || command == "search") {
        desiredArgs = 1;
    } else {
        desiredArgs = 2;
    }
    if (args.size() != desiredArgs + 1) {
        cout << command << " requires " << desiredArgs << " arguments, but you passed " << args.size() - 1 << endl;
        return false;
    }
    return true;
}

int handleCommand(std::string command, std::vector<std::string>& args, FileSim* fs) {
    // Verify we have passed the correct number of arguments
    if (!validateArgs(command, args)) { return 1; }

    // Resolve the command with the appropriate function
    if (command == "touch") {
        return fs->touch(args.at(1));
    } else if (command == "edit") {
            return fs->edit(args.at(1), args.at(2));
    } else if (command == "mkdir") {
        return fs->mkdir(args.at(1));
    } else if (command == "ls") {
        fs->ls();
    } else if (command == "tree") {
        fs->tree();
    } else if (command == "rm") {
        bool recursive = false;
        if (args.size() == 3) {
            if (args.at(2) == "-r") {
                recursive = true;
            } else {
                cout << "rm: Invalid flag " << args.at(2) << endl;
                return 1;
            }
        }
        return fs->rm(args.at(1), recursive);
    } else if (command == "cd") {
        return fs->cd(args.at(1));
    } else if (command == "mv") {
        return fs->mv(args.at(1), args.at(2));
    } else if (command == "pwd") {
        fs->pwd();
    } else if (command == "stat") {
        return fs->stat(args.at(1));
    } else if (command == "cat") {
        return fs->cat(args.at(1));
    } else if (command == "search") {
        fs->search(args.at(1));
    } else {
        cout << "Invalid command. Use `help` to get a list of valid commands." << endl;
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {

    if(argc!=1 && argc != 2) 
    {
        cout << "Incorrect # Arguments. Need 1 or 2 arguments." << endl;
        return 1;  
    }
     string diskSizeInput;
    int diskSize;
       //get user input for disk size
    //using stringstream to clear input to avoid errors. 
    cout << "Enter your desired disk size." << endl;
    getline(cin, diskSizeInput);
    stringstream ss(diskSizeInput);
    ss>>diskSize;

    cout << "Welcome to fileSim! Type `help` to view commands." << endl;

    bool quit = false;    

    // Initialize an object of the FileSim class
    FileSim fs = FileSim(diskSize); // TODO: set the file sim to be users input 
    // test that disk size updated properly: cout<<"disk size is"<<diskSize<<endl;

    ifstream inFile(argv[1]);
    std::string s_input;
    int lastReturnValue;

    while(!quit) {
        
        if (argc == 2){
            
            getline(inFile, s_input);
        }
        
        else{
            cout << "> ";
            getline(cin, s_input);
        }

        // Split the input into args
        vector<string> input_args;
        std::istringstream iss(s_input);
        string arg;
        while (iss >> arg) {
            input_args.push_back(arg);
        }

        if (input_args.empty()) {
        continue; // Ignore empty input
      }

        string command = input_args.front();

        if (command == "help") {
            cout << "Valid commands: quit, touch, edit, mkdir, ls, tree, rm, cd, mv, pwd, stat, cat, search" << endl;
        } 
        else if (command == "quit") {
            quit = true;
        }
        //ask for another arg if edit
        else if(command == "edit"){
            cout << "What file would you like to edit?" << endl;
            //add next argument
            string filename;         
            getline(cin,filename);
            input_args.push_back(filename);
            cout << "Enter new data string:" << endl;
            string Newstring;
            getline(cin,Newstring);
            input_args.push_back(Newstring);

            lastReturnValue = handleCommand(command,input_args,&fs);
        }
        else if(command == "touch")
        {
            cout << "Enter the new file name: " << endl;
            string filename;
            cin >> filename;         
            input_args.push_back(filename);
            lastReturnValue = handleCommand(command,input_args,&fs);
        }

        else if(command == "mv")
        {
            cout << "Enter origin file or path" << endl;
            string origin;            
            getline(cin,origin);
            input_args.push_back(origin);

            cout << "Enter new name or path" << endl;
            string newone;           
            getline(cin,newone);
            input_args.push_back(newone);
            lastReturnValue = handleCommand(command,input_args,&fs);
        }
        else if(command == "mkdir"){
            cout << "What is the name of the new directory?" << endl;
            string name;
            getline(cin,name);
            input_args.push_back(name);
          
            lastReturnValue = handleCommand(command,input_args,&fs);

        }
        else if(command == "rm"){
            cout << "What would you like to delete from the current directory?" << endl;
            string deletion;
            getline(cin,deletion);
            input_args.push_back(deletion);

            
             lastReturnValue = handleCommand(command, input_args, &fs); 
        }
        else if(command == "cd")
        {
            cout << "What directory would you like to change to?" << endl;
            string wantedDirect;
            getline(cin,wantedDirect);
            input_args.push_back(wantedDirect);
            lastReturnValue = handleCommand(command, input_args, &fs);
        }
        else if(command == "stat")
        {
            cout << "What file in the current directory do you want to see info for?" << endl;
            string wantedfile;
            getline(cin,wantedfile);
            input_args.push_back(wantedfile);
            lastReturnValue = handleCommand(command, input_args, &fs);
        }
        else if(command == "search")
        {
            cout << "What would you like to search for?" << endl;
            string search;
            getline(cin,search);
            input_args.push_back(search);
            lastReturnValue = handleCommand(command, input_args,&fs);
        }
        else if(command == "cat")
        {
            cout << "What file in current directory would you like to print data contents?" << endl;
            string CAT;
            getline(cin,CAT);
            input_args.push_back(CAT);
            lastReturnValue = handleCommand(command, input_args, &fs);
        }
        else {
            lastReturnValue = handleCommand(command, input_args, &fs);
        }
    }
    return 0;
}

