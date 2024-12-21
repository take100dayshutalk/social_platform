#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVector>
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>
#define MAX_CONNECT_COUNT 20
class connectionpool{

public:
    static connectionpool *instance();
    connectionpool(QString username,QString password,QString database_name,quint16 port);
    ~connectionpool();
    void closeconnect(QString connect_name);
    QSqlDatabase openconnection();

protected:
    QSqlDatabase createconnection(QString connect_name);
    void Release_all_connection();

private:
    QString username;
    QString database_name;
    QString password;
    quint16 port;
    QMutex mutex;
    QWaitCondition condition;
    QVector<QSqlDatabase> used;

};
