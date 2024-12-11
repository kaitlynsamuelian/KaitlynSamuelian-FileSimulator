#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include "fileSim.hpp"
#include <vector>
using namespace std;
//test
#include <filesystem>
namespace fs = std::filesystem;

// #include "fileSim.hpp"
// #include <vector>

// ===========Lifecycle methods===========

FileSim::FileSim(int _diskSize) : numCollisions(0)
{
    diskSize = _diskSize;

    root = new Inode;
    root->parentNode = nullptr;
    root->name = "(root)";
    root->creationTime = time(NULL);
    root->modTime = root->creationTime;
    root->isDirectory = true;
    currentDir = root;
    currentUsedDiskMemory = 0;

    // Add Hash Table intialization
    tableSize = 5;
    numElements = 0;
    numCollisions = 0;
    table = new HashNode *[tableSize];
    HashNode *previous = nullptr;

    for (int i = 0; i < tableSize; ++i)
    {
        //call to create a new node
        string key = to_string(i);
        HashNode *newNode = createNode(key, nullptr, nullptr);
        table[i] = newNode;
        if (previous != nullptr)
        {
            //update previous
            previous->next = table[i];
        }

        //establish previous pointer
        previous = table[i];
    }
}

FileSim::~FileSim()
{
    // Any postprocessing that may be required
}

// ===========Helper methods===========

int FileSim::makeNode(std::string name, bool isDirectory)
{
    if (currentUsedDiskMemory + 1 > diskSize)
    {
        cout << "No space left on disk!" << endl;
        return 1;
    }

    Inode *newNode = new Inode;
    newNode->name = name;
    newNode->isDirectory = isDirectory;
    newNode->fileSize = 1;              //Set the  to one
    newNode->creationTime = time(NULL); //Update the creationTime member with the current time.
    ////Insert the Inode into the current directory.
    newNode->parentNode = currentDir;
    currentDir->childNodes.push_back(newNode);
    currentUsedDiskMemory = currentUsedDiskMemory + newNode->fileSize;
    bool insert = insertItemLinear(name, newNode);
    return 0;
}

Inode *getChild(Inode *parent, std::string name)
{
    for (size_t i = 0; i < parent->childNodes.size(); i++)
    {
        if (parent->childNodes[i]->name == name)
        {
            return parent->childNodes[i];
        }
    }
    return nullptr;
}

Inode *FileSim::pathToNode(std::string path)
{
    // Returns the node pointed to by `path`.
    //
    // `path` may be...
    //    1. The special path '..', which returns the parent of the current
    //       directory, or the current directory if there is no parent.
    //    2. An absolute path in the format /dir1/dir2/...
    //       where / is the root and /dir1 is a child directory of the root.
    //    3. A relative path (a file in the current working directory)

    // 1
    if (path == "..")
    {
        return currentDir->parentNode;
    }

    // 2 & 3
    Inode *node;
    if (path.front() == '/')
    {
        node = root; // 2
        path = path.substr(1);
    }
    else
    {
        node = currentDir; // 3
    }

    std::stringstream pathStream(path);
    std::string pathSegment;
    std::vector<std::string> seglist;

    while (std::getline(pathStream, pathSegment, '/'))
    {
        if (Inode *child = getChild(node, pathSegment))
        {
            node = child;
        }
        else
        {
            return nullptr;
        }
    }
    return node;
}

std::string FileSim::nodeToPath(Inode *node)
{
    // Returns an absolute path given a node
    if (!node->parentNode)
    {
        // Root
        return "/";
    }

    std::string path = "";
    while (node->parentNode)
    {
        path.insert(0, "/" + node->name);
        node = node->parentNode;
    }
    return path;
}

int recursiveDelete(Inode *node)
{
    //
    if (!node)
    {
        return 0;
    }
    if (node->isDirectory)
    {
        for (size_t i = 0; i < node->childNodes.size(); i++)
        {
            recursiveDelete(node->childNodes[i]);
        }
    }
    delete node;

    return 0;
}

