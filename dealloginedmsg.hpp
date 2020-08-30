#ifndef DEALLOGINEDMSG_HPP
#define DEALLOGINEDMSG_HPP

#include"utility.hpp"
#include<map>

int dealloginedmsg(int sockfd, User* puser, std::map<unsigned long, User*>* pusersbyID,  char* str, size_t n);

#endif