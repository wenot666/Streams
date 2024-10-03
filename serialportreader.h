#ifndef SERIALPORTREADER_H
#define SERIALPORTREADER_H

#include <QObject>
#include <QSerialPort>

class SerialPortReader : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortReader(QObject *parent = nullptr);  // Конструктор
    ~SerialPortReader();  // Деструктор

    // Метод для начала чтения данных с COM-порта
    void startReading(const QString &portName);

signals:
    // Сигнал для передачи полученных данных
    void dataReceived(const QByteArray &data);

private slots:
    // Слот для обработки поступающих данных
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);


private:
    QSerialPort *serialPort;  // Указатель на объект QSerialPort
};

#endif // SERIALPORTREADER_H