void treeHelper(Inode *node, int level)
{
    cout << " " << node->name;
    cout << endl;

    if (node->isDirectory)
    {
        for (size_t i = 0; i < node->childNodes.size(); i++)
        {
            cout << "-";
            treeHelper(node->childNodes[i], level + 1);
        }
    }
}

std::string timeToString(time_t t)
{
    return std::string();
}

// ===========Public API===========

int FileSim::touch(std::string fileName)
{

    for (size_t i = 0; i < currentDir->childNodes.size(); i++)
    {
        if (currentDir->childNodes[i]->name == fileName)
        {
            cout << "Duplicate name: " << fileName << endl;
            return 1;
        }
    }

    return makeNode(fileName, false);
}

int FileSim::mkdir(std::string dirName)
{

    for (size_t i = 0; i < currentDir->childNodes.size(); i++)
    {
        if (currentDir->childNodes[i]->name == dirName)
        {
            cout << "Duplicate name: " << dirName << endl;
            return 1;
        }
    }

    return makeNode(dirName, true);
}

void FileSim::ls()
{
    if (currentDir->childNodes.empty())
    {
        cout << "";
        return;
    }
    //there are files or directories
    for (size_t i = 0; i < currentDir->childNodes.size(); i++)
    {
        Inode *currentNode = currentDir->childNodes[i];
        cout << currentNode->name << endl;
    }
}

void FileSim::pwd()
{
    //pwd - from the current directory, print the path to the root.
    string path = nodeToPath(currentDir);
    cout << path << endl;
}

void FileSim::tree()
{
    treeHelper(currentDir, 0);
}

int FileSim::cat(std::string path)
{
    //

    Inode *file = pathToNode(path);
    if (!file)
    {
        cout << "cat: " << path << ": No such file or directory" << endl;
        return 1;
    }
    if (file->isDirectory)
    {
        cout << "cat: " << path << ": Is a directory" << endl;
        return 1;
    }
    cout << file->fileData << endl;

    return 0;
}

int FileSim::stat(std::string path)
{
    Inode *statNode = pathToNode(path);
    bool isItThere = false;
    if (statNode == nullptr)
    {
        cout << "Not there" << endl;
        return 0;
    }
    //search current directory
    for (int i = 0; i < currentDir->childNodes.size(); i++)
    {
        if (currentDir->childNodes[i] == statNode)
        {
            isItThere = true;
            break;
        }
    }

    if (isItThere == false)
    {
        cout << "Not in correct directory" << endl;
        return 0;
    }

    else
    {
        cout << path << " information: " << endl;
        cout << "Name: " << statNode->name << endl;
        cout << "File Data: " << statNode->fileData << endl;
        ;
        cout << "Filse Size: " << statNode->fileSize << endl;
        cout << "Creation Time: " << statNode->creationTime << endl;
        cout << "Mod Time: " << statNode->modTime << endl;
        ;
        cout << "Parent Directory: " << statNode->parentNode->name << endl;
    }

    return 0;
}

int FileSim::edit(std::string path, std::string newValue)
{
    // Overwrite the file with a new value
    //ask the user which file to edit.

    // cout << "what is the name of the file you would like to edit ." << endl;
    // cin.ignore();
    // getline(cin, path);

    Inode *fileToEdit = pathToNode(path);

    //     //if it still is nullptr, it doesnt exist

    if (fileToEdit == nullptr)
    {
        cout << "edit: " << path << ": No such file or directory" << endl;
        return 1;
    }
    //     //dont allow editing if it is a directory type
    if (fileToEdit->isDirectory == true)
    {
        cout << "edit: " << fileToEdit->name << ": Is a directory" << endl;
        return 1;
    }
    //     //Ask the user to enter a string to override the string in the corresponding fileData Inode member

    // //The new file size is the length of the fileData string size plus one. Ensure that this does not cause the disk capacity to overflow, and update the corresponding fileSize.
    int newFileSize = newValue.length() + 1;
    if ((currentUsedDiskMemory - fileToEdit->fileSize) + newFileSize > diskSize)
    {
        cout << "edit: No more space left on disk" << endl;
        return 1;
    }
    fileToEdit->fileData = newValue;
    currentUsedDiskMemory = (currentUsedDiskMemory - fileToEdit->fileSize) + newFileSize;
    fileToEdit->fileSize = newFileSize;
    fileToEdit->modTime = time(NULL);

    return 0;
}

