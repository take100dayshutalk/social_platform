#ifndef ANALYSISDATA_H
#define ANALYSISDATA_H
#include <QString>
#include <QByteArray>
#include <QDebug>
class analysisdata{
public:
    analysisdata(QByteArray raw);
    QString type();
    QByteArray data();
    QString Filename();
private:
    QByteArray content_type;
    QByteArray m_data;
    QByteArray file_name;
};



class analysistoken{
public:
    analysistoken(QString raw,QString key);
    bool isvalid();
    QString user_account;
    QString accept_key;
private:
    bool is_valid;


};

#endif // ANALYSISDATA_H
