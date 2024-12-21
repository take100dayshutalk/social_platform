#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include <QWidget>
#include <QMainWindow>
#include <QByteArray>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSystemTrayIcon>
#include <QAction>
#include <QStackedWidget>
#include <QToolBar>
#include <QToolButton>
#include <QList>
#include <QIcon>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QFile>
#include <QIODevice>
#include <QEvent>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QPaintEvent>
#include <QSettings>
#include <QMenuBar>
#include "talkwidget.h"
#include "settingwidget.h"
#include "friendwidget.h"
class container;
class mainwidget:public QMainWindow{
    Q_OBJECT
public:
    mainwidget(QString user_account,QString token,QWidget *parent=nullptr);
    bool test();
    void mouseMoveEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *event) override;
    void setaccount_and_token(QString user_account,QString token){
        this->user_account=user_account;
        this->token=token;
    }
signals:
    void logout();
private:
    void init_action();
    void init_();
    QString token;
    QString user_account;
    QWidget *center;
    QStackedWidget *stackwidget;
    QWidget *containerwidget;
    //QNetworkAccessManager *networkmanager;
    //QAction *setting,*friend_list,*friend_circle,*home,*talk;
    QToolButton *right_tool[5];
    QPropertyAnimation *animation;
    bool is_hiden_container;

    talkwidget *TalkWidget;
    settingwidget *setting;
    friendwidget *yourfriend;
    QMenuBar *menubar;
};
class container:public QWidget{
    Q_OBJECT
public:
    container(QWidget *parent=nullptr);
protected:
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;

};

#endif // MAINWIDGET_H

