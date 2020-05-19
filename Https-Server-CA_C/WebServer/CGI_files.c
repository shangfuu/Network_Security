#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>

#define BUFF_SIZE 512

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

    // Read from stdin fd: the file name
    read(STDIN_FILENO,buf,unread);

    // Open the resource file
	int fd = open(buf, O_RDWR);
    if(fd == -1){
        perror("NOT Found : ");
        exit(EXIT_FAILURE);
    }
    else {
        // Get the file size using fd
        off_t fsize;

        fsize = lseek(fd,0,SEEK_END); // Seek to the end of the file
        free(buf);
        buf = (char*)malloc(sizeof(char)*(fsize+1));    // Get the buffer size should be malloc
        lseek(fd,0,SEEK_SET);   // Seek back to the head of th file

        // Read the file in to buffer
        read(fd, buf, fsize);
        // Output to STDOUT
        printf("%s\n",buf);

        FILE *fp;
        
        // Change current dir to WebRoot
        chdir("./WebServer/WebRoot");

        // Use popen() to use the SHELL
        if ((fp = popen("ls -p | grep -v /","r")) == NULL){
            perror("Popen faild");
            return(EXIT_FAILURE);
        }

        free(buf);
        buf = (char*)malloc(sizeof(char) * BUFF_SIZE);

        while(fgets(buf, BUFF_SIZE, fp) != NULL){
            printf("<a href='./%s' download>%s</a><br>\n",buf, buf);
        }

        if(pclose(fp) == -1){
            perror("Pclose faild");
            return(EXIT_FAILURE);
        }
        free(buf);
    }
    close(fd);
}
