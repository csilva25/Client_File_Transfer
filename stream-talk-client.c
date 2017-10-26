
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/uio.h>
#include <string.h>

#define MAX_LINE 256

int main(int argc, char *argv[])
{
    char *host;
    if (argc==4)
        host = argv[1];
    else{
        fprintf(stderr, "usage: %s host\n", argv[0]);
        exit(1);
    }
    
    struct addrinfo hints, *rp, *result;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    
    int here;
    if ((here = getaddrinfo(host, argv[2], &hints, &result)) != 0 ){
        printf("Client: host '%s' \n", argv[1]);
        exit(1);
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next){
        if ((here = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol))
            == -1 )
            continue;
        
        if (connect(here, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
        
        close(here);
    }
    
    if (rp == NULL){
        perror("client: connect");
        exit(1);
    }
    
    char buffer[MAX_LINE] = {'\0'};
    int sent_length = strlen(argv[3]);
    
    int recieved_length = recv(here, &buffer, sizeof(buffer), 0);
    send(here, argv[3], sent_length, 0);
    
    
    if(strcmp(buffer, "-1") == 0){
        printf("Server from clients:file '%s' \n", argv[3]);
        close(here);
        exit(1);
    }
    
    else{
        int new_fd = open(argv[3], O_RDWR | O_CREAT | O_TRUNC, 0644);
        write(new_fd, &buffer, recieved_length);
        
        while((recieved_length = recv(here, &buffer, sizeof(buffer), 0))){
            write(new_fd, &buffer, recieved_length);
            memset(&buffer, 0, sizeof(buffer));
        }
        
        close(new_fd);
    }
    
    freeaddrinfo(result);
    close(here);
    
    return 0;
}
