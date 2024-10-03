#ifndef ETHERNETREADER_H
#define ETHERNETREADER_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>

class EthernetReader : public QObject
{
    Q_OBJECT

public:
    explicit EthernetReader(QObject *parent = nullptr);  // Конструктор
    ~EthernetReader();  // Деструктор

    // Метод для установления TCP-соединения
    void startTcpConnection(const QString &host, quint16 port);

    // Метод для установления UDP-соединения
    void startUdpConnection(quint16 port);

    // Закрытие TCP-соединения
    void closeTcpConnection();

    // Проверка статуса TCP-соединения
    bool isTcpConnected() const;

signals:
    // Сигнал для передачи полученных данных
    void dataReceived(const QByteArray &data);

private slots:
    // Слоты для обработки поступающих данных
    void handleTcpReadyRead();
    void handleUdpReadyRead();

    // Обработка ошибок для TCP и UDP соединений
    void handleTcpError(QAbstractSocket::SocketError socketError);
    void handleUdpError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *tcpSocket;  // Сокет для TCP-соединения
    QUdpSocket *udpSocket;  // Сокет для UDP-соединения
};

#endif // ETHERNETREADER_H
