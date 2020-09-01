#include"puserfrommapID.hpp"

using namespace std;

int puserfrommapID(IDTp ID, map<IDTp, User*>* pusersbyID, User** outpusr){
    map<IDTp, User*>::iterator it;
    if((it = pusersbyID->find(ID)) != pusersbyID->end()){
        *outpusr = it->second;
        return 0;
    }
    else{
        return -1;
    }
}