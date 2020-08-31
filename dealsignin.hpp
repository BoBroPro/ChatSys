#ifndef DEALSIGNIN_HPP
#define DEALSIGNIN_HPP

#include"utility.hpp"
#include<map>
#include<iterator>
#include<cstdio>

int dealsignin(std::map<int, User*>::iterator iterfd, std::map<int, User*>* pusersbysockfd, std::map<IDTp, User*>* pusersbyID, char* str, size_t n);

#endif