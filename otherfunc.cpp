#include"otherfunc.hpp"
#include"utility.hpp"
#include"ultoa.hpp"
#include"puserfrommapID.hpp"
#include"mysqlutility.hpp"

using namespace std;

void to_json(json& j, const User& u){

    j = json{
        {"ID", u.ID},
        {"passwd", u.passwd}, 
        {"name", u.name}, 
        {"friends", u.friends}, 
        {"vrfyfrds", u.vrfyfrds}, 
        {"addfrds", u.addfrds}, 
        {"msgsnotread", u.msgsnotread}
    };
    return ;
}
void from_json(const json& j, User& u){

    j.at("ID").get_to(u.ID);
    j.at("passwd").get_to(u.passwd);
    j.at("name").get_to(u.name);
    j.at("friends").get_to(u.friends);
    j.at("vrfyfrds").get_to(u.vrfyfrds);
    j.at("addfrds").get_to(u.addfrds);
    j.at("msgsnotread").get_to(u.msgsnotread);
    return;
}

int findinredis(string ipaddr,PortTp port, IDTp ID, User& outuser){
        CHiredis credisConnect(ipaddr, port);
        string IDkey = "ID";
        char IDbuf[20] = {0};
        ultoa(IDbuf, sizeof(IDbuf),ID);
        IDkey.append(IDbuf);
        string val = credisConnect.get(IDkey);
        if(val.size() != 0){
            json j = json::parse(val);
            outuser = move(j.get<User>());
            return 0; 
        }
        else{
            return -1;
        }
}

string redisIDkey(IDTp ID){

    string rediskey = string("ID");
    char IDbuf[20];
    ultoa(IDbuf, sizeof(IDbuf), ID);
    rediskey.append(string(IDbuf));
    return rediskey;

}

int writeRedisUser(const string& ipaddr, PortTp port, const User& user){
    CHiredis chiredis(ipaddr, port);
    if(chiredis.geterr()<0){
        return -1;
    }
    string key = redisIDkey(user.getID());
    json j(user);
    string val = j.dump(4);
    chiredis.set(key, val);
    return 0;
}

int modifyRedisUser(const string& ipaddr, PortTp port, const User& user){
    CHiredis chiredis(ipaddr, port);
    if(chiredis.geterr()<0){
        return -1;
    }
    string key = redisIDkey(user.getID());
    json j(user);
    string val = j.dump(4);
    chiredis.getset(key, val);
    return 0;
}

void writeOrModifyUserRedis(const User& user){

    User usernouse;
    if(findinredis("127.0.0.1",6379, user.getID(),usernouse) < 0){
        writeRedisUser("127.0.0.1", 6379, user);
    }
    else{
        modifyRedisUser("127.0.0.1",6379, user);
    }
    return;
}
int findUser(IDTp frdID, map<IDTp, User*>* pusersbyID/*and redis*/, User** outppusermap, User* outpuserRedis){
    if(puserfrommapID(frdID, pusersbyID, outppusermap) < 0){
        if(findinredis("127.0.0.1", 6379,frdID, *outpuserRedis) < 0){
            if(findUserinMySQL(frdID, *outpuserRedis) < 0){
                return -1;
            }
            else{
                return 2; //find in mysql;
            }
        }
        else{

            return 1; // find in the redis
        }
    }
    else{
        return 0;  //find int he map
    }
}

int writeMySQLUser(User& user){
    char IDbuf[20];
    ultoa(IDbuf,sizeof(IDbuf),user.getID());
    
    string frds = json(*user.getpfrds()).dump(0);
    string vrfyfrds = json(*user.getpvrfyfrds()).dump(0);
    string addfrds = json(*user.getpaddfrds()).dump(0);
    string msgsnotread = json(*user.getpmsgsnotread()).dump(0);
    string statement = "INSERT INTO users \
                        (ID, passwd, name, friends, vrfyfrds, addfrds, msgsnotread)\
                        VALUES \
                        (";
    statement.append(IDbuf).append(",'").append(user.getpasswd()).append("','")\
    .append(user.getname()).append("','").append(frds).append("','").append(vrfyfrds).append("','")\
    .append(addfrds).append("','").append(msgsnotread).append("')");
    
    cout << statement<<endl;

    CMySQL mysql("127.0.0.1", 3306);
    return mysql.quary(statement);
}


int updateMySQLUser(User& user){
    char IDbuf[20];
    ultoa(IDbuf,sizeof(IDbuf),user.getID());

    string dltStatment = string("DELETE FROM users WHERE ID =").append(string(IDbuf));
    CMySQL cmysql("127.0.0.1", 3306);

    if(cmysql.quary(dltStatment) < 0){
        cout << "DELETE statement grammer error"<<endl;
        return -1;
    }
    else{
        writeMySQLUser(user);
        cout << "write in MySQL ok in update" <<endl;
    }

    return 0;
}

int findUserinMySQL(IDTp ID, User& outuser){
    char IDbuf[20];
    ultoa(IDbuf,sizeof(IDbuf),ID);
    string findStatment = string("SELECT * FROM users WHERE ID =").append(string(IDbuf));

    CMySQL mysql("127.0.0.1", 3306);
    if(mysql.quary(findStatment) < 0){
        return -2;
    }
    vector<string> keys(7);
    vector<string> vals(7);

    int affectedRowsn = mysql.getQuaryRes(&keys, &vals, 7);
    if(affectedRowsn == 0){
        return -1;
    }
    // affectedRowsn ==1;
    outuser.setID(ID); 
    outuser.setpasswd(vals[1]);
    outuser.setname(vals[2]);
    outuser.setfrds(move(json::parse(vals[3]).get<list<IDTp>>()));
    outuser.setvrfyfrds(move(json::parse(vals[4]).get<list<IDTp>>()));
    outuser.setaddfrds(move(json::parse(vals[5]).get<list<IDTp>>()));
    outuser.setmsgsnotread(move(json::parse(vals[6]).get<vector<string>>()));
   return 1;

}
