#include "serialportreader.h"
#include <QDebug>

SerialPortReader::SerialPortReader(QObject *parent)
    : QObject(parent), serialPort(new QSerialPort(this))
{
    // Подключаем сигнал готовности данных и сигнал ошибки
    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortReader::handleReadyRead);
    connect(serialPort, &QSerialPort::errorOccurred, this, &SerialPortReader::handleError);
}

SerialPortReader::~SerialPortReader()
{
    // Закрываем COM-порт, если он открыт
    if (serialPort->isOpen()) {
        serialPort->close();
    }
}

void SerialPortReader::startReading(const QString &portName)
{
    // Устанавливаем имя порта
    serialPort->setPortName(portName);

    // Открываем порт в режиме только для чтения
    if (serialPort->open(QIODevice::ReadOnly)) {
        qDebug() << "Serial port" << portName << "opened successfully!";
    } else {
        qDebug() << "Failed to open serial port:" << serialPort->errorString();
    }
}

void SerialPortReader::handleReadyRead()
{
    // Читаем все доступные данные из порта
    const QByteArray data = serialPort->readAll();

    // Генерируем сигнал для передачи полученных данных
    emit dataReceived(data);
}

void SerialPortReader::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        qDebug() << "Critical error:" << serialPort->errorString();
        serialPort->close();  // Закрываем порт при критической ошибке
    } else {
        qDebug() << "Error:" << serialPort->errorString();
    }
}
