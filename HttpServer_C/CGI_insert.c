#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>

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

    // Read from stdin fd: the file name, query
    read(STDIN_FILENO,buf,unread);

    char* filename = strtok(buf,"\n");
    char* query = strtok(NULL,"\n");

    // Open the resource file
	int fd = open(filename, O_RDWR);
    if(fd == -1){
        perror("NOT Found : ");
        exit(EXIT_FAILURE);
    }
    else {
        // Get the file size using fd
        off_t fsize;

        // write the query at the end of file
        lseek(fd,0,SEEK_END); 
        write(fd, "\n<h2>",5);
        write(fd, query, strlen(query));
        write(fd,"</h2>",5);
        
        lseek(fd,0,SEEK_SET);   // Seek back to the head of th file
        fsize = lseek(fd,0,SEEK_END); 
        free(buf);
        buf = (char*)malloc(sizeof(char)*(fsize+1));
        lseek(fd,0,SEEK_SET);   // Seek back to the head of th file

        // Read the file in to buffer
        read(fd, buf, fsize);
        // Output to STDOUT
        printf("%s\n",buf);
    }
    close(fd);
}