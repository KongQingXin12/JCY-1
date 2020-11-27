#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //    ui->sector_dis->installEventFilter(this);
    //    UpdatePainterTimer =new QTimer;
    //    UpdatePainterTimer->setInterval(500);
    //
    //  connect(UpdatePainterTimer,&QTimer::timeout,this,&MainWindow::handleTimeout);
    //    UpdatePainterTimer->start();
    auto PowerRate = ui->actionPowerRate;

    connect(PowerRate, &QAction::triggered, this, &MainWindow::PowerRateWindow);

    // 查找可用串口
    // emit Send_Data_To_MainWindow("111");
    Search_Serial_Port();

    // 设置波特率下拉菜单默认显示第三项
    // ui->BaudBox->setCurrentIndex(3);
    ui->dis_num_sdj->setPalette(Qt::white);
    ui->dis_ang_sdj->setPalette(Qt::white);

    // 关闭发送按钮使能

    ui->SendDataButton->setEnabled(false);
    qDebug() << tr("界面设定成功");
    qDebug() << tr("搜寻串口成功");
}

bool MainWindow::evenFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Timer)
    {
        handleTimeout();
    }
    return QWidget::eventFilter(obj, event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * UI函数
 */
void MainWindow::on_open_serial_clicked()
{
    // 判断文件
    if ((ui->open_serial->text() == tr("打开串口")) && warrning())
    {
        serial = new QSerialPort;

        // 设置串口名
        serial->setPortName(ui->PortBox->currentText());
        qDebug() << tr("设置串口名成功");

        // 打开串口
        serial->open(QIODevice::ReadWrite);
        qDebug() << tr("打开串口成功");

        // 设置波特率
        serial->setBaudRate(ui->BaudBox->currentText().toInt());

        // 设置数据位

        switch (ui->BitBox->currentIndex())
        {
        case 0:
            serial->setDataBits(QSerialPort::Data8);
            qDebug() << tr("设置数据位成功");
            break;
        }

        // 设置奇偶校验位
        QString ParityStrValue = ui->ParityBox->currentText();

        switch (ui->ParityBox->currentIndex())
        {
        case 0:
            serial->setParity(QSerialPort::NoParity);
            qDebug() << tr("设置奇偶校验位成功");
            break;

        case 1:
            serial->setParity(QSerialPort::OddParity);

            //            cout<<"设置偶校验位成功"<<endl;
            //            qDebug()<<QString::fromLocal8Bit("设置偶校验位成功");
            qDebug() << tr("设置偶校验位成功");
            break;

        case 2:
            serial->setParity(QSerialPort::EvenParity);
            qDebug() << tr("设置奇校验位成功");
            break;

        default: break;
        }

        // 设置停止位
        switch (ui->StopBox->currentIndex())
        {
        case 0:
            serial->setStopBits(QSerialPort::OneStop);
            qDebug() << tr("设置停止位0成功");
            break;

        case 1:
            serial->setStopBits(QSerialPort::TwoStop);
            qDebug() << tr("设置停止位1成功");
            break;

        default: break;
        }

        // 设置控制流
        serial->setFlowControl(QSerialPort::NoFlowControl);

        // 关闭设置菜单使能
        ui->PortBox->setEnabled(false);
        ui->PortBox->setEnabled(false);
        ui->BaudBox->setEnabled(false);
        ui->BitBox->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->StopBox->setEnabled(false);
        ui->open_serial->setText(tr("关闭串口"));

        //按钮功能失能
        ui->save_receive_data->setEnabled(false);
        ui->refresh_serial_port->setEnabled(false);
        ui->Open_data->setEnabled(false);

        //按钮功能使能
        ui->SendDataButton->setEnabled(true);
        qDebug() << tr("打开串口成功");

        // 连接信号槽
        QObject::connect(serial,
                         &QSerialPort::readyRead,
                         this,
                         &MainWindow::Read_Data);
    }
    else if (ui->open_serial->text() == tr("关闭串口"))
    {
        /*  emit Send_Data_To_MainWindow("port_close")*/

        // 关闭串口
        serial->clear();
        serial->close();
        serial->deleteLater();

        // 恢复设置使能
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->open_serial->setText(tr("打开串口"));
        ui->SendDataButton->setEnabled(false);
        time = QDateTime::currentDateTime();
        ItemIndex = 0;
        qDebug() << tr("复位成功");

        //按钮功能使能
        ui->SendDataButton->setEnabled(true);
        ui->save_receive_data->setEnabled(true);
        ui->refresh_serial_port->setEnabled(true);
        ui->Open_data->setEnabled(true);

        //        ItemIndex=1;
    }
}

void MainWindow::on_ClearDataButton_clicked()
{
    ui->Receive_Window->clear();
    ui->Reveive_data_display->clear();
}

void MainWindow::on_SendDataButton_clicked()
{
    //    serial->write(ui->Send_Window->toPlainText().toLatin1());
}

void MainWindow::on_save_receive_data_clicked()
{
    if (!Save_file_data_flag) {
        QString temppath = dir->currentPath();

        // qDebug() << temppath;
        QDir temp(temppath);

        if (!temp.exists("temp"))
        {
            temp.mkdir(tr("temp"));
        }
        Open_filename = temppath + "/temp";
        Open_filename += "/";
        qDebug() << Open_filename;
        time = QDateTime::currentDateTime();
        str_time = time.toString("yyyy.MM.dd.hh.mm.ss");
        Save_filename = Open_filename + str_time + ".txt";
        qDebug() << Save_filename;
        QFile file(Save_filename);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            file.close();
        }
        QTextStream in(&file);

        in << str_time << "\r\n";
        file.close();
        QMessageBox::information(nullptr,
                                 "提示信息",
                                 tr("数据存储路径设置成功"),
                                 QMessageBox::Yes);
        Save_file_data_flag = true;
    } else {
        QMessageBox::warning(nullptr,
                             "警告",
                             tr("数据存储路径已设置\n不要重复设置\n请打开串口接收数据"),
                             QMessageBox::Yes);
    }
}

