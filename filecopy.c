
/*
 -------------------------------------
 File:    filecopy.c
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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

void file_copy(const char *input_file_name, const char*output_file_name);

#endif

void file_copy(const char *input_file_name, const char *output_file_name){
    // Check if file_name is empty
    if(input_file_name == NULL || output_file_name == NULL){
        printf("Insufficent parameters passed.");
        exit(EXIT_FAILURE);
    }

    // Open file and error check
    int fd = open(input_file_name, O_RDONLY); 
    if (fd == -1){
        if (errno == ENOENT){
            printf("Error: File does not exist.\n");
            exit(EXIT_FAILURE);
        } else if (errno == EACCES){
            printf("Error: User does not have the necessary permissions.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Check if file name already exists
    if (access(output_file_name, F_OK) != -1){
        if(remove(output_file_name)!= 0){
            perror("Error: Could not delete existing output file");
            exit(EXIT_FAILURE);
        }
    }
     // If no error, create output file
    int output = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC | S_IRUSR | S_IWUSR);
    if (output == -1){
        perror("Error creating the file\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read;
    ssize_t bytes_written;
    char buffer[BUFFER_SIZE];
    // Read input file and write to output file
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0){
        bytes_written = write(output, buffer, bytes_read);

        if (bytes_written == - 1){
            perror("Error writing to the output file\n");
            close(fd);
            close(output);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read == -1){
        perror("Error reading from the input file\n");
    }

    // Close files before exiting
    close(fd);
    close(output);

    printf("The contents of the file input.txt have been successfully copied into the output.txt file.\n");
    return;
}

int main(int argc, char*argv[]){
    if (argc != 3){
        printf("Insufficent parameters passed");
        return EXIT_FAILURE;
    }

    const char *input_file_name = argv[1];
    const char *output_file_name = argv[2];
    file_copy(input_file_name, output_file_name);
    return EXIT_SUCCESS;
}