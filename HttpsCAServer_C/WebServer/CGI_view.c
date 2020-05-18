#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/*
   Open and read the file,
   then send back to server.
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
    
    // Read from stdin fd, the file name need to open
    read(STDIN_FILENO,buf,unread);

    printf("CGI: %s\n", buf);

    // Open the resource file
	int fd = open(buf,O_RDONLY);
    if(fd == -1){
        perror("NOT Found\n");
        exit(EXIT_FAILURE);
    }
    else {
        off_t fsize;
        fsize = lseek(fd,0,SEEK_END);   // Get the file size using fd
        free(buf);
        buf = (char*)malloc(sizeof(char)*(fsize+1));
        lseek(fd,0,SEEK_SET);   // Seek back to the head of th file

        // Read the file in to buffer
        read(fd, buf, fsize);
        // Output to STDOUT
        printf("%s\n",buf);
    }
}
