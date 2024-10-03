#ifndef DATAEMITTER_H
#define DATAEMITTER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <cmath> // Для работы с математическими функциями

class DataEmitter : public QObject
{
    Q_OBJECT

public:
    explicit DataEmitter(QObject *parent = nullptr);
    ~DataEmitter();

    void startSending(const QString &host, quint16 port); // Начать отправку данных
    void stopSending(); // Остановить отправку данных

    void setAmplitude(double amplitude); // Установка амплитуды
    void setFrequency(double frequency); // Установка частоты
    void setPhase(double phase); // Установка фазы

    void setRandomDataMode(bool enabled); // Включение/выключение случайных данных

private slots:
    void sendData(); // Отправка данных

private:
    QTcpSocket *socket; // Сокет для TCP соединения
    QTimer *timer; // Таймер для периодической отправки данных
    double amplitude; // Амплитуда сигнала
    double frequency; // Частота сигнала
    double phase; // Фаза сигнала
    bool randomDataMode; // Режим случайных данных
    double time; // Время для генерации сигнала
};

#endif // DATAEMITTER_H
