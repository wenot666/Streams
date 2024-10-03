#include "dataemitter.h"
#include <QDebug>
#include <cstdlib> // Для std::rand()
#include <ctime>   // Для std::time()

DataEmitter::DataEmitter(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this)), timer(new QTimer(this)),
    amplitude(1.0), frequency(1.0), phase(0.0), randomDataMode(false), time(0.0)
{
    connect(timer, &QTimer::timeout, this, &DataEmitter::sendData);
    std::srand(static_cast<unsigned>(std::time(nullptr))); // Инициализация генератора случайных чисел
}

DataEmitter::~DataEmitter()
{
    stopSending(); // Остановить отправку при уничтожении
}

void DataEmitter::startSending(const QString &host, quint16 port)
{
    socket->connectToHost(host, port); // Подключаемся к серверу
    if (socket->waitForConnected()) {
        qDebug() << "Connected to" << host << "on port" << port;
        timer->start(100); // Отправляем данные каждые 100 мс
    } else {
        qDebug() << "Failed to connect:" << socket->errorString();
    }
}

void DataEmitter::stopSending()
{
    timer->stop(); // Останавливаем таймер
    socket->disconnectFromHost(); // Отключаемся
    if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(100)) {
        qDebug() << "Disconnected.";
    }
}

void DataEmitter::sendData()
{
    QByteArray data;

    // Генерация данных
    if (randomDataMode) {
        // Генерация случайного числа
        double randomValue = std::rand() % 100; // Генерация случайного числа от 0 до 100
        data.append(QString::number(randomValue).toUtf8()); // Преобразуем QString в QByteArray
    } else {
        // Генерация синусоидального сигнала
        double signalValue = amplitude * std::sin(2 * M_PI * frequency * time + phase);
        data.append(QString::number(signalValue).toUtf8()); // Преобразуем QString в QByteArray
        time += 0.1; // Увеличиваем время для следующего значения
    }

    // Отправка данных
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(data);
        socket->flush(); // Обеспечиваем отправку данных
    } else {
        qDebug() << "Socket not connected.";
    }
}


void DataEmitter::setAmplitude(double newAmplitude)
{
    amplitude = newAmplitude;
}

void DataEmitter::setFrequency(double newFrequency)
{
    frequency = newFrequency;
}

void DataEmitter::setPhase(double newPhase)
{
    phase = newPhase;
}

void DataEmitter::setRandomDataMode(bool enabled)
{
    randomDataMode = enabled;
}
