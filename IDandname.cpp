#include<cctype>
#include<cstdlib>
#include<cstring>
#include<iostream>
using namespace std;

int IDandname(char* IDandnamebuf, size_t n, unsigned long* pID, char* name, size_t namebuflen){
    *pID = 0;
    bool isfoundcomma = false;
    int commapos;
    char* pname;
    for(int i = 0; i< n; i++){

        if(!isfoundcomma){
            if(isdigit(IDandnamebuf[i])){
                continue;
            }
            else if(IDandnamebuf[i] == ','){
                isfoundcomma = true;
                IDandnamebuf[i] = 0;
                *pID = atol(IDandnamebuf);
                commapos = i;
                break;
            }
            else{
                return -1; // ID is non-numberic             
            }
        }

    }

    pname = IDandnamebuf + commapos + 1;
    if(strlen(pname)+1 > namebuflen){
         return -2; // name is longer than namebuf;
    }
    else{
        int i = 0;
        for(i =0; i<strlen(pname)+1 && pname[i] !='\n' && pname[i]!='\0'; ++i){
            name[i] = pname[i];
        }
        name[i] = 0;
    }
    return 0;

}
// test
#if 0
int main(){
    char namebuf[7];
    char IDandnamebuf[] = "124,yangbo22222";
    unsigned long ID;
    int tmp = IDandname(IDandnamebuf,sizeof(IDandnamebuf),&ID, namebuf, sizeof(namebuf));
    if(tmp == -1){
        cout << "ID is not a number" <<endl;
        return -1;
    }
    if(tmp == -2){
        cout << "name is too long" <<endl;
        return -1;
    }

    cout << "ID: "<< ID << " name: " << namebuf <<endl;
    return 0;
}
#endif