int FileSim::cd(std::string dirPath)
{
    //cd - change the directory by traversing the tree structure and updating the currentDir pointer.
    Inode *directory = pathToNode(dirPath);

    //case where directory doesnt exsists
    if (!directory)
    {
        cout << "cd: no such file or directory: " << dirPath << endl;
        return 1; //return 1 because this is an error
    }

    //case where path goes to a file not directory
    if (!directory->isDirectory)
    {
        cout << "cd: not a directory: " << dirPath << endl;
        return 1;
    }

    //case 3: valid directory exisists: update the pointer
    currentDir = directory;
    return 0;
}

int FileSim::rm(std::string path, bool recursive)
{
    if (path == "." || path == "..")
    {
        cout << "rm: '.' and '..' may not be removed" << endl;
        return 1;
    }

    Inode *node = pathToNode(path);
    if (!node)
    {
        cout << "rm: " << path << ": No such file or directory" << endl;
        return 1;
    }

    // If the node is a directory and not recursive, ask the user for a choice
    if (node->isDirectory && !recursive)
    {
        cout << "rm: " << path << " is a directory. Options:" << endl;
        cout << "1. Delete the directory only (move its contents to root)" << endl;
        cout << "2. Cancel operation" << endl;
        cout << "3. Delete the entire directory and its contents" << endl;
        cout << "Enter your choice (1/2/3): " << endl;
        int choice;
        cin >> choice;

        if (choice == 2) // Cancel operation
        {
            cout << "Operation canceled.\n";
            return 1;
        }
        else if (choice == 1) // Move contents to root
        {
            cout << "Moving contents of " << path << " to root." << endl;
            // Move each child node to the root directory
            for (size_t i = 0; i < node->childNodes.size(); i++)
            {
                Inode *child = node->childNodes[i];
                cout << "Moving child node: " << child->name << " to root." << endl;
                child->parentNode = root;
                root->childNodes.push_back(child);
            }
            node->childNodes.clear(); // Remove all children from the current node
        }
        else if (choice == 3) // Recursive deletion
        {
            recursive = true;
            cout << "Recursively deleting the entire directory and its contents." << endl;

            // If the directory has no children, just delete the directory
            if (node->childNodes.empty())
            {
                cout << "The directory has no children. Deleting the directory." << endl;

                // Delete the directory node directly
                Inode *parent = node->parentNode;
                if (parent)
                {
                    // Remove this directory node from the parent's child nodes
                    for (size_t i = 0; i < parent->childNodes.size(); i++)
                    {
                        if (parent->childNodes[i] == node)
                        {
                            parent->childNodes.erase(parent->childNodes.begin() + i);
                            break;
                        }
                    }
                }

                // Delete the directory node (no need for recursive delete)
               // deleteElementofHash(path);
                delete node;
                return 0;
            }
        }
        else
        {
            cout << "Invalid choice. Operation canceled.\n";
            return 1;
        }
    }

    // If the node is a file or directory with recursive deletion selected, proceed
    Inode *parent = node->parentNode;
    if (parent)
    {
        // Ensure we're safely removing the node from its parent's child nodes
        for (size_t i = 0; i < parent->childNodes.size(); i++)
        {
            if (parent->childNodes[i] == node)
            {
                parent->childNodes.erase(parent->childNodes.begin() + i);
                break;
            }
        }
    }

    // Perform the deletion of the node itself (non-recursive)
    delete node;

    cout << "rm: " << path << " has been removed successfully.\n";
    return 0;

    
}

