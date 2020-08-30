#ifndef DEALCONNMSG_HPP
#define DEALCONNMSG_HPP

#include<map>
#include"utility.hpp"

int dealconnmsg(int sockfd, User* puser, std::map<unsigned long, User*>* pusersbyID,  char* str, size_t n);

#endif