#ifndef CHECKONLINE_HPP 
#define CHECKONLINE_HPP 

#include"utility.hpp"
#include<map>

int checkonline(int sockfd, User* puser, std::map<unsigned long, User*>* pusersbyID,  char* str, size_t n);

#endif