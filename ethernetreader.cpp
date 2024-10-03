#include "ethernetreader.h"
#include <QHostAddress>
#include <QDebug>

EthernetReader::EthernetReader(QObject *parent)
    : QObject(parent), tcpSocket(new QTcpSocket(this)), udpSocket(new QUdpSocket(this))
{
    // Подключаем сигналы для чтения данных и обработки ошибок
    connect(tcpSocket, &QTcpSocket::readyRead, this, &EthernetReader::handleTcpReadyRead);
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, &EthernetReader::handleTcpError);

    connect(udpSocket, &QUdpSocket::readyRead, this, &EthernetReader::handleUdpReadyRead);
    connect(udpSocket, &QUdpSocket::errorOccurred, this, &EthernetReader::handleUdpError);
}

EthernetReader::~EthernetReader()
{
    // Закрываем соединения при завершении
    if (tcpSocket->isOpen()) {
        tcpSocket->close();
    }
    if (udpSocket->isOpen()) {
        udpSocket->close();
    }
}

void EthernetReader::startTcpConnection(const QString &host, quint16 port)
{
    tcpSocket->connectToHost(host, port);  // Подключаемся к указанному хосту и порту через TCP
    if (tcpSocket->waitForConnected()) {
        qDebug() << "TCP connection established with" << host << "on port" << port;
    } else {
        qDebug() << "TCP connection failed:" << tcpSocket->errorString();
    }
}

void EthernetReader::startUdpConnection(quint16 port)
{
    if (udpSocket->bind(QHostAddress::Any, port)) {
        qDebug() << "UDP socket bound to port" << port;
    } else {
        qDebug() << "Failed to bind UDP socket:" << udpSocket->errorString();
    }
}

void EthernetReader::closeTcpConnection()
{
    if (tcpSocket->isOpen()) {
        tcpSocket->close();
        qDebug() << "TCP connection closed.";
    }
}

bool EthernetReader::isTcpConnected() const
{
    return tcpSocket->state() == QAbstractSocket::ConnectedState;
}

void EthernetReader::handleTcpReadyRead()
{
    const QByteArray data = tcpSocket->readAll();  // Читаем все доступные данные из TCP-сокета
    emit dataReceived(data);  // Генерируем сигнал с полученными данными
}

void EthernetReader::handleUdpReadyRead()
{
    QByteArray buffer;
    buffer.resize(udpSocket->pendingDatagramSize());  // Резервируем место для входящего UDP-пакета
    udpSocket->readDatagram(buffer.data(), buffer.size());  // Читаем пакет
    emit dataReceived(buffer);  // Генерируем сигнал с полученными данными
}

void EthernetReader::handleTcpError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "TCP Socket Error:" << tcpSocket->errorString();
}

void EthernetReader::handleUdpError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "UDP Socket Error:" << udpSocket->errorString();
}
