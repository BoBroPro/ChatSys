#include"utility.hpp"
using namespace std;
int main(int argc, char** argv){
    struct sockaddr cliaddr, servaddr;
    int sockfd, connfd;
    char recvline[MAXBUFLEN], sendline[MAXBUFLEN];

    memset(&servaddr, 0, sizeof(servaddr));

    if(argc != 2){
        printf("Just IP<addresss> needs to input\n");
        return(-1);
    }

    struct sockaddr_in* ipv4servaddr = reinterpret_cast<struct sockaddr_in*>(&servaddr);
    ipv4servaddr->sin_family=AF_INET;
    ipv4servaddr->sin_port= htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &ipv4servaddr->sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if((connect(sockfd, &servaddr, sizeof(servaddr))) < 0){
        printf("connection failed!\n");
        exit(0);
    }
    printf("connfd OK!\n");
    int stdinfd = fileno(stdin); 
    int maxfd = stdinfd > sockfd ? stdinfd : sockfd;
    fd_set allset, rset;
    FD_ZERO(&allset);
    FD_SET(sockfd, &allset);
    FD_SET(stdinfd, &allset);
    rset = allset;

    for(;;){

        select(maxfd+1,&rset,NULL,NULL, 0);
        if(FD_ISSET(stdinfd,&rset)){
            memset(sendline, 0, MAXBUFLEN);
            fgets(sendline, MAXBUFLEN, stdin);
            if(strlen(sendline) == 0){
                cout << "EOF" <<endl;
                close(sockfd);
                return 0;
            }
            write(sockfd, sendline, strlen(sendline));
            fputs("-------------------------------\n", stdout);
        }
        if(FD_ISSET(sockfd, &rset)){
            int n;
            if((n = read(sockfd, recvline, MAXBUFLEN)) <= 0){
                close(sockfd);
                return 0;
            };
            recvline[n] = 0;
            fputs(recvline, stdout);

        }
        rset = allset;
        
    }
    return 0;
}