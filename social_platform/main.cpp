#include <QApplication>
#include <QWidget>
#include "server.h"
//qq授权码:wqbkutjcqdckcjai
int main(int argc,char *argv[]){
    QApplication app(argc,argv);
    server m_server("0.0.0.0",8081,true);
    m_server.setkey("112233445566");
    QWidget w;
    w.show();
    return app.exec();
}
