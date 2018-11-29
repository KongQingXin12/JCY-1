#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sector_dis->installEventFilter(this);
    UpdatePainterTimer =new QTimer;
    UpdatePainterTimer->setInterval(500);
    connect(UpdatePainterTimer,&QTimer::timeout,this,&MainWindow::handleTimeout);
    UpdatePainterTimer->start();

    //查找可用串口
    //emit Send_Data_To_MainWindow("111");
    Search_Serial_Port();
    //设置波特率下拉菜单默认显示第三项
    //ui->BaudBox->setCurrentIndex(3);
    ui->dis_num_sdj->setPalette(Qt::white);
    ui->dis_ang_sdj->setPalette(Qt::white);
    //关闭发送按钮使能

    ui->SendDataButton->setEnabled(false);
    qDebug()<<QString::fromLocal8Bit("界面设定成功");
    qDebug()<<QString::fromLocal8Bit("搜寻串口成功");
}

bool MainWindow::evenFilter(QObject *obj, QEvent *event)
{

    if(event->type()==QEvent::Timer)
    {
        handleTimeout();
    }
    return QWidget::eventFilter(obj,event);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleTimeout()
{
    ui->sector_dis->update();
}

void MainWindow::on_open_serial_clicked()
{
    //判断文件
    if(ui->open_serial->text()==QString::fromLocal8Bit("打开串口")&&warrning())
    {
        serial= new QSerialPort;
        //设置串口名
        serial->setPortName(ui->PortBox->currentText());
        qDebug()<<QString::fromLocal8Bit("设置串口名成功");
        //打开串口
        serial->open(QIODevice::ReadWrite);
        qDebug()<<QString::fromLocal8Bit("打开串口成功");
        //设置波特率
        serial->setBaudRate(ui->BaudBox->currentText().toInt());
        //设置数据位

        switch (ui->BitBox->currentIndex())
        {
        case 0:
            serial->setDataBits(QSerialPort::Data8);
            qDebug() << QString::fromLocal8Bit("设置数据位成功");
            break;
        }
        //设置奇偶校验位
        switch (ui->ParityBox->currentIndex())
        {
        case 0:
            serial->setParity(QSerialPort::NoParity);
            qDebug()<<QString::fromLocal8Bit("设置奇偶校验位成功");
            break;
        default:break;
        }
        //设置停止位
        switch (ui->StopBox->currentIndex())
        {
        case 0:
            serial->setStopBits(QSerialPort::OneStop);
            qDebug()<<QString::fromLocal8Bit("设置停止位0成功");
            break;
        case 1:
              serial->setStopBits(QSerialPort::TwoStop);
              qDebug()<<QString::fromLocal8Bit("设置停止位1成功");
            break;
        default:break;
        }

        //设置控制流
        serial->setFlowControl(QSerialPort::NoFlowControl);
        //关闭设置菜单使能
        ui->PortBox->setEnabled(false);
        ui->PortBox->setEnabled(false);
        ui->BaudBox->setEnabled(false);
        ui->BitBox->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->StopBox->setEnabled(false);
        ui->open_serial->setText(QString::fromLocal8Bit("关闭串口"));
        ui->SendDataButton->setEnabled(true);
        qDebug()<<QString::fromLocal8Bit("打开串口成功");
        //连接信号槽
        QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::Read_Data);

    }
    else if(ui->open_serial->text()==QString::fromLocal8Bit("关闭串口"))
    {
      /*  emit Send_Data_To_MainWindow("port_close")*/
       // times++;
        //关闭串口
        serial->clear();
        serial->close();
        serial->deleteLater();
        //恢复设置使能
       // ui->PortBox->setEnabled(true);
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->open_serial->setText(QString::fromLocal8Bit("打开串口"));
        ui->SendDataButton->setEnabled(false);
        file->close();
    }

}

void MainWindow::Read_Data()
{
    buf += serial->readAll();
    str_time=time1.toString("yyyy-MM-dd hh:mm:ss");
    Dispose_buf_data();
}

void MainWindow::on_ClearDataButton_clicked()
{
    ui->Receive_Window->clear();
}

void MainWindow::on_SendDataButton_clicked()
{
    serial->write(ui->Send_Window->toPlainText().toLatin1());
}

void MainWindow::on_save_receive_data_clicked()
{
    QString temppath=dir->currentPath();
    //qDebug() << temppath;
    QDir temp(temppath);
    if(!temp.exists("temp"))
    {
        temp.mkdir(tr("temp"));
    }
    Open_filename= temppath+"/temp";
    Open_filename+="/";
    qDebug()<<Open_filename;

}

void MainWindow::on_refresh_serial_port_clicked()
{
    Search_Serial_Port();
}

bool MainWindow::warrning()
{
    if(Open_filename==QString())
    {
        QMessageBox::information(nullptr,"critical",QString::fromLocal8Bit("没有选择存储路径"),QMessageBox::Yes);
        return false;
    }
    return true;
}

bool MainWindow::data_Check(QByteArray data, QByteArray erc)
{
    QString d=data.toHex();
    QString e=erc.toHex();
    auto i=d.rbegin();
    auto j=e.rbegin();
    for(;j!=e.rend();i++,j++)
    {
        if(*i!=*j)
        {
            return false;
        }
    }
    return true;
}

void MainWindow::Search_Serial_Port()
{
    foreach(const  QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        auto a = info.description();
        //qDebug() << a;
        if (a == QString::fromLocal8Bit("蓝牙链接上的标准串行"))
        {
            continue;
        }
        else
        {
            ui->PortBox->addItem(serial.portName());
            serial.close();
        }
    }
}


