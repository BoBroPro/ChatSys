#ifndef MYSQLUTILITY_HPP
#define MYSQLUTILITY_HPP
#include"mysql/mysql.h"
#include<string>
#include<iostream>
#include<vector>

class CMySQL{
public:
    CMySQL(std::string ipaddr, PortTp port):res(nullptr){
        mysql_init(&mysqlconn);
        if(!mysql_real_connect(&mysqlconn, ipaddr.c_str(), "root","123456", "mydb", port, NULL, 0)){
            std::cout << "failed connection"<<mysql_error(&mysqlconn) << std::endl;
        }
    }
    CMySQL(const CMySQL&) = delete;
    CMySQL(CMySQL&&) = delete; 
    CMySQL& operator =(const CMySQL&)=delete;
    CMySQL& operator=(CMySQL&&) = delete;
    ~CMySQL(){
        if(res){
            mysql_free_result(res);
        }
        mysql_close(&mysqlconn);
    }

    int quary(std::string statement){ //not for SELECT * FROM tablename.
        if(mysql_query(&mysqlconn, statement.c_str()) != 0){
            std::cout << "query failure:\n"<<std::endl;
            std::cout << statement.c_str()<<std::endl;

            return -1;
        }
        return 0;
    }

    int getQuaryRes(std::vector<std::string>* pkeys, std::vector<std::string>* pvals, int n){
        if(!(res = mysql_store_result(&mysqlconn))){
            std::cout << "no data" <<std::endl;
            return -1;
        }

        int cntfileds = 0;
        cntfileds = mysql_field_count(&mysqlconn);
        MYSQL_ROW rown = mysql_fetch_row(res);
        //std::cout << "filedcnt:" << cntfileds<< std::endl;
        int affectedRowsn = mysql_affected_rows(&mysqlconn); 
        std::cout << "affectedRows:" << affectedRowsn<< std::endl;
        if(affectedRowsn == 0){
            return 0;
        }

        if(pkeys == nullptr || pvals == nullptr){
            return affectedRowsn;    
        }

        for(int i = 0; i < cntfileds && i < n; ++i){
            (*pkeys)[i]= mysql_fetch_field(res)->name;
            if(rown[i] == NULL){
            //    std::cout <<std::endl;
                (*pvals)[i] = "";
                continue;
            }
            else{
                (*pvals)[i] = rown[i];
            }
           // std::cout << rown[i]  <<std::endl;
        }
        return affectedRowsn;
    }


private:
    MYSQL mysqlconn;
    MYSQL_RES* res;
};




#endif