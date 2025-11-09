#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <iomanip>
#include <ctime>

using namespace std;

class FileManager {
public:
    // -------------------- Basic Operations --------------------

    void listFiles(const string& path = ".") {
        DIR* dir = opendir(path.c_str());
        if (!dir) {
            cerr << "Error: Unable to open directory: " << path << endl;
            return;
        }

        cout << "\nFiles in Directory: " << path << endl;
        cout << "--------------------------------------" << endl;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            cout << entry->d_name << endl;
        }

        cout << "--------------------------------------" << endl;
        closedir(dir);
    }

    bool changeDirectory(const string& path) {
        if (chdir(path.c_str()) == 0) {
            cout << "Directory changed to: " << path << endl;
            return true;
        }
        cerr << "Error: Failed to change directory." << endl;
        return false;
    }

    void createFile(const string& filename) {
        FILE* file = fopen(filename.c_str(), "w");
        if (!file) {
            cerr << "Error: Unable to create file." << endl;
            return;
        }
        fclose(file);
        cout << "File created: " << filename << endl;
    }

    void copyFile(const string& source, const string& destination) {
        FILE* src = fopen(source.c_str(), "rb");
        FILE* dest = fopen(destination.c_str(), "wb");

        if (!src || !dest) {
            cerr << "Error: Cannot open source or destination file." << endl;
            if (src) fclose(src);
            if (dest) fclose(dest);
            return;
        }

        char buffer[4096];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
            fwrite(buffer, 1, bytes, dest);
        }

        fclose(src);
        fclose(dest);
        cout << "File copied successfully." << endl;
    }

    void moveFile(const string& source, const string& destination) {
        if (rename(source.c_str(), destination.c_str()) == 0) {
            cout << "File moved successfully." << endl;
        } else {
            cerr << "Error: Failed to move file." << endl;
        }
    }

    void deleteFile(const string& filename) {
        if (remove(filename.c_str()) == 0) {
            cout << "File deleted: " << filename << endl;
        } else {
            cerr << "Error: Failed to delete file." << endl;
        }
    }

    void searchFiles(const string& dirPath, const string& fileName) {
        DIR* dir = opendir(dirPath.c_str());
        if (!dir) return;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, fileName.c_str()) == 0) {
                cout << "Found: " << dirPath << "/" << entry->d_name << endl;
            } else if (entry->d_type == DT_DIR &&
                       strcmp(entry->d_name, ".") != 0 &&
                       strcmp(entry->d_name, "..") != 0) {
                searchFiles(dirPath + "/" + entry->d_name, fileName);
            }
        }

        closedir(dir);
    }

    void managePermissions(const string& filename, mode_t perms) {
        if (chmod(filename.c_str(), perms) == 0) {
            cout << "Permissions updated for: " << filename << endl;
        } else {
            cerr << "Error: Failed to update permissions." << endl;
        }
    }

    // -------------------- File Info Feature --------------------

    void showFileInfo(const string& filename) {
        struct stat fileStat;

        if (stat(filename.c_str(), &fileStat) != 0) {
            cerr << "Error: Unable to access file information." << endl;
            return;
        }

        cout << "\nFile Information: " << filename << endl;
        cout << "--------------------------------------" << endl;

        cout << left << setw(20) << "Size (bytes): " << fileStat.st_size << endl;
        cout << left << setw(20) << "File Type: " 
             << ((S_ISDIR(fileStat.st_mode)) ? "Directory" : "Regular File") << endl;

        cout << left << setw(20) << "Permissions: ";
        cout << ((fileStat.st_mode & S_IRUSR) ? "r" : "-")
             << ((fileStat.st_mode & S_IWUSR) ? "w" : "-")
             << ((fileStat.st_mode & S_IXUSR) ? "x" : "-")
             << ((fileStat.st_mode & S_IRGRP) ? "r" : "-")
             << ((fileStat.st_mode & S_IWGRP) ? "w" : "-")
             << ((fileStat.st_mode & S_IXGRP) ? "x" : "-")
             << ((fileStat.st_mode & S_IROTH) ? "r" : "-")
             << ((fileStat.st_mode & S_IWOTH) ? "w" : "-")
             << ((fileStat.st_mode & S_IXOTH) ? "x" : "-") << endl;

        char timeBuf[80];
        struct tm* timeinfo = localtime(&fileStat.st_mtime);
        strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", timeinfo);
        cout << left << setw(20) << "Last Modified: " << timeBuf << endl;

        cout << "--------------------------------------" << endl;
    }

    // -------------------- Utility Methods --------------------

    void showMenu() {
        cout << "\n==========================================" << endl;
        cout << "             FILE MANAGEMENT SYSTEM        " << endl;
        cout << "==========================================" << endl;
        cout << "Available Commands:" << endl;
        cout << "  list                         - List all files" << endl;
        cout << "  cd <dir>                     - Change directory" << endl;
        cout << "  create <file>                - Create a new file" << endl;
        cout << "  copy <src> <dest>            - Copy a file" << endl;
        cout << "  move <src> <dest>            - Move a file" << endl;
        cout << "  del <file>                   - Delete a file" << endl;
        cout << "  search <file>                - Search a file recursively" << endl;
        cout << "  chmod <file> <octal>         - Change file permissions" << endl;
        cout << "  showinfo <file>              - Display file details" << endl;
        cout << "  exit                         - Exit the program" << endl;
        cout << "------------------------------------------" << endl;
    }

    void run() {
        string command, arg1, arg2;
        char cwd[1024];

        while (true) {
            getcwd(cwd, sizeof(cwd));
            cout << "\nCurrent Directory: " << cwd << endl;
            cout << "Enter Command: ";
            cin >> command;

            if (command == "list") {
                listFiles();
            }
            else if (command == "cd") {
                cin >> arg1;
                changeDirectory(arg1);
            }
            else if (command == "create") {
                cin >> arg1;
                createFile(arg1);
            }
            else if (command == "copy") {
                cin >> arg1 >> arg2;
                copyFile(arg1, arg2);
            }
            else if (command == "move") {
                cin >> arg1 >> arg2;
                moveFile(arg1, arg2);
            }
            else if (command == "del") {
                cin >> arg1;
                deleteFile(arg1);
            }
            else if (command == "search") {
                cin >> arg1;
                searchFiles(".", arg1);
            }
            else if (command == "chmod") {
                cin >> arg1 >> arg2;
                mode_t perms = strtol(arg2.c_str(), nullptr, 8);
                managePermissions(arg1, perms);
            }
            else if (command == "showinfo") {
                cin >> arg1;
                showFileInfo(arg1);
            }
            else if (command == "exit") {
                cout << "Exiting File Management System..." << endl;
                break;
            }
            else {
                cout << "Invalid command. Try again." << endl;
                showMenu();
            }
        }
    }
};

// -------------------- Main Function --------------------

int main() {
    FileManager manager;
    manager.showMenu();
    manager.run();
    return 0;
}
