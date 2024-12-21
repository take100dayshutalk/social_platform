#include "mainwidget.h"

mainwidget::mainwidget(QString user_account,QString token,QWidget *parent):QMainWindow(parent){
    setFixedSize(800,480);
    this->token=token;
    this->user_account=user_account;
    center=new QWidget(this);
    center->setMouseTracking(true);
    this->setCentralWidget(center);
    this->setMouseTracking(true);
    QSettings set("C:/Users/Administrator/AppData/Local/hutalkcache/soft.ini",QSettings::IniFormat);
    if(set.value("is_init_second").toBool()){
        qDebug()<<"初始化过的";

    }
    else{
        set.setValue("auto_start",false);
        set.setValue("message_ring",false);
        set.setValue("ip_check",false);
        set.setValue("join_plan",false);
        set.setValue("update",false);
        set.setValue("file_cache","C:/Users/Administrator/AppData/Local/hutalkcache/file_cache");
        set.setValue("is_init_second",true);
    }
    init_();
    menubar=new QMenuBar(this);
    QAction *act1=new QAction(QIcon(":/logout.png"),"退出登陆",this);
    menubar->addAction(act1);
    connect(act1,&QAction::triggered,[=]{emit logout();});
    this->setMenuBar(menubar);



}
void mainwidget::init_(){

    QHBoxLayout *main_layout=new QHBoxLayout(center);
    stackwidget=new QStackedWidget;
    stackwidget->setMouseTracking(true);
    stackwidget->resize(640,480);
    main_layout->addWidget(stackwidget);
    containerwidget=new QWidget(center);
    containerwidget->setMouseTracking(true);
    main_layout->addWidget(containerwidget);
    QFile filesheet(":/style.css");
    if(!filesheet.open(QIODevice::ReadOnly)){
        qDebug()<<"样式文件打开失败";
    }
    auto style=filesheet.readAll();
    containerwidget->setStyleSheet(style);
    filesheet.close();
    QVBoxLayout *right=new QVBoxLayout(containerwidget);
    containerwidget->setLayout(right);
    for(int i=0;i<5;i++){
        right_tool[i]=new QToolButton(containerwidget);
        right->addWidget(right_tool[i]);
        right_tool[i]->setToolButtonStyle(Qt::ToolButtonIconOnly);
        right_tool[i]->setIconSize(QSize(32,32));
    }
    right_tool[0]->setIcon(QIcon(":/talk.png"));
    right_tool[1]->setIcon(QIcon(":/friend_circle.png"));
    right_tool[2]->setIcon(QIcon(":/friend_list.png"));
    right_tool[3]->setIcon(QIcon(":/home.png"));
    right_tool[4]->setIcon(QIcon(":/setting.png"));
    containerwidget->hide();
    is_hiden_container=true;

    main_layout->setSpacing(0);
    main_layout->setContentsMargins(0,0,0,0);



    TalkWidget=new talkwidget(user_account,token,"vs4.tunnelfrp.com",stackwidget);
    setting=new settingwidget(stackwidget);
    yourfriend=new friendwidget(user_account,token,"http://hutalk.vs4.tunnelfrp.com",stackwidget);
    stackwidget->addWidget(TalkWidget);
    stackwidget->addWidget(yourfriend);
    stackwidget->addWidget(setting);
    connect(right_tool[4],&QToolButton::clicked,[=]{ stackwidget->setCurrentIndex(2); });
    connect(right_tool[2],&QToolButton::clicked,[=]{stackwidget->setCurrentIndex(1);});
    connect(right_tool[0],&QToolButton::clicked,[=]{stackwidget->setCurrentIndex(0);});


}
void mainwidget::mouseMoveEvent(QMouseEvent *e){

    if(e->pos().x()>650&&is_hiden_container){
        is_hiden_container=false;
        containerwidget->show();
    }
    if(e->pos().x()<=650&&!is_hiden_container){
        is_hiden_container=true;
        containerwidget->hide();
    }
    QMainWindow::mouseMoveEvent(e);

}
void mainwidget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.fillRect(rect(),Qt::white);




}
bool mainwidget::test(){



    return false;

}
container::container(QWidget *parent):QWidget(parent){
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);

}
void container::enterEvent(QEnterEvent *e){
    qDebug()<<"测试1";
    show();
}
void container::leaveEvent(QEvent *e){
    qDebug()<<"测试2";
    hide();
}

