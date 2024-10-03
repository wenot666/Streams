#include "mainwindow.h"
#include "serialportreader.h"
#include "ethernetreader.h"
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QInputDialog>
#include <QMenuBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    accumulatedSum(0),
    accumulatedProduct(1)
{
    // Создаем основной виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Создаем вертикальный макет
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Создаем текстовое поле для логов
    logTextEdit = new QTextEdit(this);
    logTextEdit->setFixedSize(400,100);
    logTextEdit->setReadOnly(true);
    layout->addWidget(logTextEdit);

    // Создаем графики
    customPlot = new QCustomPlot(this);
    customPlot->setFixedSize(400,200);
    layout->addWidget(customPlot);
    customPlot->addGraph();
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel("Value");
    customPlot->xAxis->setRange(0, 10);
    customPlot->yAxis->setRange(-1, 1);

    sumPlot = new QCustomPlot(this);
    sumPlot->setFixedSize(400,200);
    layout->addWidget(sumPlot);
    sumPlot->addGraph();
    sumPlot->xAxis->setLabel("Time");
    sumPlot->yAxis->setLabel("Sum");
    sumPlot->xAxis->setRange(0, 10);
    sumPlot->yAxis->setRange(-10, 10);

    productPlot = new QCustomPlot(this);
    productPlot->setFixedSize(400,200);
    layout->addWidget(productPlot);
    productPlot->addGraph();
    productPlot->xAxis->setLabel("Time");
    productPlot->yAxis->setLabel("Product");
    productPlot->xAxis->setRange(0, 10);
    productPlot->yAxis->setRange(-10, 10);

    // Создаем объекты для чтения данных
    serialReader = new SerialPortReader();
    ethernetReader = new EthernetReader();

    // Создаем потоки
    serialThread = new QThread();
    ethernetThread = new QThread();

    // Перемещаем объекты в потоки
    serialReader->moveToThread(serialThread);
    ethernetReader->moveToThread(ethernetThread);

    // Подключаем обработчики сигналов
    connect(serialReader, &SerialPortReader::dataReceived, this, &MainWindow::onDataReceived);
    connect(ethernetReader, &EthernetReader::dataReceived, this, &MainWindow::onDataReceived);

    // Запускаем потоки
    serialThread->start();
    ethernetThread->start();

    // Создаем экземпляр DataEmitter
    dataEmitter = new DataEmitter(this);

    // Создаем меню для управления подключениями
    QMenu *portMenu = menuBar()->addMenu("Port");
    QAction *selectPortAction = new QAction("Select COM Port", this);
    portMenu->addAction(selectPortAction);
    connect(selectPortAction, &QAction::triggered, this, &MainWindow::selectComPort);

    QMenu *ethernetMenu = menuBar()->addMenu("Ethernet");
    QAction *connectEthernetAction = new QAction("Connect to Ethernet", this);
    ethernetMenu->addAction(connectEthernetAction);
    connect(connectEthernetAction, &QAction::triggered, this, &MainWindow::selectEthernetAddress);

    // Добавляем кнопки для управления эмуляцией
    QToolBar *toolBar = addToolBar("Control");
    QAction *startEmitterAction = new QAction("Start Emitter", this);
    QAction *stopEmitterAction = new QAction("Stop Emitter", this);
    toolBar->addAction(startEmitterAction);
    toolBar->addAction(stopEmitterAction);

    connect(startEmitterAction, &QAction::triggered, this, &MainWindow::startDataEmitter);
    connect(stopEmitterAction, &QAction::triggered, this, &MainWindow::stopDataEmitter);
}

MainWindow::~MainWindow()
{
    // Останавливаем потоки
    serialThread->quit();
    serialThread->wait();

    ethernetThread->quit();
    ethernetThread->wait();

    delete logTextEdit;
    delete customPlot;
    delete sumPlot;
    delete productPlot;
    delete serialReader;
    delete ethernetReader;
    delete dataEmitter; // Удаляем эмулятор
}

void MainWindow::onDataReceived(const QByteArray &data)
{
    // Преобразуем данные в строку
    QString receivedData = QString::fromUtf8(data);
    logTextEdit->append(receivedData);

    bool ok;
    double yValue = receivedData.toDouble(&ok);
    if (ok) {
        static double xValue = 0;  // Для оси X

        customPlot->graph(0)->addData(xValue, yValue);
        customPlot->replot();

        addValue(yValue);
        multiplyValue(yValue);

        xValue += 1;
    }
}

void MainWindow::selectComPort()
{
    bool ok;
    QString portName = QInputDialog::getText(this, tr("Select COM Port"),
                                             tr("Port Name:"), QLineEdit::Normal, "COM1", &ok);
    if (ok && !portName.isEmpty()) {
        QMetaObject::invokeMethod(serialReader, "startReading", Qt::QueuedConnection, Q_ARG(QString, portName));
    }
}

void MainWindow::selectEthernetAddress()
{
    bool ok;
    QString host = QInputDialog::getText(this, tr("Enter Ethernet Address"),
                                         tr("Host IP:"), QLineEdit::Normal, "localhost", &ok);
    quint16 port = QInputDialog::getInt(this, tr("Enter Port"), tr("Port:"), 1234, 0, 65535, 1, &ok);
    if (ok && !host.isEmpty()) {
        QMetaObject::invokeMethod(ethernetReader, "startTcpConnection", Qt::QueuedConnection, Q_ARG(QString, host), Q_ARG(quint16, port));
    }
}

void MainWindow::startDataEmitter()
{
    // Запуск эмулятора с определенными параметрами
    dataEmitter->setAmplitude(1.0); // Задаем амплитуду
    dataEmitter->setFrequency(1.0); // Задаем частоту
    dataEmitter->setPhase(0.0);      // Задаем фазу
    dataEmitter->startSending("localhost", 1234); // Начинаем отправку на localhost:1234
}

void MainWindow::stopDataEmitter()
{
    dataEmitter->stopSending(); // Остановить отправку данных
}

void MainWindow::addValue(double newValue)
{
    accumulatedSum += newValue;  // Обновляем сумму
    logTextEdit->append(QString("Sum: %1").arg(accumulatedSum)); // Выводим сумму в лог

    static double sumXValue = 0; // Статическая переменная для оси X на графике суммы
    sumPlot->graph(0)->addData(sumXValue, accumulatedSum);
    sumPlot->replot(); // Перерисовываем график суммы
    sumXValue += 1;    // Увеличиваем значение X для следующего чтения
}

void MainWindow::multiplyValue(double newValue)
{
    if (accumulatedProduct == 0) { // Инициализируем на первом значении
        accumulatedProduct = newValue;
    } else {
        accumulatedProduct *= newValue; // Обновляем произведение
    }
    logTextEdit->append(QString("Product: %1").arg(accumulatedProduct)); // Выводим произведение в лог

    static double productXValue = 0; // Статическая переменная для оси X на графике произведения
    productPlot->graph(0)->addData(productXValue, accumulatedProduct);
    productPlot->replot(); // Перерисовываем график произведения
    productXValue += 1;    // Увеличиваем значение X для следующего чтения
}
