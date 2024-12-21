#ifndef ENCIPHER_H
#define ENCIPHER_H
#include <QString>
#include <QByteArray>
class encipher{
public:
    encipher(QString str,QString key);
    QString data;
private:
    void OP();
    QByteArray DATA;
};

#endif // ENCIPHER_H
