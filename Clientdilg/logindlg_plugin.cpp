#include "logindlg_plugin.h"
#include "Clientdlg.h"
login_plugin_message::login_plugin_message(QWidget *parent):QDialog(parent){
    resize(250,150);
    setWindowTitle("信息填入");
    init();

}
void login_plugin_message::init(){
    QVBoxLayout *all=new QVBoxLayout(this);
    QHBoxLayout *gender=new QHBoxLayout;
    man=new QRadioButton("男");
    man->setChecked(true);
    woman=new QRadioButton("女");
    gender->addWidget(man);
    gender->addWidget(woman);
    confirm=new QPushButton("确认");
    gender->addWidget(confirm);
    email=new QLineEdit;
    email->setPlaceholderText("邮箱(必填项)");
    phone=new QLineEdit;
    phone->setPlaceholderText("手机电话(非毕填项)");
    all->addWidget(email);
    all->addWidget(phone);
    all->addLayout(gender);
    QRegularExpression regExp1("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    QRegularExpression regExp2("\\d+");
    QRegularExpressionValidator *validator1=new QRegularExpressionValidator(regExp1,email);
    QRegularExpressionValidator *validator2=new QRegularExpressionValidator(regExp2,phone);
    email->setValidator(validator1);
    phone->setValidator(validator2);

    connect(confirm,&QPushButton::clicked,[=]{
        if(email->text()==""){
            QMessageBox::information(this,"tips","邮箱不可为空");
            return ;
        }
        bool is_man;
        if(man->isChecked()) is_man=true;
        else is_man=false;
        emit submit_message(email->text(),phone->text(),is_man);
        login_plugin_verify verifydlg(this);
        connect(&verifydlg,&login_plugin_verify::submit_code,this,[=](QString code){
            emit submit_seconde_code(code);
        });

        connect(&verifydlg,&login_plugin_verify::get_code,this,[=]{emit get_seconde_code();});
        if(verifydlg.exec()==QDialog::Rejected) return ;
        else this->accept();

    });
}
login_plugin_verify::login_plugin_verify(QWidget *parent):QDialog(parent){
    resize(200,100);
    setWindowTitle("邮箱验证");
    init();
    QRegularExpression regExp("\\d+");
    QRegularExpressionValidator *validator=new QRegularExpressionValidator(regExp,code);
    code->setValidator(validator);
    limit60=new QTimer(this);
    QFile file_timeout("C:/Users/Administrator/AppData/Local/hutalkcache/timeout");
    file_timeout.open(QIODevice::ReadOnly);
    QDataStream in(&file_timeout);
    in>>timeout;
    file_timeout.close();
    qDebug()<<timeout;
    if(timeout!=0){
        resend->setEnabled(false);
        resend->setText(QString("%1s").arg(timeout));
        QTimer *timeleft=new QTimer(this);
        connect(timeleft,&QTimer::timeout,[=]{
            if(timeout==0){
                resend->setEnabled(true);
                resend->setText("获取");
                timeleft->stop();
                timeleft->deleteLater();
                return ;
            }
            timeout--;
            QString text=QString("%1s").arg(timeout);
            resend->setText(text);
        });
        timeleft->start(1000);

    }
    connect(resend,&QPushButton::clicked,[=]{
        resend->setEnabled(false);
        emit get_code();
        limit60->start(1000);
    });
    connect(limit60,&QTimer::timeout,this,[=]{
        static int i=60;
        static int j=1;
        static int time=60;
        if(i==0){
            j++;
            time=j*60;
            i=time;
            resend->setEnabled(true);
            resend->setText("获取");
            limit60->stop();
            return ;
        }
        QString text=QString("%1s").arg(i);
        resend->setText(text);
        i--;
        timeout=i;

    });
    connect(confirm,&QPushButton::clicked,[=]{
        if(code->text().length()!=6){
            QMessageBox::information(this,"Tips","验证码错误");
            return ;
        }
        emit submit_code(code->text());
    });
    auto ptr=this->parent()->parent();
    auto pptr=dynamic_cast<Logindlg*>(ptr);
    connect(pptr,&Logindlg::verify_sucess,[=]{this->accept();});
}
void login_plugin_verify::init(){
    code=new QLineEdit;
    code->setPlaceholderText("验证码");
    resend=new QPushButton("获取");
    confirm=new QPushButton("确认");
    QHBoxLayout *row=new QHBoxLayout;
    QVBoxLayout *all=new QVBoxLayout(this);
    row->addWidget(code,4);
    row->addWidget(resend,1);
    all->addLayout(row);
    all->addWidget(confirm);




}
void login_plugin_verify::closeEvent(QCloseEvent *e){
    limit60->deleteLater();
    QFile file_timeout("C:/Users/Administrator/AppData/Local/hutalkcache/timeout");
    file_timeout.open(QIODevice::WriteOnly);
    QDataStream in(&file_timeout);
    in<<timeout;
    file_timeout.close();
    QDialog::closeEvent(e);
}
void login_plugin_message::paintEvent(QPaintEvent *e){
    QPainter painter(this);
    painter.fillRect(rect(),qRgb(163,172,218));
}
void login_plugin_verify::paintEvent(QPaintEvent *e){
    QPainter painter(this);
    painter.fillRect(rect(),qRgb(163,172,218));
}



























