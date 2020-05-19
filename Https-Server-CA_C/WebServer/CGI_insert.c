#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

/*
Open and read the file,
and write the Query at the end of file.
Then send back to server.
*/

int main(void){
    int unread;
    char *buf;

    // wait for stdin
    while(unread < 1){
        if(ioctl(STDIN_FILENO,FIONREAD,&unread)){
            perror("ioctl");
            exit(EXIT_FAILURE);
        }
    }
    buf = (char*)malloc(sizeof(char)*(unread+1));

    // Read from stdin fd: the file name, query, LOG FILE name
    read(STDIN_FILENO,buf,unread);

    char* filename = strtok(buf,"\n");
    char* query = strtok(NULL,"\n");
    char* LOG_FILE = strtok(NULL, "\n");

    // Open the resource file
	int fd = open(filename, O_RDWR);
    if(fd == -1){
        perror("NOT Found : ");
        exit(EXIT_FAILURE);
    }
    else {
        // Get the file size using fd
        off_t fsize;        
        
        // Write the filename file back to server.
        fsize = lseek(fd,0,SEEK_END); 
        free(buf);
        buf = (char*)malloc(sizeof(char)*(fsize+1));
        lseek(fd,0,SEEK_SET);   // Seek back to the head of th file

        // Read the file in to buffer
        read(fd, buf, fsize);
        // Output to STDOUT
        printf("%s\n",buf);
        free(buf);
        close(fd);

        // Open LOG FILE
        int fd_L = open(LOG_FILE, O_RDWR);
        if (fd_L == -1){
            perror("LOG FILE not found");
        }
        else {
            // write the query at the end of LOG FILE
            lseek(fd_L, 0, SEEK_END);

            // write time
            time_t current = time(NULL);
            char buffer[64];
            char *temp = strtok(ctime(&current), "\n");
            sprintf(buffer,"%s: ", temp);
            write(fd_L, buffer, strlen(buffer));

            write(fd_L, query, strlen(query));
            write(fd_L, "<br>\n", 5);
            close(fd_L);
        }
    }
}
