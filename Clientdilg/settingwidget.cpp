#include "settingwidget.h"

settingwidget::settingwidget(QWidget *parent):QWidget(parent){
    setFixedSize(640,480);
    for(int i=0;i<5;i++)
    switchbu[i]=new BoundarySwitchBase(1,false,this);
    QVBoxLayout *mainlayout=new QVBoxLayout(this);
    this->setLayout(mainlayout);
    QLabel *label[5];
    label[0]=new QLabel("开机自启动",this);
    label[1]=new QLabel("消息提醒",this);
    label[2]=new QLabel("自动获取更新",this);
    label[3]=new QLabel("加入用户体验计划",this);
    label[4]=new QLabel("异常安全验证",this);
    for(int i=0;i<5;i++){
        QHBoxLayout *temp=new QHBoxLayout;
        temp->addWidget(switchbu[i]);
        temp->addWidget(label[i]);
        temp->addStretch();
        mainlayout->addLayout(temp);
    }
    QLabel *file_cachetext=new QLabel("文件缓存目录设置",this);
    QLineEdit *file_cache=new QLineEdit(this);
    QPushButton *confirm=new QPushButton("更改",this);
    QHBoxLayout *temp=new QHBoxLayout;
    temp->addWidget(file_cachetext);
    temp->addWidget(file_cache);
    temp->addWidget(confirm);
    temp->addStretch();
    mainlayout->addLayout(temp);
    QPushButton *clear_toone=new QPushButton("清除特定人或群的聊天记录",this);
    QPushButton *clear_all=new QPushButton("清除所有聊天记录",this);
    QPushButton *clear_cache=new QPushButton("清除文件缓存",this);
    clear_toone->resize(300,35);
    clear_all->resize(300,35);
    clear_cache->resize(300,35);
    mainlayout->addWidget(clear_toone);
    mainlayout->addWidget(clear_all);
    mainlayout->addWidget(clear_cache);
    mainlayout->addStretch();
}
