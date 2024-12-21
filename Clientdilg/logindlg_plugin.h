#ifndef LOGINDLG_PLUGIN_H
#define LOGINDLG_PLUGIN_H
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QEvent>
#include <QCloseEvent>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
class login_plugin_message:public QDialog{
    Q_OBJECT
public:
    login_plugin_message(QWidget *parent=nullptr);
    void paintEvent(QPaintEvent *e) override;
signals:
    void submit_message(const QString &email,const QString &phone,bool is_man);
    void submit_seconde_code(const QString &code);
    void get_seconde_code();
private:
    void init();
    QRadioButton *man;
    QRadioButton *woman;
    QLineEdit *email;
    QLineEdit *phone;
    QPushButton *confirm;
};
class login_plugin_verify:public QDialog{
    Q_OBJECT
public:
     login_plugin_verify(QWidget *parent=nullptr);
     void paintEvent(QPaintEvent *e) override;
     void closeEvent(QCloseEvent *e) override;
signals:
     void submit_code(const QString &code);
     void get_code();



private:
    QLineEdit *code;
    QPushButton *confirm;
    QPushButton *resend;
    QTimer *limit60;
    int timeout;
    void init();


};

#endif // LOGINDLG_PLUGIN_H
