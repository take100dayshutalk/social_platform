#include "server.h"
#include "resultcode.h"
#include "analysisdata.h"
#include "encipher.h"
#include <QDateTime>
#include <QPair>
#include <QMimeDatabase>

customtcpserver::customtcpserver(QObject *parent):QObject(parent){

    server=new QTcpServer(this);
    connect(server,&QTcpServer::newConnection,server,[=]{
        qDebug()<<"来了连接";
        QTcpSocket *newsocket=server->nextPendingConnection();
        connect(newsocket,&QTcpSocket::readyRead,newsocket,[=]{
            auto data=newsocket->readAll();
            if(data=="-\r\n-CLOSE-\r\n-CONNECTION-\r\n-"){
                newsocket->write("-\r\n-CLOSE-\r\n-CONNECTION-\r\n-");
                newsocket->close();
            }
        });
        connect(newsocket,&QTcpSocket::disconnected,newsocket,[=]{
            QString ip=newsocket->peerAddress().toString();
            qDebug()<<ip<<"断开连接";
            map.remove(ip);
            newsocket->deleteLater();

        });
        auto ip=newsocket->peerAddress().toString();
        map[ip]=newsocket;
    });

}
void customtcpserver::sendmessage_(QString sender,QString target,QString message){
    if(!map.contains(target)){
        qDebug()<<"没有目标连接";
        return ;
    }
    QTcpSocket *socket=map[target];
    QString new_message=sender+"\r\n-MESSAGE-\r\n"+message;
    socket->write(new_message.toUtf8());
}

void customtcpserver::sendfile_(QString sender,QString target,QByteArray data){
    if(!map.contains(target)){
        qDebug()<<"没有目标连接";
        return ;
    }
    QTcpSocket *socket=map[target];
    QByteArray send=sender.toUtf8();
    QByteArray Data=send+"\r\n-FILE-\r\n"+data;
    socket->write(Data);
}
void customtcpserver::StartListen(){
    server->listen(QHostAddress::Any,8080);
}


