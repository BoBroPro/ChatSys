#include"utility.hpp"
#include"otherfunc.hpp"

using namespace std;

int main(){
    
    delUserRedis("127.0.0.1", 6379, 999);
    cout << "ok" <<endl;

}