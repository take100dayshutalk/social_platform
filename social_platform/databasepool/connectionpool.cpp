#include "connectionpool.h"

connectionpool::connectionpool(QString username,QString password,QString database_name ,quint16 port){
    this->database_name=database_name;
    this->username=username;
    this->password=password;
    this->port=port;
}
connectionpool* connectionpool::instance(){
    static connectionpool *ins=new connectionpool("hutalk","650603468","test.db",8077);
    return ins;
}
QSqlDatabase connectionpool::openconnection(){
    static qint64 create_times=1;
    QMutexLocker<QMutex> locker(&mutex);
    while(used.size()>=MAX_CONNECT_COUNT){
        qDebug()<<"连接池已满，需等待空出连接";
        condition.wait(&mutex);
    }
    QString connect_name=QString("connection-%1").arg(create_times);
    create_times++;
    QSqlDatabase db= createconnection(connect_name);
    used.append(db);
    return db;



}
QSqlDatabase connectionpool::createconnection(QString connect_name){
    if(QSqlDatabase::contains(connect_name)){
        qDebug()<<QString("已存在%1连接").arg(connect_name);
        return QSqlDatabase::database(connect_name);
    }
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE",connect_name);
    db.setUserName(username);
    db.setPort(port);
    db.setPassword(password);
    db.setDatabaseName(database_name);
    if(!db.open()){
        qDebug()<<"数据库连接失败";
        qDebug()<<db.lastError();
        return QSqlDatabase();
    }
    qDebug()<<"成功创建连接:"<<connect_name;
    return db;
}
void connectionpool::closeconnect(QString connect_name){
    qDebug()<<connect_name<<"关闭";
    QMutexLocker<QMutex> locker(&mutex);
    for(int i=0;i<used.size();i++)
        if(used.at(i).connectionName()==connect_name){
            used.remove(i);
            QSqlDatabase::removeDatabase(connect_name);
            break;
        }
    condition.wakeOne();

}
void connectionpool::Release_all_connection(){
    for(int i=0;i<used.size();i++)
        QSqlDatabase::removeDatabase(used.at(i).connectionName());
    used.clear();



}
connectionpool::~connectionpool(){
    Release_all_connection();

}