void MainWindow::Dispose_buf_data()
{

    if(buf.size()>=16)
    {
        for(int i=0;i<buf.size();i++)
        {
            if(buf[i]=='\x55')
            {
                step=1;
            }
            else if(buf[i]=='\xaa')
            {
                step=2;
            }
            else if(step==2)
            {
                te='\x00';
                switch (buf[i])
                {
                case '\x01':
                    te='\x01';
                    AD_Number="AD_01";
                    step=3;
                    break;
                case '\02':
                    te='\x02';
                    AD_Number="AD_02";
                    step=3;
                    break;
                case '\x03':
                    te='\x03';
                    AD_Number="AD_03";
                    break;
                    step=3;
                    break;
                case '\x04':
                    te='\x04';
                    AD_Number="AD_04";
                    step=3;
                    break;
                case '\x06':
                    te='\x06';
                    AD_Number="AD_06";
                    step=3;
                    break;
                }
            }
            else if(step==3)
            {
                while (data.size()<4)
                {
                    data += buf[i];
                    te+=buf[i];
                    i++;
                }
                if(data.size()==4)
                {
                    te=te&'\xff';
                    Check+=te;
                    step=4;
                    te='\x00';
                    i--;
                }
            }
            else if(step==4)
            {
                erc+=buf[i];
                step++;
                if (step == 5)
                {
                    if (data_Check(Check, erc))
                    {
                        times++;
                        string ss = data.toHex().toStdString();
                        long long a = 0;
                        for (auto i = ss.rbegin(); i !=ss.rend(); i++)
                        {
                            int n = distance(ss.rbegin(), i);
                            if (*i >= 'a'&&*i <= 'f')
                            {
                                a += pow(16, n)*(*i - 'a' + 10);
                            }
                            else
                            {
                                a += pow(16, n)*(*i - '0');
                            }
                        }
                        data.clear();
                        ss=to_string(a);
                        data_number=QString::fromStdString(ss);
                        //显示处理结果
                        /*emit Send_Data_To_MainWindow("start")*/;
                        /*emit Send_Num_Data_To_MainWindow(QPointF(times,a))*/;
                        ui->Receive_Window->clear();
                        ui->Receive_Window->append(QString::fromStdString(to_string(a)));
                        ui->Receive_Window->append(QString::fromLocal8Bit("校验成功，数据合格"));
                        /*LCD显示处理结果-整数*/
                        ui->dis_num_sdj->display(data_number);
                        /*LCD显示处理结果-角度*/

                        if(a<=61440)
                        {
                            angle=(float)a*12/4096;
                        }
                        else
                        {
                            angle=((float)a-122880)*12/4096;
                        }

                        ui->dis_ang_sdj->display(QString("%1").arg(angle));
                        /*emit Send_Angle_Data_To_MainWindow("start",angle)*/;//将计算好的角度信息实时传递给主窗口
                        /*Draw_Sector()*/;
                        /*建立存储文件*/
                        Save_filename=Open_filename+AD_Number+" " +time1.toString("hh.mm.ss")+".txt";
                        QFile file(Save_filename);
                        if(!file.open(QIODevice::WriteOnly|QIODevice::Append))
                        {
                           file.close();
                        }
                        QTextStream in(&file);
                        in << str_time + " " + "55aa" +" "<< a << " " + erc.toHex() << "\r\n";
                        file.close();
                    }
                    else
                    {
                        if(data.size()==4)
                            data.clear();
                        ui->Receive_Window->clear();
                        ui->Receive_Window->append(QString::fromLocal8Bit("校验失败"));
                    }
                    if (buf.size() == 8)
                        buf.clear();
                    else
                        buf = buf.mid(i);
                    step = 0;
                    Check.clear();
                    erc.clear();
                    break;
                }

            }
        }
    }
    else if(step==0&&erc==nullptr&&Check==nullptr)
    {
        times=0;
        ui->Receive_Window->clear();
        ui->Receive_Window->append(QString::fromLocal8Bit("串口停止发送数据"));
    }

}

void MainWindow::paintEvent(QPaintEvent *event)
{
    /* 绘制第一个圆环 */
    QPainter painter1(this);
    //painter1.begin(this);
    painter1.setRenderHint(QPainter::Antialiasing);//设置圆滑绘制风格（抗锯齿）
   //绘制圆环
    float m_persent1 = angle+90;//此处我画80%
    int m_rotateAngle1 = 180* (1 - m_persent1 / 180);
    //int side1 = qMin(width(), height());
    //定义矩形绘制区域
    QRectF outRect1(50, 80, 200, 200);
    QRectF inRect1(70, 100, 200 - 40, 200 - 40);
    //转换需要绘出的值
    QString valueStr1 = QString("%1"+QString::fromLocal8Bit("度")).arg(QString::number(angle));

    //画外圆
    painter1.setPen(Qt::NoPen);
    painter1.setBrush(QBrush(QColor(255, 107, 107)));//红色
    painter1.drawPie(outRect1, 0 * 16, 180 * 16);
    //画内圆
    painter1.setBrush(QBrush(QColor(97, 117, 118)));//黑色
    painter1.drawPie(outRect1, 0 * 16, m_rotateAngle1 * 16);
    //画遮罩，遮罩颜色为窗口颜色
    painter1.setBrush(palette().window().color());
    painter1.drawEllipse(inRect1);
    //画文字
    QFont f1 = QFont("Microsoft YaHei", 15, QFont::Bold);
    painter1.setFont(f1);
    painter1.setFont(f1);
    painter1.setPen(QColor("#555555"));
    painter1.drawText(inRect1, Qt::AlignCenter, valueStr1);
    //painter1.end();
}

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