void MainWindow::on_refresh_serial_port_clicked()
{
    Search_Serial_Port();
}

void MainWindow::on_Open_data_clicked()
{
    const QString currentdir = QDir::currentPath() + "/temp/";
    QString filename =
        QFileDialog::getOpenFileName(this, tr("打开缓存文件"), currentdir);

    if (!filename.isEmpty()) loadfile(filename);
}

/*
 * 自定义函数
 */
void MainWindow::Read_Data()
{
    buf += serial->readAll();
    Dispose_buf_data_jcy();
}

void MainWindow::Dispose_buf_data_jcy()
{
    if (buf.size() >= 11) {
        QString tmp_buf;
        int     flag = 0;
        char    jiaoyanhe = '\x00';

        for (int i = 0; i < buf.size(); i++) {
            switch (i) {
            case 0:

                if ((buf[i] == '\x50') && (flag == 0)) {
                    flag = 1;
                } else {
                    flag = 0;
                }
                break;

            case 9:

                if ((jiaoyanhe == buf[i]) && (flag == 1)) {
                    qDebug() << ItemIndex << " " << tr("校验合格，校验和正确");
                } else {
                    flag = 0;
                    qDebug() << ItemIndex << " " << tr("校验失败，校验和不正确");
                }
                break;

            case 10:

                if (flag == 1) {
                    if (buf[i] == '\xD0') {
                        ui->Receive_Window->append(QString::number(
                                                       ItemIndex) + "\t" +
                                                   tr(" 数据正确"));
                        ui->BandA_R->display(QString::number(ItemIndex));
                        ItemIndex += 1;
                    } else {
                        ui->Receive_Window->append(QString::number(
                                                       ItemIndex) + "\t" +
                                                   tr(" 数据错误"));
                    }
                }
                break;

            default:

                if (flag == 1) {
                    jiaoyanhe += buf[i];
                    jiaoyanhe &= '\xff';
                }
            }
        }

        if ((ItemIndex <= 1500) && (flag == 1)) {
            Save_filename = Open_filename + str_time + ".txt";
            QFile file(Save_filename);

            if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
            {
                file.close();
            }
            QTextStream in(&file);
            in << QString::number(ItemIndex) << "\t";
            QString buf_qstr = buf.toHex();


            for (int i = 0; i < buf_qstr.size(); i += 2) {
                in << "x" + buf_qstr.mid(i, 2) << " ";
            }

            //        in<<QString::fromStdString(a.toStdString())<<" ";
            in << "\r\n";
            file.close();
            flag = 0;
        }
        buf.clear();
        Save_file_data_flag = false;
    }
}

bool MainWindow::warrning()
{
    if (!Save_file_data_flag)
    {
        QMessageBox::information(nullptr,
                                 "critical",
                                 tr("没有选择存储路径"),
                                 QMessageBox::Yes);
        return false;
    }
    return true;
}

void MainWindow::Search_Serial_Port()
{
    foreach(const  QSerialPortInfo& info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;

        serial.setPort(info);
        auto a = info.description();

        //        auto aa=info.portName();
        // qDebug() << a;
        if ((a == tr("蓝牙链接上的标准串行")) || (a == tr("JLink CDC UART Port")))
        {
            continue;
        }
        else
        {
            if (ui->PortBox->findText(serial.portName()) == -1) {
                ui->PortBox->addItem(serial.portName());
            }
            serial.close();
        }
    }
}

void MainWindow::loadfile(const QString& filename)
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("不能打开文件%1:\n%2.").arg(QDir::toNativeSeparators(
                                                          filename),
                                                      file.errorString()));
        return;
    }
    QTextStream in(&file);

    ui->Reveive_data_display->clear();
    ui->Reveive_data_display->setPlainText(in.readAll());
}

/*
 * 信号函数
 */
void MainWindow::handleTimeout()
{
    //    ui->sector_dis->update();
}

void MainWindow::PowerRateWindow()
{
    auto linechart = new LineChart();
    const QString currentdir = QDir::currentPath() + "/temp/";
    QString filename_linechart = QFileDialog::getOpenFileName(this,
                                                              tr("打开缓存文件"),
                                                              currentdir);

    if (!filename_linechart.isEmpty()) {
        QFile datafile_linechart(filename_linechart);

        if (!datafile_linechart.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("Application"),
                                 tr("不能打开文件%1:\n%2.").arg(QDir::toNativeSeparators(
                                                              filename_linechart),
                                                          datafile_linechart.
                                                          errorString()));
            return;
        }
        QTextStream in(&datafile_linechart);
        linechart->show();
        linechart->disposeData(in);
        linechart->exec();
    }
}

/*
 * Protect函数
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        close();
        break;

    default:
        QMainWindow::keyPressEvent(event);
    }
}
