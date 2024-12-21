#include "analysisdata.h"
analysisdata::analysisdata(QByteArray raw){
    auto pos=raw.indexOf("\r\n\r\n");
    auto header=raw.mid(0,pos);
    auto header_list=header.split('\r');
    content_type=header_list.at(2).trimmed();
    auto bounder=header_list.at(0).trimmed();
    auto pos1=raw.lastIndexOf(bounder);
    m_data=raw.mid(pos+4,pos1-pos-6);
    auto temp=header_list.at(1).trimmed();
    auto pos2=temp.indexOf("filename=");
    file_name=temp.mid(pos2+10);

}
QString analysisdata::type(){
    return QString::fromUtf8(content_type);
}
QByteArray analysisdata::data(){
    return m_data;
}
QString analysisdata::Filename(){

    QString filename=QString::fromUtf8(file_name);
    QString newname=filename.remove('\"');
    return newname;
}




analysistoken::analysistoken(QString raw,QString key){
    unsigned char temp,b1,b2;
    auto datatemp=raw.toUtf8();
    auto data=QByteArray::fromBase64(datatemp);
    for(auto it=data.begin();it!=data.end();it++){
        temp=*it;
        b1=temp<<4;
        b2=temp>>4;
        *it=b1|b2;
    }
    QString product=QString::fromUtf8(data);
    qDebug()<<product;
    QString midder("\r--------\n");
    auto pos=data.indexOf("\r--------\n");
    auto data1=data.mid(0,pos);
    auto data2=data.mid(pos+midder.length());
    user_account=QString::fromUtf8(data1);
    accept_key=QString::fromUtf8(data2);
    if(key==accept_key) is_valid=true;
    else is_valid=false;
}
bool analysistoken::isvalid(){
    return is_valid;
}


