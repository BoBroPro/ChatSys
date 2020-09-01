#ifndef PUSERFROMMAPID
#define PUSERFROMMAPID
#include"utility.hpp"
#include<map>

int puserfrommapID(IDTp ID, std::map<IDTp, User*>* pusersbyID, User** outpusr);

#endif