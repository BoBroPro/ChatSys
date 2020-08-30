#ifndef DEALMSG_HPP
#define DEALMSG_HPP

#include"utility.hpp"
#include<map>

int dealmsg(int sockfd, User* puser, std::map<unsigned long, User*>* pusersbyID,  char* str, size_t n);

#endif