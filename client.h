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
    Client(const QString& _strHost, int _nPort, QWidget *parent = 0);
    ~Client();

private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit* m_ptxtInfo;
    QLineEdit* m_ptxtInput;
    quint16 m_nNextBlockSize;
    Ui::Client *ui;
    QString strHost;
    int nPort;
private:
    void sendToServer(QTcpSocket* pSocket, const QString& str);
private slots:
    void slotReadyRead();
    void ConnectToHost(bool connect);
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();
};

#endif // CLIENT_H
