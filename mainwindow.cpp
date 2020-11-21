#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->sector_dis->installEventFilter(this);
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
    qDebug()<<tr("界面设定成功");
    qDebug()<<tr("搜寻串口成功");
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
//    ui->sector_dis->update();
}

void MainWindow::on_open_serial_clicked()
{
    //判断文件
    if(ui->open_serial->text()==tr("打开串口")&&warrning())
    {
        serial= new QSerialPort;
        //设置串口名
        serial->setPortName(ui->PortBox->currentText());
        qDebug()<<tr("设置串口名成功");
        //打开串口
        serial->open(QIODevice::ReadWrite);
        qDebug()<<tr("打开串口成功");
        //设置波特率
        serial->setBaudRate(ui->BaudBox->currentText().toInt());
        //设置数据位

        switch (ui->BitBox->currentIndex())
        {
        case 0:
            serial->setDataBits(QSerialPort::Data8);
            qDebug() << tr("设置数据位成功");
            break;
        }
        //设置奇偶校验位
        QString ParityStrValue=ui->ParityBox->currentText();
        switch (ui->ParityBox->currentIndex())
        {
        case 0:
            serial->setParity(QSerialPort::NoParity);
            qDebug()<<tr("设置奇偶校验位成功");
            break;
        case 1:
            serial->setParity(QSerialPort::OddParity);
            //            cout<<"设置偶校验位成功"<<endl;
            //            qDebug()<<QString::fromLocal8Bit("设置偶校验位成功");
            qDebug()<<tr("设置偶校验位成功");
            break;
        case 2:
            serial->setParity(QSerialPort::EvenParity);
            qDebug()<<tr("设置奇校验位成功");
            break;
        default:break;
        }
        //设置停止位
        switch (ui->StopBox->currentIndex())
        {
        case 0:
            serial->setStopBits(QSerialPort::OneStop);
            qDebug()<<tr("设置停止位0成功");
            break;
        case 1:
            serial->setStopBits(QSerialPort::TwoStop);
            qDebug()<<tr("设置停止位1成功");
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
        ui->open_serial->setText(tr("关闭串口"));
        //按钮功能失能
        ui->save_receive_data->setEnabled(false);
        ui->refresh_serial_port->setEnabled(false);
        ui->Open_data->setEnabled(false);
        //按钮功能使能
         ui->SendDataButton->setEnabled(true);
        qDebug()<<tr("打开串口成功");
        //连接信号槽
        QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::Read_Data);

    }
    else if(ui->open_serial->text()==tr("关闭串口"))
    {
        /*  emit Send_Data_To_MainWindow("port_close")*/
        // times++;
        //关闭串口
        serial->clear();
        serial->close();
        serial->deleteLater();
        //恢复设置使能
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->open_serial->setText(tr("打开串口"));
        ui->SendDataButton->setEnabled(false);
        time1=QDateTime::currentDateTime();
        ItemIndex=0;
        qDebug()<<tr("复位成功");
        //按钮功能使能
        ui->SendDataButton->setEnabled(true);
        ui->save_receive_data->setEnabled(true);
        ui->refresh_serial_port->setEnabled(true);
        ui->Open_data->setEnabled(true);
//        ItemIndex=1;
    }

}

void MainWindow::Read_Data()
{
    buf += serial->readAll();
    str_time=time1.toString("yyyy-MM-dd hh:mm:ss");
    Dispose_buf_data_jcy();
}

void MainWindow::on_ClearDataButton_clicked()
{
    ui->Receive_Window->clear();
}

void MainWindow::on_SendDataButton_clicked()
{
//    serial->write(ui->Send_Window->toPlainText().toLatin1());
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
        QMessageBox::information(nullptr,"critical",tr("没有选择存储路径"),QMessageBox::Yes);
        return false;
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
//        auto aa=info.portName();
        //qDebug() << a;
        if (a == tr("蓝牙链接上的标准串行")||a==tr("JLink CDC UART Port"))
        {
            continue;
        }
        else
        {
            if(ui->PortBox->findText(serial.portName())==-1){
                ui->PortBox->addItem(serial.portName());
              }
            serial.close();
        }
      }
}

void MainWindow::loadfile(const QString &filename)
{
  QFile file(filename);
  if (!file.open(QFile::ReadOnly|QFile::Text)){
      QMessageBox::warning(this,tr("Application"),tr("不能打开文件%1:\n%2.").arg(QDir::toNativeSeparators(filename),file.errorString()));
      return;
    }
  QTextStream in(&file);
  ui->Send_Window->setPlainText(in.readAll());
}

void MainWindow::Dispose_buf_data_jcy()
{
    if (buf.size()>=10){
        int flag=0;
        char jiaoyanhe='\x00';
        for (int i=0;i<buf.size();i++) {
            switch (i) {
            case 0:
                if(buf[i]=='\x55'&&flag==0){
                    flag=1;
                }else{
                    flag=0;
                }
                break;
            case 8:
                if (jiaoyanhe==buf[i]&&flag==1){
                    qDebug()<<ItemIndex<<" "<<tr("校验合格，校验和正确");
                    //                ui->Receive_Window->append(tr("校验合格，校验和正确"));
                }else{
                    qDebug()<<tr("校验失败，校验和不正确");
                    //                ui->Receive_Window->append(tr("校验失败，校验和不正确"));
                }
                break;
            case 9:
                if (flag==1){
                    if (buf[i]=='\xcc'){
                        ui->Receive_Window->append(QString::number(ItemIndex) + "\t"+ tr(" 数据正确"));
                        ui->BandA_R->display(QString::number(ItemIndex));
                    }else{
                        ui->Receive_Window->append(QString::number(ItemIndex) + "\t"+ tr(" 数据错误"));
                    }
                }
                break;
            default:
                if(flag==1){
                    jiaoyanhe+=buf[i];
                    jiaoyanhe&='\xff';
                }
            }
        }
        ItemIndex+=1;
        Save_filename=Open_filename+" " +time1.toString("hh.mm.ss")+".txt";
        QFile file(Save_filename);
        if(!file.open(QIODevice::WriteOnly|QIODevice::Append))
        {
            file.close();
        }
        QTextStream in(&file);
        in << QString::number(ItemIndex)<<"\t";
        QString a=buf.toHex();
        for(int i=0;i<a.size();i+=2){
            in<<"0x"+a.mid(i,2)<<" ";
          }
//        in<<QString::fromStdString(a.toStdString())<<" ";
        in<< "\r\n";
        file.close();
        buf.clear();

    }
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

void MainWindow::on_Open_data_clicked()
{
//  QDir temp(Save_filename);
  QString filename=QFileDialog::getOpenFileName(this);
  if(!filename.isEmpty())
    loadfile(filename);
}
