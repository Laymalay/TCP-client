#ifndef CLIENT_H
#define CLIENT_H
#include<QTcpSocket>
#include<QTextEdit>
#include<QLineEdit>
#include <QWidget>
#include <QTime>

class QTextEdit;
class QLineEdit;
namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    Client(const QString& strHost, int nPort, QWidget *parent = 0);
    ~Client();

private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit* m_ptxtInfo;
    QLineEdit* m_ptxtInput;
    quint16 m_nNextBlockSize;
    Ui::Client *ui;
private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();
};

#endif // CLIENT_H
