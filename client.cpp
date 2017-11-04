#include "client.h"
#include "ui_client.h"

Client::Client(const QString &strHost, int nPort, QWidget *parent) :
    QWidget(parent), m_nNextBlockSize(0),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()),this, SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotError(QAbstractSocket::SocketError)));
    m_ptxtInfo = ui->textEdit;
    m_ptxtInput = ui->lineEdit;
    m_ptxtInfo->setReadOnly(true);
    connect(ui->pushButton,SIGNAL(clicked()),this, SLOT(slotSendToServer()));
    connect(m_ptxtInput,SIGNAL(returnPressed()),this,SLOT(slotSendToServer()));
}

Client::~Client()
{
    delete ui;
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
        m_ptxtInfo->append(time.toString()+" "+str);
        m_nNextBlockSize = 0;
    }
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
   m_ptxtInfo->append(strError);

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
    m_ptxtInput->clear();

}

void Client::slotConnected()
{
    m_ptxtInfo->append("Received the connected() signal");
}


