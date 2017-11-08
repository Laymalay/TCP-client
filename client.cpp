#include "client.h"
#include "ui_client.h"

Client::Client(const QString &_strHost, int _nPort, QWidget *parent) :
    QWidget(parent), m_nNextBlockSize(0),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    m_pTcpSocket = new QTcpSocket(this);
    strHost = _strHost;
    nPort = _nPort;
    connect(m_pTcpSocket, SIGNAL(connected()),this, SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotError(QAbstractSocket::SocketError)));
    m_ptxtInput = ui->lineEdit;
    ui->chat->setReadOnly(true);
    connect(ui->pushButton,SIGNAL(clicked()),this, SLOT(slotSendToServer()));
    connect(m_ptxtInput,SIGNAL(returnPressed()),this,SLOT(slotSendToServer()));
    ui->checkBox->setChecked(false);
    connect(ui->checkBox,SIGNAL(clicked(bool)),this,SLOT(ConnectToHost(bool)));
}
void Client::ConnectToHost(bool connect){
    if(connect){
         m_pTcpSocket->connectToHost(strHost, nPort);
         sendToServer(m_pTcpSocket, "Client connected");
    }
    else{
        sendToServer(m_pTcpSocket, "Client disconnected");
        m_pTcpSocket->disconnectFromHost();
    }
}
void Client::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for(;;){
        if(!m_nNextBlockSize){
            if(m_pTcpSocket->bytesAvailable() < sizeof(quint16)){
                break;
            }
            in >> m_nNextBlockSize;
        }
        if(m_pTcpSocket->bytesAvailable()<m_nNextBlockSize){
            break;
        }
        QTime time;
        QString str;
        in>>time>>str;
        ui->chat->append(time.toString()+" Server: "+str);
        m_nNextBlockSize = 0;
//        sendToServer(m_pTcpSocket, "Client Response: Received \"" + str + "\"");
    }
}



void Client::slotSendToServer()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);
    //TODO whyyy version
    out<<quint16(0)<<QTime::currentTime()<<m_ptxtInput->text();
    out.device()->seek(0);
    out << quint16(arrBlock.size()- sizeof(quint16));
    m_pTcpSocket->write(arrBlock);
    ui->chat->append(QTime::currentTime().toString()+" "+m_ptxtInput->text());
    m_ptxtInput->clear();

}


void Client::sendToServer(QTcpSocket *pSocket, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

void Client::slotConnected()
{
    ui->chat->append("Received the connected() signal");
}




Client::~Client()
{
    delete ui;
}



void Client::slotError(QAbstractSocket::SocketError err){
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                         "the host wasn't found." :
                         err == QAbstractSocket::RemoteHostClosedError ?
                         "the remote host is closed." :
                         err == QAbstractSocket::ConnectionRefusedError ?
                         "the connection was refused." :
                         QString(m_pTcpSocket->errorString())
            );
   ui->chat->append(strError);
}
