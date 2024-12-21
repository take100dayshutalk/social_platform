#include "talkwidget.h"
QString deal_n(QString str){

    int count=0;
    int i=0;
    QString temp=str;
    for(i=0;i<temp.length();i++){
        if(temp[i]=='\n'){
            count=0;
            continue;
        }
        count++;
        if(count==40){
            temp.insert(i+1,'\n');
            count=0;
        }
    }

    return temp;
}

talkwidget::talkwidget(QString user_account,QString token,QString hostpri,QWidget *parent):QWidget(parent){
    this->user_account=user_account;
    qDebug()<<this->user_account;
    this->token=token;
    cur_cache=-1;
    cur_target="";
    setMouseTracking(true);
    setFixedSize(640,480);



    manager=new QNetworkAccessManager(this);
    init();
    init_data();
    init_delegate();
    init_subcontrol();
    socket=new QTcpSocket(this);
    connect(socket,&QTcpSocket::readyRead,socket,[=]{
        auto data=socket->readAll();
        if(data=="-\r\n-CLOSE-\r\n-CONNECTION-\r\n-"){
            socket->close();
            return ;
        }
        QString midder("\r\n-MESSAGE-\r\n");
        int pos=data.indexOf("\r\n-MESSAGE-\r\n");
        if(pos==-1) qDebug()<<"发生错误";
        auto data1=data.mid(0,pos);
        auto data2=data.mid(pos+midder.length());
        QString sender=QString::fromUtf8(data1);
        QString message=QString::fromUtf8(data2);
        qDebug()<<sender<<"   "<<message;
        emit acceptmessage(sender,message);
    });
    socket->connectToHost(hostpri,10101);
    if(!socket->waitForConnected()){
        qDebug()<<"连接服务器端口失败";
        qDebug()<<hostpri;
    }
}
void talkwidget::init(){
    left=new QListView(this);
    QHBoxLayout *main_layout=new QHBoxLayout(this);
    main_layout->addWidget(left,2);
    message=new QListView(this);
    message->setSelectionMode(QAbstractItemView::NoSelection);
    editor=new QTextEdit(this);
    send=new QPushButton("发送",this);
    QVBoxLayout *right_layout=new QVBoxLayout;
    main_layout->addLayout(right_layout,8);
    right_layout->addWidget(message,7);
    QHBoxLayout *combo=new QHBoxLayout;
    right_layout->addLayout(combo,3);
    combo->addWidget(editor);
    combo->addWidget(send);
    send->setStyleSheet("QPushButton{ width: 76px;height: 76px;font-size: 20px;border-radius:38px;border:none;background-color:rgb(127,127,127);}");
    combo->setSpacing(0);
    main_layout->setSpacing(0);
    main_layout->setContentsMargins(0,0,0,0);

}
void talkwidget::init_data(){
    QFile file_talk("C:/Users/Administrator/AppData/Local/hutalkcache/data");
    if(!file_talk.open(QIODevice::ReadOnly)){
        file_talk.open(QIODevice::WriteOnly);
        QDataStream init_open(&file_talk);
        node obj;
        obj.cache=0;
        obj.name="社交学习平台服务号";
        obj.account="13767277336";
        obj.picture=QString(":/group/group_%1.png").arg(QRandomGenerator::global()->bounded(0,56));
        init_open<<obj.name<<obj.picture<<obj.cache<<obj.account;
        file_talk.close();
        chat node;
        node.head_pictire=QString(":/group/group_%1.png").arg(QRandomGenerator::global()->bounded(0,56));
        node.is_me=false;
        node.message="这是是社交平台服务号，欢迎来到此处咨询,都是人工回答,不是机器人,后台我们可以看到您的问题,请耐心等待。";
        node.who="服务号";
        QDir dir1("C:/Users/Administrator/AppData/Local/hutalkcache/cache");
        if(!dir1.exists())  dir1.mkdir("C:/Users/Administrator/AppData/Local/hutalkcache/cache");

        QFile file_cache0("C:/Users/Administrator/AppData/Local/hutalkcache/cache/cache_0");
        file_cache0.open(QIODevice::WriteOnly);
        QDataStream in(&file_cache0);
        in<<node.who<<node.message<<node.head_pictire<<node.is_me;
        file_cache0.close();
        file_talk.open(QIODevice::ReadOnly);
    }
    QDataStream in(&file_talk);
    while(!file_talk.atEnd()){
        node obj;
        in>>obj.name>>obj.picture>>obj.cache>>obj.account;
        talk_object.append(obj);
    }
     file_talk.close();

    qDebug()<<talk_object.at(0).picture;
    model_left=new QStandardItemModel(this);   
    for(const auto &item:talk_object){
        QStandardItem *standitem=new QStandardItem;
        standitem->setData(item.name,Qt::DisplayRole);
        standitem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        QFont temp=standitem->font();
        temp.setPointSize(10);
        standitem->setFont(temp);
        QPixmap pixmap(item.picture);
        pixmap=pixmap.scaled(32,32,Qt::KeepAspectRatio);
        standitem->setData(QVariant(pixmap),Qt::DecorationRole);
        standitem->setData(item.cache,Qt::UserRole+1);
        standitem->setData(item.account,Qt::UserRole+2);
        model_left->appendRow(standitem);
    }
    left->setModel(model_left);


    QDir dir1("C:/Users/Administrator/AppData/Local/hutalkcache/cache");
    if(!dir1.exists())  dir1.mkdir("C:/Users/Administrator/AppData/Local/hutalkcache/cache");

    model_message=new QStandardItemModel(this);
    message->setModel(model_message);
    connect(left,&QListView::clicked,[=](const QModelIndex &index){
        cur_cache=index.data(Qt::UserRole+1).toInt();
        cur_target=index.data(Qt::UserRole+2).toString();
        model_message->clear();
        talkmessage.clear();
        QString filename=QString("C:/Users/Administrator/AppData/Local/hutalkcache/cache/cache_%1").arg(cur_cache);
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)){
            file.open(QIODevice::WriteOnly);
            file.close();
            file.open(QIODevice::ReadOnly);
        }
        QDataStream In(&file);
        while(!In.atEnd()){
            chat tempchat;
            In>>tempchat.who>>tempchat.message>>tempchat.head_pictire>>tempchat.is_me;
            talkmessage.append(tempchat);
        }
        file.close();
        for(const auto item_message:talkmessage){
        QStandardItem *standarditem=new QStandardItem;
        QPixmap pixmap(item_message.head_pictire);
        pixmap=pixmap.scaled(25,25,Qt::KeepAspectRatio);
        standarditem->setData(QVariant(pixmap),Qt::DecorationRole);
        QString str;
        if(item_message.is_me) str=item_message.message;
        else str=item_message.who+':'+item_message.message;
        str=deal_n(str);
        standarditem->setData(str,Qt::DisplayRole);
        standarditem->setData(item_message.is_me,Qt::UserRole+1);
        model_message->appendRow(standarditem);
      }


    });





}
void talkwidget::init_delegate(){
    customdelegate *delegate=new customdelegate(left);
    left->setItemDelegate(delegate);
    customdelegate1 *delegate_message=new customdelegate1(message);
    message->setItemDelegate(delegate_message);


}
void talkwidget::init_subcontrol(){
    connect(send,&QPushButton::clicked,[=]{
        if(editor->toPlainText()=="") return ;
        if(cur_cache==-1) return ;
        chat temp_chat;
        temp_chat.head_pictire=":/group/group_3.png";
        temp_chat.is_me=true;
        temp_chat.message=editor->toPlainText();
        temp_chat.who="me";
        QFile file(QString("C:/Users/Administrator/AppData/Local/hutalkcache/cache/cache_%1").arg(cur_cache));
        if(!file.open(QIODevice::QIODevice::Append)){
            file.open(QIODevice::WriteOnly);
        }
        QDataStream in(&file);
        in<<temp_chat.who<<temp_chat.message<<temp_chat.head_pictire<<temp_chat.is_me;
        file.close();

        QStandardItem *temp=new QStandardItem;
        temp->setData(true,Qt::UserRole+1);
        QPixmap myself(QString(":/group/group_3.png"));
        myself=myself.scaled(25,25,Qt::KeepAspectRatio);
        temp->setData(QVariant(myself),Qt::DecorationRole);
        temp->setData(editor->toPlainText(),Qt::DisplayRole);
        model_message->appendRow(temp);
        sendmessage(user_account,cur_target,editor->toPlainText());
        editor->clear();

    });
    connect(this,&talkwidget::acceptmessage,[=](QString sender,QString message){
        bool is_find=false;
        int cache;
        for(const auto obj:talk_object)
            if(obj.account==sender){
                is_find=true;
                cache=obj.cache;
                break;
            }
        if(!is_find){
            int count=model_left->rowCount();
            QFile file(QString("C:/Users/Administrator/AppData/Local/hutalkcache/cache/cache_%1").arg(count));
            file.open(QIODevice::WriteOnly);
            QFile file1("C:/Users/Administrator/AppData/Local/hutalkcache/data");
            file1.open(QIODevice::Append);
            node N;
            N.account=sender;
            N.cache=count;
            N.name="暂定";
            N.picture=":/group/group_5.png";
            QDataStream intalk(&file1);
            intalk<<N.name<<N.picture<<N.cache<<N.account;
            file1.close();
            talk_object.append(N);
            QStandardItem *itemtalk=new QStandardItem;
            itemtalk->setData(N.name,Qt::DisplayRole);
            itemtalk->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            QFont temp=itemtalk->font();
            temp.setPointSize(10);
            itemtalk->setFont(temp);
            QPixmap pixmap(N.picture);
            pixmap=pixmap.scaled(32,32,Qt::KeepAspectRatio);
            itemtalk->setData(QVariant(pixmap),Qt::DecorationRole);
            itemtalk->setData(N.cache,Qt::UserRole+1);
            itemtalk->setData(N.account,Qt::UserRole+2);
            model_left->appendRow(itemtalk);
            chat C;
            C.head_pictire=":/group/group_6.png";
            C.is_me=false;
            C.who=sender;
            C.message=message;
            QDataStream in(&file);
            in<<C.who<<C.message<<C.head_pictire<<C.is_me;
            file.close();
        }
        else{
            QFile file(QString("C:/Users/Administrator/AppData/Local/hutalkcache/cache/cache_%1").arg(cache));
            if(!file.open(QIODevice::Append)){
                file.open(QIODevice::WriteOnly);
            }
            chat C;
            C.head_pictire=":/group/group_6.png";
            C.is_me=false;
            C.who=sender;
            C.message=message;
            QDataStream in(&file);
            in<<C.who<<C.message<<C.head_pictire<<C.is_me;
            file.close();
            if(cur_target==sender){
                QStandardItem *itemmessage=new QStandardItem;
                itemmessage->setData(false,Qt::UserRole+1);
                QPixmap headpicture(C.head_pictire);
                headpicture=headpicture.scaled(25,25,Qt::KeepAspectRatio);
                itemmessage->setData(QVariant(headpicture),Qt::DecorationRole);
                itemmessage->setData(message,Qt::DisplayRole);
                model_message->appendRow(itemmessage);
            }
        }

    });

}

