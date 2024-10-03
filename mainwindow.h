#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>    // Для текстового поля лога
#include <QVBoxLayout>  // Для вертикального макета
#include "qcustomplot.h"  // Для графиков
#include "dataemitter.h"
#include <QInputDialog>  // Для диалогов

QT_BEGIN_NAMESPACE
class SerialPortReader;  // Предварительное объявление класса для работы с COM-портом
class EthernetReader;    // Предварительное объявление класса для работы с Ethernet
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDataReceived(const QByteArray &data);  // Слот для обработки полученных данных
    void selectComPort();  // Выбор COM-порта через диалог
    void selectEthernetAddress();  // Ввод Ethernet-адреса и порта
    void startDataEmitter(); // запуск эмулятора
    void stopDataEmitter();  // остановкa эмулятора

private:
    // Хранилище для данных
    double accumulatedSum;     // Для хранения суммы
    double accumulatedProduct; // Для хранения произведения

    // Элементы пользовательского интерфейса
    QTextEdit *logTextEdit;    // Текстовое поле для логов
    QCustomPlot *customPlot;   // График для отображения оригинальных данных
    QCustomPlot *sumPlot;      // График для отображения суммы
    QCustomPlot *productPlot;  // График для отображения произведения

    // Потоки и классы для работы с COM-портом и Ethernet
    SerialPortReader *serialReader;  // Объект для работы с COM-портом
    EthernetReader *ethernetReader;  // Объект для работы с Ethernet
    QThread *serialThread;           // Поток для работы с COM-портом
    QThread *ethernetThread;         // Поток для работы с Ethernet

    DataEmitter *dataEmitter; // Объект для генерации данных

    // Объявление методов для математических операций
    void addValue(double newValue);       // Метод для сложения
    void multiplyValue(double newValue);  // Метод для умножения
};

#endif // MAINWINDOW_H
