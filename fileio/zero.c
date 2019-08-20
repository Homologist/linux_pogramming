#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <error.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
 
 
#include <assert.h>
 
 
#define BUF_SIZE 8192       // 8k ;)
 
 
static off_t fsize(const char *filename);
 
 
/**
 * fsize(const char *filename)
 * 
 * @brief Returns the filesize of a file using stat.h
 * @param filename
 * @return -1 if there is an error, zero or positive value otherwise
 */
static off_t fsize(const char *filename)
{
    struct stat st = { 0 };
 
 
    if (stat(filename, &st) == 0) {
 
 
        return st.st_size;
    }
 
 
    return (-1);
}
 
 
/**
 * main(int argc, char **argv)
 * 
 * @brief A main function
 * @param argc
 * @param argv
 * @return -1 if error, 0 for success
 */
int main(int argc, char **argv)
{
    FILE *inputFP = NULL;
    FILE *outputFP = NULL;
    char buf[8192] ;// = { };
    ssize_t file_size = 0;
    ssize_t apparent_size = 0;
 
 
    /* Open a file descriptor for the input file */
    if ((inputFP = fopen(argv[1], "r")) == NULL) {
        perror("Error opening input file descriptor");
        return (-1);
    }
 
 
    /* Determine the file_size of the input file */
    if ((apparent_size = fsize(argv[1])) < 0) {
        perror("Unable to determine accurate size of file");
        return (-1);
    }
    printf("file's apparent size:%i\n", (unsigned int)apparent_size);
 
 
    /* Open a file descriptor for the output file */
    if ((outputFP = fopen(argv[2], "w")) == NULL) {
        perror("Error opening output file descriptor");
        return (-1);
    }
 printf("%s", argv[2]);
 
    /* Lets advise the kernel that we are going to attempt a long
     * sequential write */
    if (posix_fadvise(fileno(inputFP), 0, 0, POSIX_FADV_SEQUENTIAL) < 0) {
        perror("Unable to advise the kernel for the upcoming sequential write");
        // Continue anyways...
    }
 
 
    /* Create a file using ftruncate and lets carry on */
    if (ftruncate(fileno(outputFP), apparent_size) < 0) {
        perror("Unable to create a file that will have a similar size to the original");
        return (-1);
    }
 
 
    /* Read from the input file descriptor and write to the output file descriptor
     * while keeping the 8k within the CPUs L1 cache */
 
 
    int i = 0;
    unsigned long holeSize = 0;
    while ((file_size = fread(buf, sizeof(char), sizeof(buf), inputFP)) > 0) {
 
 
        for (i = 0; i < file_size; i++) {
 
 
            if (buf[i] == '\0') {
                holeSize++;
            } else if (holeSize > 0) {
                fseek(outputFP, holeSize, SEEK_CUR);
		printf("totot %c", &buf[i]);
                fwrite(&buf[i], 1, 1, outputFP);
                clearerr(outputFP);
                if (ferror(outputFP)) {
                    perror("write\n");
                    break;
                }
                holeSize = 0;
            } else {
 
 
                fwrite(&buf[i], sizeof(char), sizeof(char), outputFP);
                 
                if (ferror(outputFP)) {
                    perror("write\n");
                    break;
                }
            }
 
 
        }
 
 
    }
 
 
    /* Close the input file descriptor */
    if (fclose(inputFP) < 0) {
        perror("closing inputFP");
        return (-1);
    }
 
 
    /* Close the output file descriptor */
    if (fclose(outputFP) < 0) {
        perror("closing outputFP");
        return (-1);
    }
 
 
    return 0;
}
