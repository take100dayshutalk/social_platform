#ifndef TALKWIDGET_H
#define TALKWIDGET_H
#include <QWidget>
#include <QListView>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyledItemDelegate>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QList>
#include <QPair>
#include <QFont>
#include <QPen>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
struct node{
    QString name;
    QString account;
    QString picture;
    int cache;
};
struct chat{
    QString who;
    QString message;
    QString head_pictire;
    bool is_me;
};


class customdelegate:public QStyledItemDelegate{
    using QStyledItemDelegate::QStyledItemDelegate;
public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        return QSize(option.rect.size().width(),40);
    }
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};
class customdelegate1:public QStyledItemDelegate{
    using QStyledItemDelegate::QStyledItemDelegate;
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        if(index.data(Qt::UserRole+1).toBool()){
            opt.decorationPosition=QStyleOptionViewItem::Position::Right;
            opt.displayAlignment=Qt::AlignRight|Qt::AlignCenter;
        }
        QStyledItemDelegate::paint(painter, opt, index);
    }


};
class talkwidget:public QWidget{
Q_OBJECT
public:
    talkwidget(QString user_account,QString token,QString hostpri,QWidget *parent=nullptr);
    ~talkwidget();
signals:
    void acceptmessage(QString sender,QString message);
private:
    void init();
    void init_data();
    void init_delegate();
    void init_subcontrol();
    void sendmessage(QString sender,QString target,QString message);
    QListView *left,*message;
    QList<node> talk_object;
    QList<chat> talkmessage;
    QPushButton *send;
    QTextEdit *editor;
    QStandardItemModel *model_left,*model_message;
    int cur_cache;
    QString cur_target;
    QHostAddress hostaddress;
    QTcpSocket *socket;
    QNetworkAccessManager *manager;
    QString user_account;
    QString token;


};





#endif // TALKWIDGET_H
