#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QColor>
#include <QTimer>
#include <QPainter>
#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>
#include <QIODevice>
#include <QFileDevice>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTime>
#include <QtWidgets/QTimeEdit>
#include <QDateTime>
#include "qdatetime.h"
#include <iostream>
#include <qtemporaryfile.h>
#include <Windows.h>
#include <QFloat16>
#include <QKeyEvent>
#include <QPointF>
#include <QString>
#include <QPaintEvent>
#include <QPlainTextEdit>
#include <bits/stdc++.h>
#include <linechart.h>
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    bool evenFilter(QObject *obj,
                    QEvent  *event);
    ~MainWindow();

public slots:

    void handleTimeout();
    void PowerRateWindow();

private slots:

    /*
     * UI 函数
     */
    void on_open_serial_clicked();         // 打开串口接收

    void on_ClearDataButton_clicked();     // 清屏

    void on_SendDataButton_clicked();      // 发送数据

    void on_save_receive_data_clicked();   // 保存接收到的数据

    void on_refresh_serial_port_clicked(); // 刷新串口

    void on_Open_data_clicked();           // 显示接收到的数据

    /*
     * 自定义函数
     */

    void Read_Data();                       // 读取数据

    void Dispose_buf_data_jcy();            // 处理JCY数据

    bool warrning();                        // 警告

    void Search_Serial_Port();              // 搜寻串口

    void loadfile(const QString& filename); // 读取文件

signals:

    //    void Send_Data_To_MainWindow(QString te);
    //    void Send_Angle_Data_To_MainWindow(QString signal, float te);
    //    void Send_Num_Data_To_MainWindow(QPointF te);

protected:

    void keyPressEvent(QKeyEvent *event);

    //    void paintEvent(QPaintEvent *event);

private:

    Ui::MainWindow *ui;

    /*
     * 需要清零
     */

    QByteArray buf, Check, erc, data; // buf:存储串口接收到的字符串
    long long ItemIndex = 0;
    bool Save_file_data_flag = false; // 判断是否已经建立存储文件

    /*
     * 不需要清零
     */

    QSerialPort *serial;       // 串口
    QString Save_filename;     // 文件存储路径+文件名
    QString Open_filename;     // 文件存储路径
    QString str_time;          // 存储时间
    QDateTime time;            // 获取当前时间
    QDir *dir = new QDir();    // 文件夹对象
    QFile *file = new QFile(); // 文件对象
    QString data_number;       // 数据个数
    QTimer *UpdatePainterTimer;
};


#endif // MAINWINDOW_H