server::server(QString ip,quint16 port,bool mode){
    this->setkey("11223344");
    this->ip=ip;
    this->port=port;
    this->net_mode=mode;//true的情况下是开启了内网穿透隧道，false情况下是路由端口转发
    m_server.route("/",[](const QHttpServerRequest &request){
        qDebug()<<"根";
        return "根";
    });
    m_server.setMissingHandler([](const QHttpServerRequest &request,QHttpServerResponder &&responder){
        QString message="错误路径，无操作";
        QString web="web";
        QString file_name=web+request.url().path();
        QFile file(file_name);
        if(!file.open(QIODevice::ReadOnly)){
            qDebug()<<file_name<<"文件打不开或请求错误或文件不存在";
            return ;
        }
        qDebug()<<file_name;
        QByteArray data=file.readAll();
        file.close();
        responder.writeStatusLine();
        responder.writeHeader("Content-Type","text/css");
        responder.writeHeader("Content-Length",QByteArray::number(data.size()));
        responder.writeBody(data);

    });

    m_server.route("/person",[](const QHttpServerRequest &request,QHttpServerResponder &&responder){
        QFile file("web/person.html");
        if(!file.open(QIODevice::ReadOnly)){
            qDebug()<<"person.html文件打不开";
            return ;
        }
        QByteArray data=file.readAll();
        file.close();
        responder.writeStatusLine();
        responder.writeHeader("Content-Type", "text/html; charset=UTF-8");
        responder.writeHeader("Content-Length",QByteArray::number(data.size()));
        responder.writeBody(data);

    });

    m_server.route("/api/version",[=](const QHttpServerRequest& request){
        QJsonObject ver_obj;
        ver_obj.insert("version","1.1");
        ver_obj.insert("name","usermanager-server");
        ver_obj.insert("date",QDateTime::currentDateTime().toString());
        ver_obj.insert("copyright","hutalk write");
        QJsonDocument ver_do(ver_obj);
        QString ip;
        if(net_mode){
            for(auto it=request.headers().begin();it!=request.headers().end();it++)
                if(it->first=="X-Real-Ip"){
                    qDebug()<<"测试"<<it->second;
                    ip=QString::fromUtf8(it->second);
                    qDebug()<<"测试"<<ip;
                    break;
                }
        }
        return ver_do.toJson(QJsonDocument::Compact);
    });

    m_server.route("/api/login",[=](const QHttpServerRequest &request){
        qDebug()<<"有人登陆";
        if(request.method()==QHttpServerRequest::Method::Get) return resultcode::geterror.tojson();

        checkjson_valid_and_account

        auto db=connectionpool::instance()->openconnection();
        QSqlQuery query(db);
        query.prepare("SELECT * FROM user WHERE user_account=? AND user_password=?;");
        query.bindValue(0,obj["user_account"].toString());
        query.bindValue(1,obj["user_password"].toString());
        if(!query.exec()){
            qDebug()<<query.lastError();
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        if(!query.next()){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::account_or_passworderror.tojson();
        }
        bool is_disable=query.value("is_disable").toBool();
        QString user_account=query.value("user_account").toString();
        connectionpool::instance()->closeconnect(db.connectionName());
        if(!is_disable) return resultcode::disable_login.tojson();
        else{
            encipher cookie(user_account,this->key);
            analysistoken token_m(cookie.data,this->key);
            return resultcode::addition(resultcode::sucess,"token",cookie.data);
        }
    });
    m_server.route("/api/register",[=](const QHttpServerRequest& request){
        if(request.method()==QHttpServerRequest::Method::Get) return resultcode::geterror.tojson();

        checkjson_valid_and_account
        if(!obj.contains("user_rank")|!obj.contains("email")|!obj.contains("phone")|!obj.contains("gender"))    return resultcode::paramjsonInvalid.tojson();
        if(obj["user_rank"].toInt()==0||obj["user_rank"].toInt()==1) return resultcode::not_allow.tojson();
        QSqlDatabase db=connectionpool::instance()->openconnection();
        QSqlQuery query(db);
        QString order("INSERT INTO user(user_account,user_password,create_time,user_rank) VALUES('%1','%2','%3',%4);");
        QDateTime ti=QDateTime::currentDateTime();
        QString time=ti.toString();
        order=order.arg(obj["user_account"].toString()).arg(obj["user_password"].toString()).arg(time).arg(obj["user_rank"].toInt());
        qDebug()<<order;
        if(!query.exec(order)){
            qDebug()<<query.lastError();
            QString errorstr=query.lastError().databaseText();
            if(errorstr.contains("UNIQUE")){
                connectionpool::instance()->closeconnect(db.connectionName());
                return resultcode::account_exist.tojson();
            }
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        QString ip;
        if(net_mode){
            for(auto it=request.headers().begin();it!=request.headers().end();it++)
                if(it->first=="X-Real-Ip"){
                    ip=QString::fromUtf8(it->second);
                    break;
                }
        }
        else ip=request.remoteAddress().toString();
        QString order2("INSERT INTO message(user_account,email,phone,gender,frequent_ip) VALUES('%1','%2','%3','%4','%5');");
        order2=order2.arg(obj["user_account"].toString()).arg(obj["email"].toString()).arg(obj["phone"].toString()).arg(obj["gender"].toString()).arg(ip);
        if(!query.exec(order2)){
            qDebug()<<query.lastError();
            QString errorstr=query.lastError().databaseText();
            if(errorstr.contains("UNIQUE")){
                query.exec(QString("DELETE FROM user WHERE user_account='%1';").arg(obj["user_account"].toString()));
                connectionpool::instance()->closeconnect(db.connectionName());
                return resultcode::emailexist.tojson();
            }
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        connectionpool::instance()->closeconnect(db.connectionName());
        return resultcode::sucess.tojson();
    });
    m_server.route("/api/userlist",[](const QHttpServerRequest &request){
        if(request.method()==QHttpServerRequest::Method::Post) return resultcode::posterror.tojson();
        QUrlQuery urlquery= request.query();
        int page= urlquery.queryItemValue("page").toInt();
        int pagesize=urlquery.queryItemValue("pagesize").toInt();
        if(page<0||pagesize<=0) return resultcode::urlqueryerror.tojson();
        QSqlDatabase db=connectionpool::instance()->openconnection();
        QSqlQuery query(db);
        if(!query.exec("SELECT * FROM user;")){
            qDebug()<<query.lastError();
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        QSqlRecord record=query.record();
        int count=record.count();
        QJsonArray arry;
        QJsonObject obj;
        while(query.next()){
            QJsonObject temp;
            for(int i=0;i<count;i++)
                temp.insert(record.fieldName(i),query.value(i).toString());
            arry.append(temp);
        }
        obj.insert("code",2000);
        obj.insert("message","正常访问");
        obj.insert("list",arry);
        connectionpool::instance()->closeconnect(db.connectionName());
        return QJsonDocument(obj).toJson(QJsonDocument::Compact);
    });
    m_server.route("/api/userdisable",[=](const QHttpServerRequest &request){
        if(request.method()==QHttpServerRequest::Method::Get) return resultcode::geterror.tojson();
        checkjson_valid
        check_token
        QSqlDatabase db=connectionpool::instance()->openconnection();
        QSqlQuery query(db);
        QString order="UPDATE user SET is_disable=0 WHERE user_account='%1';";
        QString order1="SELECT user_rank FROM user WHERE user_account='%1';";
        order=order.arg(obj["user_account"].toString());
        order1=order1.arg(obj["user_account"].toString());
        if(!query.exec(order1)){
            qDebug()<<query.lastError();
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        if(!query.next()){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::not_exist.tojson();
        }
        int rank_target=query.value("user_rank").toInt();
        if(rank_target<=rank_op||rank_op>=2){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::not_allow.tojson();
        }
        if(!query.exec(order)){
            qDebug()<<query.lastError();
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        connectionpool::instance()->closeconnect(db.connectionName());
        return resultcode::sucess_operation.tojson();

    });
    m_server.route("/api/userenable",[=](const QHttpServerRequest& request){
        if(request.method()==QHttpServerRequest::Method::Get) return resultcode::geterror.tojson();
        checkjson_valid
        check_token
        QSqlDatabase db=connectionpool::instance()->openconnection();
        QSqlQuery query(db);
        QString order="UPDATE user SET is_disable=1 WHERE user_account='%1';";
        QString order1="SELECT user_rank FROM user WHERE user_account='%1';";
        order=order.arg(obj["user_account"].toString());
        order1=order1.arg(obj["user_account"].toString());
        if(!query.exec(order1)){
            qDebug()<<query.lastError();
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        if(!query.next()){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::not_exist.tojson();
        }
        int rank_target=query.value("user_rank").toInt();
        if(rank_target<=rank_op||rank_op>=2){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::not_allow.tojson();
        }
        if(!query.exec(order)){
            qDebug()<<query.lastError();
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        connectionpool::instance()->closeconnect(db.connectionName());
        return resultcode::sucess_operation.tojson();
    });
    m_server.route("/api/userdel",[=](const QHttpServerRequest &request){
        if(request.method()==QHttpServerRequest::Method::Get) return resultcode::geterror.tojson();
        checkjson_valid
        check_token
        if(rank_op!=0) return resultcode::not_allow.tojson();
        if(obj["user_account"].toString()=="root") return resultcode::addition(resultcode::not_allow,"warning","不允许删除root用户");
        QString order="DELETE FROM user WHERE user_account='%1';";
        order=order.arg(obj["user_account"].toString());
        QSqlDatabase db=connectionpool::instance()->openconnection();
        QSqlQuery query(db);
        if(!query.exec(order)){
            qDebug()<<query.lastError();
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        connectionpool::instance()->closeconnect(db.connectionName());
        return resultcode::sucess_operation.tojson();
    });
    m_server.route("/api/headpicture",[](const QHttpServerRequest &request,QHttpServerResponder &&responder){
        qDebug()<<"收到请求";
        qDebug()<<request.method();
        QByteArray rawdata=request.body();
        //qDebug()<<rawdata;
        if(request.method()==QHttpServerRequest::Method::Post){
            analysisdata data(rawdata);
            qDebug()<<data.type();
            QString path="D:/mixedarea/";
            QDir catalog;
            if(!catalog.exists(path)){
                qDebug()<<"路径不存在，已创建路径";
                catalog.mkpath(path);
             }
              QString filename=path+data.Filename();
              QFile file(filename);
              if(!file.open(QIODevice::WriteOnly)){
              qDebug()<<file.error();
                  responder.write(resultcode::server_loaderror.tojson(),"application/json");
        }
            file.write(data.data());
             file.close();
            responder.write( resultcode::sucess.tojson(),"application/json");
        }
        else{
            QFile file("D:/mixedarea/test.mp4");
            if(!file.open(QIODevice::ReadOnly)){
                qDebug()<<file.error();
                responder.write(resultcode::server_openfileerror.tojson(),"application/json");
                return ;
            }
            QByteArray data2= file.readAll();
            file.close();
            responder.writeStatusLine();
            responder.writeHeader("Content-Type","video/mp4");
            responder.writeHeader("Content-Length",QByteArray::number(data2.size()));
            responder.writeBody(data2);
        }


    });
    m_server.route("/api/emailverify",[=](const QHttpServerRequest &request){
        qDebug()<<"发送邮箱验证码";
        if(request.method()!=QHttpServerRequest::Method::Post) return resultcode::geterror.tojson();
        checkjson_valid
         auto obj =rawdata.object();
        if(!obj.contains("email")) return resultcode::paramjsonInvalid.tojson();
        MimeMessage message;
        QString sender="3459352294@qq.com";
        EmailAddress send(sender,"hutalk");
        message.setSender(send);
        EmailAddress to(obj["email"].toString(),"YOU");
        message.addRecipient(to);
        message.setSubject("邮箱验证");
        MimeText text;
        auto code=QRandomGenerator::global()->bounded(100000,999999);
        text.setText(QString("你的验证码是%1").arg(code));
        message.addPart(&text);
        SmtpClient smtp("smtp.qq.com", 465, SmtpClient::SslConnection);
        smtp.connectToHost();
        if(!smtp.waitForReadyConnected()) qDebug() << "Failed to connect to host!";
        smtp.login(sender,"wqbkutjcqdckcjai");
        if(!smtp.waitForAuthenticated()) qDebug()<<"faild to login";
        smtp.sendMail(message);
        if(!smtp.waitForMailSent()) qDebug()<<"发送失败";
        smtp.quit();
        return resultcode::addition(resultcode::sucess,"verifycode",code);
    });
    m_server.route("/message_transmit",[=](const QHttpServerRequest &request){
        if(request.method()==QHttpServerRequest::Method::Get) return resultcode::geterror.tojson();
        checkjson_valid
        check_token
        if(!obj.contains("message")) return resultcode::paramjsonInvalid.tojson();
        if(!obj.contains("friend")) return resultcode::paramjsonInvalid.tojson();
        QString Message=obj["message"].toString();
        QSqlDatabase db=connectionpool::instance()->openconnection();
        QString order=QString("SELECT frequent_ip FROM message WHERE user_account='%1';").arg(obj["friend"].toString());
        QSqlQuery query(db);
        if(!query.exec(order)){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        if(!query.next()){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::addition(resultcode::error,"reason","发生异常");
        }
        auto ip=query.value("frequent_ip").toString();
        emit sendmessage(obj["user_account"].toString(),ip,Message);
        connectionpool::instance()->closeconnect(db.connectionName());
        return resultcode::sucess.tojson();

    });
    m_server.route("/api/add_friend",[=](const QHttpServerRequest &request){
        if(request.method()==QHttpServerRequest::Method::Get) return resultcode::geterror.tojson();
        checkjson_valid
        check_token
        if(!obj.contains("friend_account")) return resultcode::paramjsonInvalid.tojson();
        QString friend_account=obj["friend_account"].toString();
        QString myself_account=obj["user_account"].toString();
        QSqlDatabase db=connectionpool::instance()->openconnection();
        QSqlQuery query(db);
        QString order=QString("SELECT is_disable FROM user WHERE user_account='%1';").arg(friend_account);
        if(!query.exec(order)){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        if(!query.next()){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::not_exist.tojson();
        }
        if(!query.value("is_disable").toBool()){
             connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::addition(resultcode::disable_login,"reason","该用户暂被封禁");
        }
        QString oreder1=QString("INSERT INTO friendship(user_account1,user_account2) VALUES('%1','%2');").arg(myself_account).arg(friend_account);
        QString oreder2=QString("INSERT INTO friendship(user_account1,user_account2) VALUES('%1','%2');").arg(friend_account).arg(myself_account);
        QString oreder3=QString("SELECT nick FROM message WHERE user_account='%1';").arg(friend_account);
        if(!query.exec(oreder1)){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        if(!query.exec(oreder2)){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        if(!query.exec(oreder3)){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        if(!query.next()){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::addition(resultcode::Server_databaseerror,"reasion","数据库查找发生异常");
        }
        QString nick=query.value("nick").toString();
        connectionpool::instance()->closeconnect(db.connectionName());
        return resultcode::addition(resultcode::sucess,"nick",nick);


    });
    m_server.route("/api/friendlist",[=](const QHttpServerRequest &request){
        checkjson_valid
        check_token
        QString order1=QString("SELECT user_account2 FROM friendship WHERE user_account1='%1';").arg(obj["user_account"].toString());
        QString order2="SELECT nick FROM message WHERE user_account='%1';";
        QSqlDatabase db= connectionpool::instance()->openconnection();
        QSqlQuery query(db);
        QSqlQuery query1(db);
        if(!query.exec(order1)){
            connectionpool::instance()->closeconnect(db.connectionName());
            return resultcode::Server_databaseerror.tojson();
        }
        QJsonArray nick;
        QJsonArray account;
        QJsonObject friendlist;
        while(query.next()){
            QString temp=query.value("user_account2").toString();
            account.append(temp);
            QString order3=order2.arg(temp);
            query1.exec(order3);
            query1.next();
            QString temp1=query1.value("nick").toString();
            nick.append(temp1);
        }
        friendlist.insert("user_account",account);
        friendlist.insert("nick",nick);
        friendlist.insert("code",2000);
        connectionpool::instance()->closeconnect(db.connectionName());
        return QJsonDocument(friendlist).toJson(QJsonDocument::Compact);

    });

    pthread=new QThread(this);
    tcpserver=new customtcpserver;
    tcpserver->moveToThread(pthread);
    connect(pthread,&QThread::started,tcpserver,&customtcpserver::StartListen);
    pthread->start();
    m_server.listen(QHostAddress(ip),port);
}
void server::setkey(QString newkey){
    key=newkey;
}
server::~server(){
    pthread->quit();
    pthread->wait();

}
