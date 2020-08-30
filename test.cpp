#include"utility.hpp"
using namespace std;
int main(){
    cout << "OK" <<endl;
    for(int i = 0; i < 5; ++i){
        if(i == 2){
            cout << "----"<<endl;
            continue;

        }
    }
    return 0;
}