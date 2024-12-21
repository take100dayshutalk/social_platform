#include "friendwidget.h"

friendwidget::friendwidget(QString user_account,QString token,QString hostpri,QWidget *parent):QWidget(parent){
    this->hostpri=hostpri;
    this->token=token;
    this->self_account=user_account;
    this->manager=new QNetworkAccessManager(this);
    init_();

}
void friendwidget::init_(){
    clickup_menu=new QMenu(this);
    QAction *act1=new QAction("发消息",clickup_menu);
    QAction *act2=new QAction("个人主页",clickup_menu);
    clickup_menu->addAction(act1);
    clickup_menu->addAction(act2);
    clickup_menu->hide();
    confirm=new QPushButton(this);
    confirm->setText("加为好友");
    search_user=new QLineEdit(this);
    QHBoxLayout *layout=new QHBoxLayout;
    layout->addWidget(search_user);
    layout->addWidget(confirm);
    QVBoxLayout *mainlayout=new QVBoxLayout(this);
    mainlayout->addLayout(layout);
    friendlist=new QListWidget(this);
    mainlayout->addWidget(friendlist);


    QRegularExpression expression("^[a-zA-Z0-9]+$");
    QRegularExpressionValidator *validator=new QRegularExpressionValidator(expression);
    search_user->setValidator(validator);





    QNetworkRequest request(QUrl(hostpri+"/api/friendlist"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QJsonObject obj;
    obj.insert("user_account",self_account);
    obj.insert("token",token);
    auto reply = manager->post(request,QJsonDocument(obj).toJson(QJsonDocument::Compact));
    connect(reply,&QNetworkReply::finished,[=]{
        auto data=reply->readAll();
        auto document=QJsonDocument::fromJson(data);
        auto OBJ=document.object();
        if(OBJ["code"].toInt()!=2000){
            QMessageBox::information(this,"warnning",OBJ["message"].toString());
            qDebug()<<OBJ["code"].toInt();
        }
        else{
            QJsonArray nick=OBJ["nick"].toArray();
            QJsonArray useraccount=OBJ["user_account"].toArray();
            for(int i=0;i<useraccount.count();i++){
                friendspace::node *temp=new friendspace::node;
                temp->user_account=useraccount[i].toString();
                temp->nick=nick[i].toString();
                friends.append(temp);
            }
        }
        reply->deleteLater();
    });
        //开始对friendlist控件调整数据

    for(const auto &item:friends){
        QListWidgetItem *temp=new QListWidgetItem(friendlist);
        temp->setText(item->nick);
        friendlist->addItem(temp);
    }
    connect(friendlist,&QListWidget::customContextMenuRequested,[=](const QPoint &point){
        auto pos=friendlist->viewport()->mapToGlobal(point);
        clickup_menu->exec(pos);


    });
    connect(confirm,&QPushButton::clicked,[=]{
        if(search_user->text()=="") return ;
        if(search_user->text()==self_account) return ;
        QJsonObject obj1;
        obj1.insert("user_account",self_account);
        obj1.insert("friend_account",search_user->text());
        obj1.insert("token",token);
        QNetworkRequest request1(QUrl(hostpri+"/api/add_friend"));
        request1.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader,"application/json");
        auto reply1=manager->post(request1,QJsonDocument(obj1).toJson(QJsonDocument::Compact));
        connect(reply1,&QNetworkReply::finished,[=]{

            auto data_raw=reply1->readAll();
            auto document_s=QJsonDocument::fromJson(data_raw);
            auto OBJs=document_s.object();
            if(OBJs["code"].toInt()!=2000){
                QMessageBox::information(this,"warnning",OBJs["message"].toString());
                reply1->deleteLater();
                return ;
                qDebug()<<OBJs["code"].toInt();
            }
           friendspace::node *temp=new friendspace::node;
           temp->nick=OBJs["nick"].toString();
           temp->user_account=search_user->text();
           friends.append(temp);
           QListWidgetItem *news=new QListWidgetItem(friendlist);
           news->setText(temp->nick);
           reply1->deleteLater();
        });



    });

}
friendwidget::~friendwidget(){
    for(int i=0;i<friends.count();i++){
        friendspace::node *temp= friends.at(i);
        delete temp;
    }

}