void talkwidget::sendmessage(QString sender,QString target,QString message){
    QNetworkRequest request(QUrl("http://hutalk.vs4.tunnelfrp.com/message_transmit"));
    QJsonObject obj;
    obj.insert("user_account",sender);
    obj.insert("friend",target);
    obj.insert("message",message);
    obj.insert("token",token);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    auto reply=manager->post(request,QJsonDocument(obj).toJson(QJsonDocument::Compact));
    connect(reply,&QNetworkReply::finished,[=]{
        QByteArray data=reply->readAll();
        auto replyjson= QJsonDocument::fromJson(data);
        auto OBJ=replyjson.object();
        if(OBJ["code"].toInt()!=2000){
            QMessageBox::information(this,"tips","发生错误");
            qDebug()<<OBJ["code"].toInt();
        }
        reply->deleteLater();
    });


}
talkwidget::~talkwidget(){
    socket->write("-\r\n-CLOSE-\r\n-CONNECTION-\r\n-");
}
void customdelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    QStyleOptionViewItem opt=option;
    this->initStyleOption(&opt,index);
    QStyledItemDelegate::paint(painter,opt,index);
    QPen pen(Qt::black);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawRect(option.rect.x(),option.rect.y(),option.rect.width()+1,option.rect.height()+1);

}
