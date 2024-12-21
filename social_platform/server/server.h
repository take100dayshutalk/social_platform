#ifndef SERVER_CUSTOM
#define SERVER_CUSTOM

#include <QHttpServer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QHttpServerResponder>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QByteArray>
#include <QIODevice>
#include <QFile>
#include <QDir>
#include <QRandomGenerator>
#include <QMap>
#include <QThread>
#include "../databasepool/connectionpool.h"
#include "../SmtpMime/SmtpMime.h"


class customtcpserver:public QObject{
    Q_OBJECT
public:
    customtcpserver(QObject *parent=nullptr);
public slots:
    void sendmessage_(QString sender,QString target,QString message);
    void sendfile_(QString sender,QString target,QByteArray data);
    void StartListen();



private:
    QTcpServer *server;
    QMap<QString,QTcpSocket*> map;
};










class server:public QObject{
    Q_OBJECT
public:
    server(QString ip,quint16 port,bool mode);
    void setkey(QString newkey);
    ~server();
signals:
    void sendmessage(const QString& sender,const QString& target,const QString& message);
private:
    QHttpServer m_server;
    customtcpserver *tcpserver;
    QString ip;
    quint16 port;
    QString key;
    QThread *pthread;
    bool net_mode;

};




#define checkjson_valid \
QJsonParseError error;\
auto rawdata=QJsonDocument::fromJson(request.body(),&error);\
if(error.error!=QJsonParseError::NoError)\
return resultcode::submiterror.tojson();\

#define checkjson_valid_and_account \
QJsonParseError error;\
auto rawdata=QJsonDocument::fromJson(request.body(),&error);\
if(error.error!=QJsonParseError::NoError)\
return resultcode::submiterror.tojson();\
auto obj=rawdata.object();\
if(!obj.contains("user_account")||!obj.contains("user_password")){\
    return resultcode::paramjsonInvalid.tojson();\
    qDebug()<<obj;}\

#define check_token \
auto obj=rawdata.object();\
if(!obj.contains("user_account")) return resultcode::paramjsonInvalid.tojson();\
int rank_op;\
if(!obj.contains("token")) return resultcode::notoken.tojson();\
else{\
    auto token_data=obj["token"].toString();\
    analysistoken token_m(token_data,this->key);\
    if(!token_m.isvalid()){\
        qDebug()<<token_m.accept_key;\
        qDebug()<<token_m.user_account;\
        return resultcode::tokenerror.tojson();\
    }\
    QString OPerator=token_m.user_account;\
    QSqlDatabase db=connectionpool::instance()->openconnection();\
    QSqlQuery query(db);\
    QString order="SELECT user_rank FROM user WHERE user_account='%1';";\
    order=order.arg(OPerator);\
    if(!query.exec(order)){\
        qDebug()<<query.lastError();\
        connectionpool::instance()->closeconnect(db.connectionName());\
        return resultcode::Server_databaseerror.tojson();\
    }\
    if(query.next()){\
        rank_op=query.value("user_rank").toInt();\
        connectionpool::instance()->closeconnect(db.connectionName());  \
    }\
    else{\
        connectionpool::instance()->closeconnect(db.connectionName());\
        return resultcode::second_checkerror.tojson();\
    }\
}\

 \
#endif
