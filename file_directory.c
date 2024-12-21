


/*
 -------------------------------------
 File:    file_directory.c
 Project: Assignment_01
 file description
 -------------------------------------
 Author:  Connor Booth
 ID:      169038238
 Email:   boot8238@mylaurier.ca
 -------------------------------------
*/

#ifndef FILE_DIRECTORY_H_
#define FILE_DIRECTORY_H_
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ftw.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void create_directory(const char *dir_name, mode_t mode);

void create_write_file(const char *file_name, const char *content);

void read_proc_mem_info_to_file(const char *file_name);

void directory_listing(const char *start_dir);

void remove_directory(const char *dir_name);

int create_menu();

void file_copy(const char *input_file_name, const char*output_file_name);
#endif



/* 
* Creates a directory with a given name and
* permissions.
*/
void create_directory(const char *dir_name, mode_t mode){
    if(mkdir(dir_name, mode) == -1){
        perror("mkdir");
    } else {
        printf("The directory '%s' is created successfully with %o.\n", dir_name, mode);
    }

    return;
}

/* 
* Creates a file with a given name and writes the text
* "Operating System is Fun!!" to the file.
*/
void create_write_file(const char *file_name, const char *content){

    int fp = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fp == -1){
        perror("Error opening file");
        return;
    }
    
    size_t len = strlen(content);

    if (write(fp, content, len) == -1){
        perror("Error writing to file");
        close(fp);
        return;
    }

    if (len > 0 && content[len -1] != '\n'){
        if (write(fp, "\n", 1) == -1){
            perror("Error writing newline to file");
        }
    }

    close(fp);

    return;
}

/* 
* Extract and save key information about the system's
* memory from /proc/meminfo to a file.
*/
void read_proc_mem_info_to_file(const char *file_name){
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL){
        perror("Error opening /proc/meminfo\n");
        return;
    }

    FILE *out =fopen(file_name, "w");
    if (out == NULL){
        perror("Error opening output file\n");
        fclose(meminfo);
        return;
    }

    char buffer[256];

    while (fgets(buffer, sizeof(buffer), meminfo) != NULL){
        fputs(buffer, out);
    }

    fclose(meminfo);
    fclose(out);

    printf("Proc info written to %s successfully\n", file_name);
    return; 
}

/* 
* Recursively lists all files and subdirectories in a
* given directory and its subdirectories. Print this list to standard output. 
*/
void directory_listing(const char *start_dir){
    struct dirent *de;

    DIR *dr = opendir(start_dir);

    if (dr == NULL){
        printf("Could not open directory\n");
        return;
    }

    while ((de = readdir(dr)) != NULL){
        printf("%s\n", de->d_name);
    }

    closedir(dr);
    return; 
}

/* 
* remove_directory helper function
*/
int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    int rv = remove(fpath);
    if (rv){
        perror(fpath);
    }

    return rv; 
}

/* 
* remove_directory
*/
int remove_directory_aux(const char *dir_name){
    return nftw(dir_name, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

/* 
* Removes a given directory and its subdirectories
* from the system
*/
void remove_directory(const char *dir_name){
    if (dir_name == NULL){
        printf("Could not find directory\n");
        return;
    }
    char userInput; 
    printf("Warning: You are about to delete the directory %s and all its contents recursively. Proceed? (y/n)\n", dir_name);
    scanf(" %c", &userInput);

    if (userInput == 'n' || userInput == 'N'){
        printf("The directory will not be deleted\n");
        return;
    }

    int dr = remove_directory_aux(dir_name);

    if (dr){
        printf("Could not delete directory\n");
    } else {
        printf("Directory has been successfully deleted\n");
    }

    return; 

}

/* 
* Creates the user menu and asks for user input to guide which function is called
*/
int create_menu(){
    printf("\n");
    printf("----- Menu -----\n");
    printf("1: Create a directory\n");
    printf("2: Create and write to a file\n");
    printf("3: Write contents of /proc/meminfo to a file\n");
    printf("4: List directory contents\n");
    printf("5: Remove a directory\n");
    printf("99: Quit\n");
    printf("Enter your choice: ");
    int user_input;
    scanf(" %d", &user_input);
    return user_input;
}

int main(){
    int user_input = create_menu();

    if (user_input == 1){

        printf("Enter the directory name you want to create:\n");
        char dir_name[64]; 
        scanf(" %63s", dir_name);

        printf("Enter the mode of the created directory (in octal, e.g., 0755):\n");
        int mode; 
        scanf(" %o", &mode);

        create_directory(dir_name, mode);

        return main();

    } else if (user_input == 2){

        printf("Enter the file name you want to write to:\n");
        char file_name[64];
        scanf(" %63s", file_name);
        
        printf("Enter the contents that you want to write to the file:\n");
        char file_contents[256];
        getchar();
        fgets(file_contents, sizeof(file_contents), stdin);

        create_write_file(file_name, file_contents);

        printf("File %s is created and written successfully\n", file_name);

        return main(); 
        
    } else if (user_input == 3){

        printf("Enter the file name you want to store memory information to:\n");
        char file_name[64];
        scanf(" %63s", file_name);

        printf("The file name id %s\n", file_name);
        read_proc_mem_info_to_file(file_name);

        return main();

    } else if (user_input == 4){

        printf("Enter the directory name you want to list the contents for (if you want to list the contents of the current directory, then pass '.')\n");
        char dir_name[256];
        scanf(" %255s", dir_name);

        directory_listing(dir_name);

        return main();
    } else if (user_input == 5){

        printf("Enter the directory name you want to remove:\n");
        char dir_name[256];
        scanf(" %255s", dir_name);

        remove_directory(dir_name);

        return main();

    } else if (user_input == 99){
        return 0;
    } else {
        printf("Not a valid command. Please enter choose options 1-5, or enter 99 to quit\n");
        main(); 
    }

    return 0;
}