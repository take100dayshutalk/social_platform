#include "encipher.h"


encipher::encipher(QString str,QString key){
    QString temp=str+"\r--------\n"+key;
    DATA=temp.toUtf8();
    OP();
}

void encipher::OP(){
    unsigned char temp,b1,b2;
    for(auto it=DATA.begin();it!=DATA.end();it++){
        temp=*it;
        b1=temp<<4;
        b2=temp>>4;
        *it=b1|b2;
    }
    data=QString::fromUtf8(DATA.toBase64());
}
