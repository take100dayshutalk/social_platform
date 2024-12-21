#include <QApplication>
#include "mainwidget.h"
#include "Clientdlg.h"
int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    Logindlg login;
    int re=login.exec();
    if(re!=QDialog::Accepted) return 0;
    mainwidget *w=new mainwidget(g_user_account,g_token);
    w->show();
    QObject::connect(w,&mainwidget::logout,[=]{
        w->hide();
        Logindlg newlogin;
        QSettings set("C:/Users/Administrator/AppData/Local/hutalkcache/soft.ini",QSettings::IniFormat);
        set.setValue("auto_login",false);
        set.setValue("remembered",false);
        qDebug()<<set.value("auto_login").toBool();
        if(newlogin.exec()!=QDialog::Accepted){
            w->deleteLater();
            qApp->deleteLater();
        }
        w->setaccount_and_token(g_user_account,g_token);
        w->show();
    });
    //出bug了


    int resultcode=app.exec();
    w->deleteLater();
    return resultcode;

}
