#include "Clientdlg.h"
QString g_token;
QString g_user_account;
Logindlg::Logindlg(QWidget *parent):QDialog(parent){
    setWindowFlags(windowFlags()|Qt::WindowType::FramelessWindowHint);
    networkmanager=new QNetworkAccessManager(this);
    is_trag=false;
    setMouseTracking(true);
    installEventFilter(this);
    resize(740,340);
    setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground);
    m_rightrect={5,25,730,290};
    m_leftrect={5,5,200,330};
    m_middlerect={0,5,400,330};
    QFile file(":/style.css");
    file.open(QIODevice::ReadOnly);
    setStyleSheet(file.readAll());
    file.close();
    bu_close=new QPushButton(this);
    connect(bu_close,&QPushButton::clicked,this,[this]{close();});
    bu_close->resize(32,32);
    bu_close->move(700,25);
    bu_close->setObjectName("bu_close");
    bu_close->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    lefttext=new QStaticText("Welcome");
    auto effect=new QGraphicsDropShadowEffect(this);
    effect->setOffset(0);
    effect->setColor(qRgb(126,112,150));
    effect->setBlurRadius(10);
    setGraphicsEffect(effect);
    hostpriname="http://hutalk.vs4.tunnelfrp.com";
    QDir dir("C:/Users/Administrator/AppData/Local/hutalkcache");
    if(!dir.exists()){
        dir.mkdir("C:/Users/Administrator/AppData/Local/hutalkcache");
        dir.mkdir("C:/Users/Administrator/AppData/Local/hutalkcache/file_cache");
    }
    else qDebug()<<"文件夹存在";
    QSettings set("C:/Users/Administrator/AppData/Local/hutalkcache/soft.ini",QSettings::IniFormat);
    QFile fileuseless("is_init");
    if(!fileuseless.open(QIODevice::ReadOnly)){
        set.setValue("is_init",false);
        set.setValue("is_init_second",false);
        set.setValue("auto_login",false);
        set.setValue("remembered",false);
        fileuseless.open(QIODevice::WriteOnly);
        fileuseless.close();
    }
    fileuseless.close();
    init_childwidget();
    QSettings setuser("C:/Users/Administrator/AppData/Local/hutalkcache/user.ini",QSettings::IniFormat);
    if(set.value("remembered").toBool()){
        account->setText(setuser.value("user_account").toString());
        password->setText(setuser.value("password").toString());
    }
    if(set.value("auto_login").toBool()){
        account->setText(setuser.value("user_account").toString());
        password->setText(setuser.value("password").toString());
        emit bu_confirm->clicked();
    }
    QFile file_timeout("C:/Users/Administrator/AppData/Local/hutalkcache/timeout");
    if(!file_timeout.open(QIODevice::ReadOnly)){
        file_timeout.open(QIODevice::WriteOnly);
        QDataStream in(&file_timeout);
        int time_left=0;
        in<<time_left;
        file_timeout.close();
    }
    else file_timeout.close();
    //http://hutalk.vs4.tunnelfrp.com

}
Logindlg::~Logindlg(){
    delete lefttext;
    networkmanager->deleteLater();
}
void Logindlg::init_childwidget(){
    stackwidget=new QStackedWidget(this);
    stackwidget->setGeometry(410,60,320,240);
    login_widget=new QWidget;
    sign_up_widget=new QWidget;
    QVBoxLayout *login_vbox=new QVBoxLayout(login_widget);
    QVBoxLayout *signup_vbox=new QVBoxLayout(sign_up_widget);
    QHBoxLayout *login_hbox=new QHBoxLayout;
    automatic_login=new QCheckBox("自动登陆");
    remembered=new QCheckBox("记住密码");
    automatic_login->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    remembered->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    account=new QLineEdit;
    account->setPlaceholderText("账户");
    password=new QLineEdit;
    password->setEchoMode(QLineEdit::EchoMode::Password);
    password->setPlaceholderText("密码");

    account1=new QLineEdit;
    account1->setPlaceholderText("账户");
    password1=new QLineEdit;
    password1->setPlaceholderText("密码");
    password1->setEchoMode(QLineEdit::EchoMode::Password);
    password2=new QLineEdit;
    password2->setPlaceholderText("再次输入相同密码");
    password2->setEchoMode(QLineEdit::EchoMode::Password);

    bu_confirm=new QPushButton("登陆");
    bu_confirm->setObjectName("Login");
    bu_confirm1=new QPushButton("注册");
    bu_confirm1->setObjectName("Register");
    bu_towidget=new QPushButton("没有账户？点击此处跳转注册");
    bu_towidget->setObjectName("Towidget");
    backlogin=new QPushButton("返回");
    backlogin->setObjectName("Backlogin");
    jobchose=new QComboBox;
    jobchose->addItem(QIcon(":/teacher.png"),"老师");
    jobchose->addItem(QIcon(":/student.png"),"学生");
    jobchose->setCurrentIndex(1);

    QSettings sets("C:/Users/Administrator/AppData/Local/hutalkcache/soft.ini",QSettings::IniFormat);
    remembered->setChecked(sets.value("remembered").toBool());
    automatic_login->setChecked(sets.value("auto_login").toBool());

    QRegularExpression expression1("[A-Za-z0-9]+");
    QRegularExpression expression2("\\d+");
    QRegularExpressionValidator *validator1=new QRegularExpressionValidator(expression1,this);
    QRegularExpressionValidator *validator2=new QRegularExpressionValidator(expression2,this);
    password->setValidator(validator1);
    password1->setValidator(validator1);
    password2->setValidator(validator1);
    account->setValidator(validator2);
    account1->setValidator(validator2);
    connect(bu_towidget,&QPushButton::clicked,[this]{this->stackwidget->setCurrentIndex(1);});
    connect(backlogin,&QPushButton::clicked,[this]{this->stackwidget->setCurrentIndex(0);});
    connect(bu_confirm,&QPushButton::clicked,[=]{
        if(password->text().isEmpty()||account->text().isEmpty()){
            QMessageBox::information(this,"Tips","请完整填写登陆账户和密码");
            return ;
        }
        QNetworkRequest request(QUrl(hostpriname+"/api/login"));
        QJsonObject obj;
        obj.insert("user_account",account->text());
        obj.insert("user_password",password->text());
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        auto reply=networkmanager->post(request,QJsonDocument(obj).toJson(QJsonDocument::Compact));
        connect(reply,&QNetworkReply::errorOccurred,[=]{
            qDebug()<<"reply error";
            qDebug()<<reply->errorString();
        });
        connect(reply,&QNetworkReply::finished,[=]{
            QByteArray data=reply->readAll();
            auto acc= QJsonDocument::fromJson(data);
            auto accept_data=acc.object();
            if(accept_data["code"].toInt()!=2000){
                QMessageBox::information(this,"Tips",accept_data["message"].toString());
                reply->deleteLater();
                return ;
            }
            g_token=accept_data["token"].toString();
            g_user_account=account->text();
            QSettings set_("C:/Users/Administrator/AppData/Local/hutalkcache/soft.ini",QSettings::IniFormat);
            QSettings setuser("C:/Users/Administrator/AppData/Local/hutalkcache/user.ini",QSettings::IniFormat);
            setuser.setValue("user_account",account->text());
            setuser.setValue("password",password->text());
            set_.setValue("is_init",true);
            set_.setValue("auto_login",automatic_login->isChecked());
            set_.setValue("remembered",remembered->isChecked());
            this->accept();
            reply->deleteLater();
        });

    });
    connect(bu_confirm1,&QPushButton::clicked,[this]{
        if(password1->text().isEmpty()||password2->text().isEmpty()||account1->text().isEmpty()){
            QMessageBox::information(this,"Tips","请完整填写注册账户和注册密码");
            return ;
        }
        if(password1->text()!=password2->text()){
            QMessageBox::information(this,"Tips","两次设置的密码不一致");
            return ;
        }
        if(password1->text().length()>=60||password1->text().length()<8){
            QMessageBox::information(this,"Tips","密码太短不安全或者密码实在太长超出限制");
            return ;
        }
        if(account->text().length()>=31){
            QMessageBox::information(this,"Tips","账户信息太长");
            return ;
        }
        login_plugin_message ssw(this);
        connect(&ssw,&login_plugin_message::submit_message,this,[=](const QString &Email,const QString &Phone,bool is_man){
            email=Email;
            phone=Phone;
            this->is_man=is_man;
        });
        connect(&ssw,&login_plugin_message::submit_seconde_code,this,[=](const QString &code){
            if(code==this->code){
                qDebug()<<code;
                emit verify_sucess();
            }
        });

        connect(&ssw,&login_plugin_message::get_seconde_code,this,[=]{
            qDebug()<<"测试----";

            QNetworkRequest REquest(QUrl(hostpriname+"/api/emailverify"));
            QJsonObject emailobj;
            emailobj.insert("email",this->email);
            REquest.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
            auto REply=networkmanager->post(REquest,QJsonDocument(emailobj).toJson(QJsonDocument::Compact));
            connect(REply,&QNetworkReply::errorOccurred,[=]{
                qDebug()<<"reply error";
                qDebug()<<REply->errorString();

            });
            connect(REply,&QNetworkReply::finished,[=]{
                QByteArray rawdata=REply->readAll();
                auto obj=QJsonDocument::fromJson(rawdata);
                if(obj["code"].toInt()!=2000){
                     QMessageBox::information(this,"Tips",obj["message"].toString());
                    REply->deleteLater();
                     return ;
                }
                int CODE=obj["verifycode"].toInt();
                this->code=QString::number(CODE);
            });

        });
        if(ssw.exec()==QDialog::Rejected) return ;



        QNetworkRequest request(QUrl(hostpriname+"/api/register"));
        QJsonObject obj;
        obj.insert("user_account",account1->text());
        obj.insert("user_password",password1->text());
        if(jobchose->currentIndex()==1) obj.insert("user_rank",3);
        else obj.insert("user_rank",2);
        obj.insert("email",this->email);
        obj.insert("phone",this->phone);
        if(is_man) obj.insert("gender","M");
        else obj.insert("gender","W");
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        auto reply= networkmanager->post(request,QJsonDocument(obj).toJson(QJsonDocument::Compact));
        connect(reply,&QNetworkReply::errorOccurred,[=]{
            qDebug()<<"reply error";
            qDebug()<<reply->errorString();
        });
        connect(reply,&QNetworkReply::finished,[=]{
            auto data=reply->readAll();
            auto acc= QJsonDocument::fromJson(data);
            auto accept_data=acc.object();
            if(accept_data["code"].toInt()!=2000){
                QMessageBox::information(this,"Tips",accept_data["message"].toString());
                reply->deleteLater();
                return ;
            }
            account1->setText(nullptr);
            password1->setText(nullptr);
            password2->setText(nullptr);
            this->stackwidget->setCurrentIndex(0);
            reply->deleteLater();
        });


    });


    login_vbox->addWidget(account);
    login_vbox->addWidget(password);
    login_hbox->addWidget(automatic_login);
    login_hbox->addWidget(remembered);
    login_vbox->addLayout(login_hbox);
    login_vbox->addWidget(bu_confirm);
    login_vbox->addWidget(bu_towidget);

    signup_vbox->addWidget(account1);
    signup_vbox->addWidget(password1);
    signup_vbox->addWidget(password2);
    QHBoxLayout *temp=new QHBoxLayout(sign_up_widget);
    temp->addWidget(bu_confirm1);
    temp->addWidget(backlogin);
    temp->setSpacing(0);
    signup_vbox->addWidget(jobchose);
    signup_vbox->addLayout(temp);
    stackwidget->addWidget(login_widget);
    stackwidget->addWidget(sign_up_widget);
    stackwidget->setCurrentIndex(0);

}
void Logindlg::paintEvent(QPaintEvent *e){
    QPainter pan(this);

    pan.setRenderHint(QPainter::RenderHint::Antialiasing);
    pan.fillRect(m_rightrect,qRgb(163,172,218));
    pan.setBrush(QBrush(qRgb(255,255,255)));
    QPen penori=pan.pen();
    pan.setPen(Qt::NoPen);
    pan.drawRect(m_leftrect);
    pan.drawChord(m_middlerect,270*16,180*16);
    pan.setPen(penori);
    pan.setFont(QFont("微软雅黑",40,20,false));
    pan.drawStaticText(150,100,*lefttext);
    pan.setFont(QFont("微软雅黑",20,10,true));
    lefttext->setText("LOGIN");
    pan.drawStaticText(150,160,*lefttext);
    pan.drawLine(150,191,400,191);
    pan.drawPixmap(QPoint(10,20),QPixmap(":/back.png"));

    pan.end();


}

void Logindlg::keyPressEvent(QKeyEvent *e){

    if(e->key()==Qt::Key::Key_Escape){
        close();
    }


}
bool Logindlg::eventFilter(QObject *watch,QEvent *e){
    static QPoint POS;
    switch(e->type()){
    case QEvent::Type::MouseButtonPress:{
        QMouseEvent *mouse= dynamic_cast<QMouseEvent*>(e);
        if(mouse->button()==Qt::MouseButton::LeftButton){
            is_trag=true;
            POS=mouse->pos();
        }
        return true;
        break;
    }
    case QEvent::Type::MouseButtonRelease:{
        QMouseEvent *mouse= dynamic_cast<QMouseEvent*>(e);
        if(mouse->button()==Qt::MouseButton::LeftButton)  is_trag=false;
        return true;
        break;
    }
    case QEvent::Type::MouseMove:{
        QMouseEvent *mouse= dynamic_cast<QMouseEvent*>(e);
        if(is_trag) move(pos()+mouse->pos()-POS);
        break;
    }
    }

    return  false;
}