int FileSim::mv(std::string originPath, std::string destPath) //either change name of inode or change location
{
    Inode *originNode = pathToNode(originPath);
    Inode *destNode = pathToNode(destPath);
  

    if (originPath.front() != '/') //we are just switching the names
    {
        // Check to see if new name is already there
        if (destNode != nullptr)
        {
            cout << "mv: " << destPath << ": Already exists!" << endl;
            return 0;
        }
        if(!originNode)
        {
            cout << "origin node does not exist" << endl;
            return 0;
        }
        HashNode *oldHash = searchItem(originNode->name);
        
        if(oldHash)
        {
            deleteElementofHash(originNode->name);
            oldHash->key = destPath;
            insertItemLinear(destPath,originNode);
        }

        
            originNode->name = destPath; // Change the name of the inode
            return 0;
        
       
    }
    else
    {
        if (originNode)
        {
            // Find the parent of the origin node to remove it from its current location
            Inode *originParent = originNode->parentNode;
            if (originParent)
            // Loop through the childNodes to find and remove the originNode
            {
                for (size_t i = 0; i < originParent->childNodes.size(); ++i)
                {
                    if (originParent->childNodes[i] == originNode)
                    {
                        originParent->childNodes.erase(originParent->childNodes.begin() + i); // Remove the node from its current location
                        break;                                                                // Exit the loop once the node is removed
                    }
                }
            }
            string oldpath = nodeToPath(originNode);
        
            
            // Now move the origin node to the new parent
            if (destNode)
            {
                originNode->parentNode = destNode;          // Set the new parent
                insertItemLinear(destPath, originNode);
                destNode->childNodes.push_back(originNode); // Add the node to the destination's childNodes
            }
            else
            {
                cout << destPath << " does not exist" << endl;
                return 0;
            }
        }
        else
        {
            cout << originPath << " does not exist" << endl;
        }
    }
    return 0;

}

void FileSim::search(std::string key)
{

    HashNode *theone = searchItem(key);
    // if(theone){
    
    if (!theone || !theone->iptr)
    {
        
        return;
    }
    Inode *theoneInode = theone->iptr;
   
    string path = nodeToPath(theoneInode);

    cout << path << endl;

    return;
}

// ===========Hash Table===========
HashNode *FileSim::createNode(std::string _key, Inode *_iptr, HashNode *_next)
{
    HashNode *newNode = new HashNode;
 
    newNode->key = _key;
    newNode->iptr = _iptr;
    newNode->next = _next;
 
    return newNode;
}
 
// Hash Function
unsigned int FileSim::hashFunction(string s)
{
    unsigned int hash = 0;
    for (char c : s)
    {
        hash = hash + (int)c;
    }
 
    return hash % tableSize;
}
 
// Search Method
HashNode *FileSim::searchItem(string key)
{
    string indexSTR;
    string name;
    //if path find the node it is searching for
 
    if (key[1] == '/')
    {
        Inode *searchNode = pathToNode(key);
        unsigned int indexINT = hashFunction(searchNode->name);
        indexSTR = to_string(indexINT);
        name = searchNode->name;
    }
 
    else
    { // if it is just a file name
        unsigned int indexINT = hashFunction(key);
       
        indexSTR = to_string(indexINT);
    
        name = key;
    }
 
    bool found = false;
    int j = 1;
    for (int i = 0; i < tableSize; ++i)
    {
 
        if (table[i]->key == indexSTR) //we found the right index but it may not be there
        {
            if (table[i]->iptr != nullptr && table[i]->iptr->name == name)
            { //we found the right one
                return table[i];
            }
            else
            { //we need to change the index and look again
                int INDEX = stoi(indexSTR);
                INDEX = INDEX + j;
                indexSTR = to_string(INDEX);
                j = j + 1;
                i = 0;
            }
        }
    }
cout << "not found " << endl;
    return nullptr; // Key not found
}
 
