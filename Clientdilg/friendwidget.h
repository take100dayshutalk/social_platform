#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QListWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QList>
#include <QMenu>
#include <QListWidgetItem>
#include <QAction>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
namespace friendspace{
struct node{
    QString user_account;
    QString nick;
};

}
class friendwidget:public QWidget{
    Q_OBJECT
public:
    friendwidget(QString user_account,QString token,QString hostpri,QWidget *parent=nullptr);
    ~friendwidget();

private:
    void init_();
    QString self_account;
    QString token;
    QString hostpri;
    QLineEdit *search_user;
    QListWidget *friendlist;
    QPushButton *confirm;
    QNetworkAccessManager *manager;
    QList<friendspace::node*> friends;
    QMenu *clickup_menu;
};
#endif // FRIENDWIDGET_H
