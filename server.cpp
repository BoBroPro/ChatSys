#include"utility.hpp"
#include<set>
#include<iterator>
#include<map>
#include<cctype>
#include<algorithm>
#include"extractID.hpp"
#include"dealconnmsg.hpp"
#include"dealloginedmsg.hpp"
#include"dealmsg.hpp"
#include"dealsigninorup.hpp"
#include"dealsignin.hpp"
#include"dealsignup1.hpp"
#include"dealsignup2.hpp"
#include"dealsignup3.hpp"
#include"otherfunc.hpp"

using namespace std;
int max(int a, int b){
    return a > b? a:b;
}

int main(int argc, char** argv){


    struct sockaddr servaddr, cliaddr;
    socklen_t servlen, clilen;
    int listenfd, connfd;
    int maxfd;
    fd_set rset, allset;
    int n;
    set<int> fds;
    map<int, User*> usersbysockfd;
    map<unsigned long, User*> usersbyID;
    char sendline[MAXBUFLEN], recvline[MAXBUFLEN];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    struct sockaddr_in * ipv4servaddr = reinterpret_cast<struct sockaddr_in *> (&servaddr);
    ipv4servaddr->sin_family = AF_INET;
    ipv4servaddr->sin_port = htons(SERV_PORT);
    ipv4servaddr->sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, &servaddr, sizeof(servaddr));
    listen(listenfd,LINSTENQ);
    FD_ZERO(&allset); 
    FD_SET(listenfd, &allset);
    FD_SET(fileno(stdin),&allset);
    maxfd = max(listenfd,fileno(stdin));
    char strip[22];
    printf("hello world\n");
    int d = 0;
    for(;;){
        rset = allset;
        cout << "round: " << ++d<<endl;
        select(maxfd+1, &rset, NULL, NULL, 0);

        if(FD_ISSET(listenfd, &rset)){
            clilen = sizeof(cliaddr);
            if((connfd = accept(listenfd, &cliaddr, &clilen)) < 0){
                printf("connect error");
            }
            cout << connfd << endl;
            inet_ntop(AF_INET, &cliaddr, strip, sizeof(strip)); 
            //printf("the peer ip: %s\n", strip);

            
            User* puser = new User();
            puser->setipaddr(string(strip));
            puser->setsts(CONNECTED);
            usersbysockfd.emplace(connfd, puser);
            
           
            maxfd = max(connfd, maxfd);
            FD_SET(connfd, &allset);

            char buf[] = "sign in or sign up? input\"sign in\" or \"sign up\"\n";
            write(connfd,  buf, sizeof(buf));
        }
        if(FD_ISSET(fileno(stdin),&rset)){
            if((n = read(fileno(stdin),recvline, MAXBUFLEN)) == 0){
                for(auto it = usersbysockfd.begin();it!= usersbysockfd.end();++it){
                    close(it->first);
                    cout << "close: " << it->first<<endl;
                }
                close(listenfd);
                return 0;
            }
        }

        for(auto iterfd = usersbysockfd.begin(); iterfd!= usersbysockfd.end(); ){
            if(FD_ISSET(iterfd->first, &rset)){
                if((n = read(iterfd->first, recvline, MAXBUFLEN)) < 0){
                    perror("Error occour:");
                }
                else if(n == 0){
                    close(iterfd->first);
                    cout << "closed: " << iterfd->second->getID()<<endl;
                    FD_CLR(iterfd->first, &allset);

                    iterfd->second->setsts(NONE);

                    if(iterfd->second->getsts() == PEERSET){

                        const char buf[] = "the peer has logined out\n";
                        write(iterfd->second->getpeeruser()->getsockfd(), buf, sizeof(buf));
                        iterfd->second->getpeeruser()->setsts(LOGINED);

                        //write redis
                        if(writeRedisUser("127.0.0.1", 6379, *iterfd->second) < 0){
                            cout << "logout, but failed to write redis, because can't connect to the redis" << endl;
                        };                    

                    }
                    else if(iterfd->second->getsts() == LOGINED){
                        //write redis
                        if(writeRedisUser("127.0.0.1", 6379, *iterfd->second) < 0){
                            cout << "logout, but failed to write redis, because can't connect to the redis" << endl;
                        };                    
                    }
                    
                    usersbyID.erase(iterfd->second->getID()); 
                    delete iterfd->second;
                    usersbysockfd.erase(iterfd);

                    iterfd= usersbysockfd.begin();//It's ok because corresponding rset flag was cleared.
                    continue;
                }

                else{
                    //cout << "this is recvline: " << recvline<<endl;
                    map<int, User*>::iterator iteruserbysockfd;
                    if((iteruserbysockfd = usersbysockfd.find(iterfd->first)) != usersbysockfd.end()){

                        if(iteruserbysockfd->second->getsts() == CONNECTED){
                            cout << "Connected" <<endl;
                            dealsigninorup(iteruserbysockfd->first, iteruserbysockfd->second, recvline, n);
                            //dealconnmsg(iteruserbysockfd->first, iteruserbysockfd->second, &usersbyID,  recvline, n);
                        }
                        else if(iteruserbysockfd->second->getsts() == INSIGNIN){
                            dealsignin(iteruserbysockfd, &usersbysockfd, &usersbyID, recvline, n);
                        }
                        else if(iteruserbysockfd->second->getsts() == INSIGNUP1){
                            dealsignup1(iteruserbysockfd->first, iteruserbysockfd->second, &usersbyID, recvline, n);
                        } 
                        else if(iteruserbysockfd->second->getsts() == INSIGNUP2){
                            dealsignup2(iteruserbysockfd->first, iteruserbysockfd->second, recvline, n);
                        }
                        else if(iteruserbysockfd->second->getsts() == INSIGNUP3){
                            dealsignup3(iteruserbysockfd->first, iteruserbysockfd->second, &usersbyID,recvline, n);
                            usersbyID.emplace(iteruserbysockfd->second->getID(), iteruserbysockfd->second);
                        }
                        else{
                            cout << "logined" <<endl;
                            dealmsg(iteruserbysockfd->first, iteruserbysockfd->second, &usersbyID, recvline, n);
                        }
            
                    }          
                }

            }
            ++iterfd;
        }
    }
    return 0;

}