// Insert new record
bool FileSim::insertItemLinear(std::string _key, Inode *_iptr)
{
    //check load factor to see if we need to resize table
    numElements = numElements + 1;
    int CurrLoadFactor = numElements / tableSize;
 
    if (CurrLoadFactor > loadFactor)
    {
        cout << "resize" << endl;
        resizeHashTable();
    }
    unsigned int indexINT = hashFunction(_key);
    //unsigned int originalIndex = indexINT;
    string indexSTR = to_string(indexINT);
 
    //find correct node that matches the index
    HashNode *indexpointer;
 
    for (int i = 0; i < tableSize; ++i)
    {
        if (table[i]->key == indexSTR)
        {
            indexpointer = table[i];
            break;
        }
    }
 
    //see if it is empty
    if (indexpointer->iptr == nullptr)
    {
        indexpointer->iptr = _iptr;
        cout << _key << " added into hash table" << endl;
        return true;
    }
    else
    {
        int i = getCollisions();
    }
    bool place = false;
    int i = 1;
 
    while (place == false)
    {
        //creates new index
        int indexnew = (indexINT + i) % tableSize;
        indexSTR = to_string(indexnew);
 
        //search for new index
 
        for (int j = 0; j < tableSize; ++j)
        {
            if (table[j]->key == indexSTR)
            {
                indexpointer = table[j];
                break;
            }
        }
 
        if (indexpointer->iptr == nullptr)
        {
            indexpointer->iptr = _iptr;
            cout << _key << " inserted into hash" << endl;
            place = true;
        }
        //update i
        i = i + 1;
    }
    return true;
    
}
 
bool FileSim::insertItemQuadratic(std::string _key, Inode *_iptr)
{
 
    //check load factor to see if we need to resize table
    numElements = numElements + 1;
    int CurrLoadFactor = numElements / tableSize;
 
    if (CurrLoadFactor > loadFactor)
    {
        cout << "resize" << endl;
        resizeHashTable();
    }
    unsigned int indexINT = hashFunction(_key);
    //unsigned int originalIndex = indexINT;
    string indexSTR = to_string(indexINT);
 
    //find correct node that matches the index
    HashNode *indexpointer;
 
    for (int i = 0; i < tableSize; ++i)
    {
        if (table[i]->key == indexSTR)
        {
            indexpointer = table[i];
            break;
        }
    }
 
    //see if it is empty
    if (indexpointer->iptr == nullptr)
    {
        indexpointer->iptr = _iptr;
        cout << _key << " added into hash table" << endl;
        return true;
    }
    else
    {
        int i = getCollisions();
    }
    bool place = false;
    int i = 1;
 
    while (place == false)
    {
        //creates new index
        int indexnew = (indexINT + (i*i)) % tableSize;
        indexSTR = to_string(indexnew);
 
        //search for new index
 
        for (int j = 0; j < tableSize; ++j)
        {
            if (table[j]->key == indexSTR)
            {
                indexpointer = table[j];
                break;
            }
        }
 
        if (indexpointer->iptr == nullptr)
        {
            indexpointer->iptr = _iptr;
            cout << _key << " inserted into hash" << endl;
            place = true;
        }
        //update i
        i = i + 1;
    }
    return true;
}
 
bool FileSim::insertItemChaining(std::string _key, Inode *_iptr)
{

return true;  

}
 
//Resize the table
void FileSim::resizeHashTable()
{
    cout << "inresize" << endl;
    int oldTableSize = tableSize;
    tableSize = tableSize * 2;
    //create new table with the new size
    HashNode **tableNEW = new HashNode *[tableSize];
 
    for (int i = 0; i < oldTableSize; i++)
    {
 
        tableNEW[i] = table[i];
    }
 
    int BLANKones = tableSize - oldTableSize;
    cout << BLANKones << endl;
    HashNode *previous = tableNEW[oldTableSize - 1];
 
    for (int k = BLANKones; k < tableSize; ++k)
    {
        //create new table node
        string KSTR = to_string(k);
        HashNode *New = createNode(KSTR, nullptr, nullptr);
 
        tableNEW[k] = New;
        previous->next = tableNEW[k];
        previous = tableNEW[k];
    }
 
    table = tableNEW;
}
 
void FileSim::resetCollisions()
{
}
 
int FileSim::getCollisions() const
{
    cout << "collision" << endl;
 
    return (numCollisions + 1);
}
 
void FileSim::deleteElementofHash(std::string key)
{
    HashNode *deleter = searchItem(key);
    
    deleter->iptr = nullptr;
    return;
}
 
