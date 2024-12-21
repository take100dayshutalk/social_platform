#ifndef EVERYONE_H
#define EVERYONE_H
#include <QString>
#include <QDataStream>
#include <QFile>
#include <QIODevice>
class everyone{
public:
    everyone(QString user_name,QString user_account,QString user_headpicture,QString email,QString phone,QString address,bool is_man){
        this->user_account=user_account;
        this->user_headpicture=user_headpicture;
        this->phone=phone;
        this->email=email;
        this->user_name=user_name;
        this->is_man=is_man;
        this->address=address;
    }
    void writeintofile(QString filename){
        QFile file(filename);
        if(!file.open(QIODevice::Append)){
            file.open(QIODevice::WriteOnly);
        }
        QDataStream in(&file);
        in<<user_account<<user_name<<user_headpicture<<email<<phone<<address<<is_man;
    }
    QString user_name;
    QString user_account;
    QString user_headpicture;
    QString email;
    QString phone;
    QString address;
    bool is_man;
};









#endif // EVERYONE_H
