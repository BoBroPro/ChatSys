#include"utility.hpp"
#include<sys/epoll.h>
#include<fcntl.h>
#include<set>
#include<iterator>
#include<map>
#include<cctype>
#include<algorithm>
#include"extractID.hpp"
#include"dealconnmsg.hpp"
#include"checkonline.hpp"
#include"dealmsg.hpp"
#include"dealsigninorup.hpp"
#include"dealsignin.hpp"
#include"dealsignup1.hpp"
#include"dealsignup2.hpp"
#include"dealsignup3.hpp"
#include"otherfunc.hpp"
#include"cacheUID.hpp"

enum {
    MAX_EVENTS = 10000,
};


using namespace std;
int max(int a, int b){
    return a > b? a:b;
}

static int setnonblocking(int sockfd)  
{  
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)| O_NONBLOCK) == -1)  
    {  
        return -1;  
    }  
    return 0;  
} 

int main(int argc, char** argv){


    struct sockaddr servaddr, cliaddr;
    socklen_t servlen, clilen;
    int listenfd, connfd;
    int maxfd;
    fd_set rset, allset;
    int n, d;
    set<int> fds;
    map<int, User*> usersbysockfd;
    map<unsigned long, User*> usersbyID;
    char sendline[MAXBUFLEN], recvline[MAXBUFLEN];
    char strip[22];
    printf("hello world\n");
    int nfds, i;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    struct sockaddr_in * ipv4servaddr = reinterpret_cast<struct sockaddr_in *> (&servaddr);
    ipv4servaddr->sin_family = AF_INET;
    ipv4servaddr->sin_port = htons(SERV_PORT);
    ipv4servaddr->sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, &servaddr, sizeof(servaddr));
    listen(listenfd,LINSTENQ);

    struct epoll_event ev, events[MAX_EVENTS];
    int epfd = epoll_create(10);
    if(epfd == -1){
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;   
    ev.data.fd= listenfd;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1){
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fileno(stdin);
    setnonblocking(fileno(stdin));
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fileno(stdin),&ev) == -1){
        perror("epoll_ctl");
    }

    for(;;){
        cout << "round: " << ++d<<endl;
        if((nfds = epoll_wait(epfd, events, MAX_EVENTS, -1)) == -1){
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for(i = 0; i< nfds; ++i){
            if(events[i].data.fd == listenfd){
                connfd = accept(listenfd, &cliaddr, &clilen);
                if(connfd == -1){
                    perror("accept");
                }
                cout << "successful connection"<<endl;
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                setnonblocking(connfd);
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev) == -1){
                    perror("epoll_ctl: connfd");
                }

                inet_ntop(AF_INET, &cliaddr, strip, sizeof(strip)); 
                //printf("the peer ip: %s\n", strip);
            
                User* puser = new User();
                puser->setipaddr(string(strip));
                puser->setsts(CONNECTED);
                usersbysockfd.emplace(connfd, puser);
                char buf[] = "sign in or sign up? input\"sign in\" or \"sign up\"\n";
                write(connfd,  buf, sizeof(buf));
            }
            else{
                if(events[i].data.fd == fileno(stdin)){

                    if((n = read(fileno(stdin),recvline, MAXBUFLEN)) == 0){
                        for(auto it = usersbysockfd.begin();it!= usersbysockfd.end();++it){
                            close(it->first);
                            cout << "close: " << it->first<<endl;
                        }
                        close(listenfd);
                        return 0;
                    }
                }
                else{
                    int eventfd = events[i].data.fd;
                    for(;;) {

                        if((n = read(eventfd, recvline, MAXBUFLEN)) < 0){
                            perror("Error occour:");
                            break;
                        }
                        else if(n == 0){
                            map<int, User*>::iterator iterfd;
                            close(eventfd);
                            iterfd = usersbysockfd.find(eventfd);
                            cout << "closed: " << iterfd->second->getID()<<endl;

                            if(iterfd->second->getsts() == PEERSET){
                                if(usersbyID.find(iterfd->second->getppeeronline()->getID()) != usersbyID.end()){

                                    const char buf[] = "the peer has logined out\n";
                                    write(iterfd->second->getppeeronline()->getsockfd(), buf, sizeof(buf));
                                }
                            }

                            if(iterfd->second->getID() == 0){

                                delete iterfd->second;
                                usersbysockfd.erase(iterfd);
                                break;
                            }
                            CacheUID::push(*iterfd->second); 
                            //writeOrModifyUserRedis("127.0.0.1", 6379, *iterfd->second);
                            updateMySQLUser(*iterfd->second);

                            usersbyID.erase(iterfd->second->getID()); 
                            delete iterfd->second;
                            usersbysockfd.erase(iterfd);
                            break;
                        }
                        else {
                            //cout << "this is recvline: " << recvline<<endl;
                            map<int, User*>::iterator iteruserbysockfd;
                            if((iteruserbysockfd = usersbysockfd.find(eventfd)) != usersbysockfd.end()){

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
                            if(n < MAXBUFLEN){
                                break;
                            }
                        }
                    }
                }

            }
        }
    }
    close(listenfd);
    return 0;

}