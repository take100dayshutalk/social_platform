#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H
#include "special.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
class settingwidget:public QWidget{
    Q_OBJECT
public:
    settingwidget(QWidget *parent=nullptr);
private:
    BoundarySwitchBase *switchbu[5];

};




#endif // SETTINGWIDGET_H
