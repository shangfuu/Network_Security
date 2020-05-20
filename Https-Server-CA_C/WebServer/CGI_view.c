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

    // Open the resource file
	FILE* fd = fopen(buf,"rb");
    if(fd == NULL){
        perror("NOT Found\n");
        exit(EXIT_FAILURE);
    }
    else {
        fseek(fd,0,SEEK_END); 
        int fsize = ftell(fd);
        free(buf);
        buf = (char*)malloc(sizeof(char)*(fsize));
        fseek(fd,0,SEEK_SET);   // Seek back to the head of th file

        // Read the file in to buffer
        fread(buf, fsize, 1, fd);
        // Output to STDOUT
        printf("%s\n",buf);
    }
}
