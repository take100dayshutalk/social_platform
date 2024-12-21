#ifndef CLIENTDLG_H
#define CLIENTDLG_H
#include <QWidget>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QCloseEvent>
#include <QPushButton>
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QStaticText>
#include <QPen>
#include <QGraphicsDropShadowEffect>
#include <QImage>
#include <QStackedWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDialog>
#include <QRadioButton>
#include "logindlg_plugin.h"
extern QString g_token;
extern QString g_user_account;
class Logindlg:public QDialog{
    Q_OBJECT
public:
    Logindlg(QWidget *parent=nullptr);
    ~Logindlg();
    bool eventFilter(QObject *watch,QEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void init_childwidget();
signals:
    void verify_sucess();
private:
    bool is_trag;
    QString hostpriname;
    QString email,phone,code;
    bool is_man;
    QRect m_leftrect,m_rightrect,m_middlerect;
    QPushButton *bu_close,*bu_confirm,*bu_confirm1,*bu_towidget,*backlogin;
    QLineEdit *account,*password,*account1,*password1,*password2;
    QStaticText *lefttext;
    QStackedWidget *stackwidget;
    QComboBox *jobchose;
    QWidget *login_widget,*sign_up_widget;
    QCheckBox *automatic_login,*remembered;
    QNetworkAccessManager *networkmanager;

};





#endif // CLIENTDLG_H
