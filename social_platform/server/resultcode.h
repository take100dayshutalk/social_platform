#ifndef RESULTCODE_H
#define RESULTCODE_H
#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
class resultcode{
    struct Code{
        Code(int code,QString codestr,QString message):code(code),codestr(codestr),message(message){}
        int code;
        QString message;
        QString codestr;
        QByteArray tojson() const{
            QJsonObject obj;
            obj.insert("code",code);
            obj.insert("message",message);
            return QJsonDocument(obj).toJson(QJsonDocument::Compact);
        }

    };
public:

#define CODE(name,code,message) inline static Code name{code,#name,message}

    CODE(sucess,2000,"正常访问");
    CODE(paramjsonInvalid,2001,"请求参数数据无效");
    CODE(geterror,2002,"请求get方式错误");
    CODE(posterror,2003,"请求post方式错误");
    CODE(submiterror,2004,"提交的数据格式错误");
    CODE(Server_databaseerror,2005,"服务器的数据库错误");
    CODE(account_or_passworderror,2006,"用户账户或密码错误");
    CODE(disable_login,2007,"暂时禁止登陆");
    CODE(account_exist,2008,"用户已存在");
    CODE(urlqueryerror,2009,"url的参数有误");
    CODE(sucess_operation,2010,"操作成功");
    CODE(server_loaderror,2011,"服务器保存文件失败");
    CODE(server_openfileerror,2012,"服务器文件操作异常");
    CODE(notoken,2013,"没有token验证信息");
    CODE(tokenerror,2014,"token错误或者已经过期");
    CODE(not_allow,2015,"权限不足");
    CODE(second_checkerror,2016,"二次校验错误");
    CODE(not_exist,2017,"对象或目标不存在");
    CODE(emailexist,2018,"邮箱已经被绑定了账户");
    CODE(error,2019,"发生异常");


#undef CODE
    static QByteArray addition(resultcode::Code code, QString key,QVariant values){
        QJsonObject obj;
        obj.insert("code",code.code);
        obj.insert("messqge",code.message);
        obj.insert(key,values.toJsonValue());
        return QJsonDocument(obj).toJson(QJsonDocument::Compact);
    }
};


#endif // RESULTCODE_H
