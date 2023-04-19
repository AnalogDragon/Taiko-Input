#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QDebug"


MainWindow::~MainWindow()
{
    delete ui;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new Serial();

    /**/
    _id_list.clear();
    _id_list.append(ui->ID1);
    _id_list.append(ui->ID2);
    _id_list.append(ui->ID3);
    _id_list.append(ui->ID4);
    _id_list.append(ui->ID5);
    _id_list.append(ui->ID6);
    _id_list.append(ui->ID7);
    _id_list.append(ui->ID8);
    //设定ID设置
    for( QLineEdit *p : _id_list )
    {
        QRegExp RegExp("[1-8]");
        p->setValidator(new QRegExpValidator(RegExp, this));
        p->setAttribute(Qt::WA_InputMethodEnabled, false);
        p->installEventFilter(this);
        qDebug()<<p->objectName();

        connect(p,&QLineEdit::textEdited,this,[=](const QString &text){
           qDebug()<<"selection"<<static_cast<QLineEdit*>(sender())->objectName()<<"text"<<text;
           int sel = _id_list.indexOf(static_cast<QLineEdit*>(sender()));
           if( sel == -1 ) return;
           if( text.isEmpty() ) return;
           sel ++;
           sel = ( sel >= _id_list.length()) ? 0 : sel;
           _id_list.at(sel)->setFocus();
        });
    }

    //定时触发全选
    selall_t = new QTimer();
    connect(selall_t,&QTimer::timeout,this,[=](){
        if( select != -1 )
        {
            _id_list.at(select)->selectAll();
        }
        selall_t->stop();
    });

    waitdown_t = new QTimer();
    connect(waitdown_t,&QTimer::timeout,this,[=](){
        this->on_BT_SV_clicked(true);
        waitdown_t->stop();
    });


    /**/
    _id_tg_list.clear();
    _id_tg_list.append(ui->ID1_TG);
    _id_tg_list.append(ui->ID2_TG);
    _id_tg_list.append(ui->ID3_TG);
    _id_tg_list.append(ui->ID4_TG);
    _id_tg_list.append(ui->ID5_TG);
    _id_tg_list.append(ui->ID6_TG);
    _id_tg_list.append(ui->ID7_TG);
    _id_tg_list.append(ui->ID8_TG);

    _id_tg_list.append(ui->ID1_TG_mul);
    _id_tg_list.append(ui->ID2_TG_mul);
    _id_tg_list.append(ui->ID3_TG_mul);
    _id_tg_list.append(ui->ID4_TG_mul);
    _id_tg_list.append(ui->ID5_TG_mul);
    _id_tg_list.append(ui->ID6_TG_mul);
    _id_tg_list.append(ui->ID7_TG_mul);
    _id_tg_list.append(ui->ID8_TG_mul);

    _id_tg_list.append(ui->LB_H);
    _id_tg_list.append(ui->LB_L);
    _id_tg_list.append(ui->LB_MT_TIME);

    _id_tg_slider_list.clear();
    _id_tg_slider_list.append(ui->SL1);
    _id_tg_slider_list.append(ui->SL2);
    _id_tg_slider_list.append(ui->SL3);
    _id_tg_slider_list.append(ui->SL4);
    _id_tg_slider_list.append(ui->SL5);
    _id_tg_slider_list.append(ui->SL6);
    _id_tg_slider_list.append(ui->SL7);
    _id_tg_slider_list.append(ui->SL8);

    _id_tg_slider_list.append(ui->SL1_mul);
    _id_tg_slider_list.append(ui->SL2_mul);
    _id_tg_slider_list.append(ui->SL3_mul);
    _id_tg_slider_list.append(ui->SL4_mul);
    _id_tg_slider_list.append(ui->SL5_mul);
    _id_tg_slider_list.append(ui->SL6_mul);
    _id_tg_slider_list.append(ui->SL7_mul);
    _id_tg_slider_list.append(ui->SL8_mul);

    _id_tg_slider_list.append(ui->SL_H);
    _id_tg_slider_list.append(ui->SL_L);
    _id_tg_slider_list.append(ui->SL_MT_TIME);

    //关联滑条和输入框
    for(int i=0;i<_id_tg_slider_list.length();i++)
    {
        QRegExp RegExp("[0-9]+");
        _id_tg_list[i]->setValidator(new QRegExpValidator(RegExp, this));
        _id_tg_list[i]->setAttribute(Qt::WA_InputMethodEnabled, false);
        _id_tg_list[i]->installEventFilter(this);
        qDebug()<<_id_tg_list[i]->objectName();

        connect(_id_tg_list[i],&QLineEdit::textEdited,this,[=](const QString &text){
           TextSetFlag1 = true;
           _id_tg_slider_list[i]->setValue(text.toInt());
        });

        connect(_id_tg_slider_list[i],&QSlider::valueChanged,this,[=](int value){
            if(TextSetFlag1 == true){
                TextSetFlag1 = false;
                return;
            }
           _id_tg_list[i]->setText(QString::number(value));
        });
    }

    //一次性全更改
    QRegExp RegExp("[0-9]+");
    ui->ALL_TG->setValidator(new QRegExpValidator(RegExp, this));
    ui->ALL_TG->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->ALL_TG->installEventFilter(this);
    connect(ui->ALL_TG,&QLineEdit::editingFinished,this,[=](){
        if(ui->ALL_TG->text().isEmpty())return;
        int temp = ui->ALL_TG->text().toInt();
        for(int i=0;i<8;i++)
        {
            _id_tg_slider_list[i]->setValue(temp);
        }
        for(int i=0;i<8;i++)
        {
            _id_tg_list[i]->setText(QString::number(temp));
        }
        ui->ALL_TG->clear();
    });

    //选择框
    connect(ui->CK_en_mute,&QCheckBox::stateChanged,this,[=](int state){
        qDebug()<<"checkBox"<<QString::number(state);
        if(0 != state){
            ui->CK_en_SSO->setEnabled(true);
            ui->LB_MT_TIME->setEnabled(true);
            ui->SL_MT_TIME->setEnabled(true);
            ui->label_16->setEnabled(true);
            ui->label_17->setEnabled(true);
        }
        else
        {
            ui->CK_en_SSO->setEnabled(false);
            ui->LB_MT_TIME->setEnabled(false);
            ui->SL_MT_TIME->setEnabled(false);
            ui->label_16->setEnabled(false);
            ui->label_17->setEnabled(false);
        }
    });

    //串口号显示
    ScanCom();
    connect(ui->COM_LB,&ClickedComboBox::clicked,this,&MainWindow::ScanCom);

    //串口回传状态
    connect(serial,&Serial::serialMessage,this,[=](QString text){
        ui->LB_Log->setText(text);
    });

    connect(serial,&Serial::serialCplt,this,[=](bool state){
        DispResult(state);
        EnableButton(true);
    });


    connect(serial,&Serial::serialData,this,&MainWindow::PutUiData);

    QLocale locale;

    if(locale.language() != QLocale::Chinese){
        ui->pushButton_2->setText("Download taiko force settings");
        ui->pushButton->setText("Download taiko 12th settings");
    }

}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type()==QEvent::FocusIn)
    {
        bool select_flag = false;
        for( QLineEdit *p : _id_list )
        {
            if( watched == p )
            {
                qDebug()<<"Focus"<<watched->objectName();
                select = _id_list.indexOf(p);
                select_flag = true;
                selall_t->start(10);
            }
        }
        if( select_flag == false )
        {
            select = -1;
        }
    }
    return QWidget::eventFilter(watched,event);
}


void MainWindow::EnableButton(bool state)
{
    qDebug()<<"set button"<<state;
    ui->BT_UP->setEnabled(state);
    ui->BT_DN->setEnabled(state);
    ui->BT_SV->setEnabled(state);
    ui->COM_LB->setEnabled(state);
}

bool MainWindow::CheckCom(void)
{
    qDebug()<<"COM number:"<<ui->COM_LB->count();
    if(ui->COM_LB->currentText() == "无串口")
    {
        return true;
    }
    return false;
}

void MainWindow::on_BT_UP_clicked(bool checked)
{
    QByteArray arreyTemp;
    Q_UNUSED(checked)
    qDebug()<<"Upload Button";
    ui->LB_Log->setText("连接"+ui->COM_LB->currentText());
    if(CheckCom())return;

    arreyTemp.clear();
    arreyTemp.append(char(0x2B));
    serial->AddToList(quint8(ui->comboBox->currentIndex() | CMD_ACK_DEF_SET) ,arreyTemp);

    if(serial->SerialStart(ui->COM_LB->itemData(ui->COM_LB->currentIndex()).toString()))
        EnableButton(false);

    ui->LB_Log_2->clear();
}

void MainWindow::on_BT_DN_clicked(bool checked)
{
    QByteArray arreyTemp;
    Q_UNUSED(checked)
    qDebug()<<"Download Button";
    ui->LB_Log->setText("连接"+ui->COM_LB->currentText());
    if(CheckCom())return;

    arreyTemp.clear();
    arreyTemp.append(GetUiData());
    if(arreyTemp.length() == 0)
    {
        return;
    }
    serial->AddToList(CMD_SEND_SET ,arreyTemp);

    if(serial->SerialStart(ui->COM_LB->itemData(ui->COM_LB->currentIndex()).toString()))
        EnableButton(false);

    ui->LB_Log_2->clear();
}

void MainWindow::on_BT_SV_clicked(bool checked)
{
    QByteArray arreyTemp;
    Q_UNUSED(checked)
    qDebug()<<"Save Button";
    ui->LB_Log->setText("连接"+ui->COM_LB->currentText());
    if(CheckCom())return;

    arreyTemp.clear();
    arreyTemp.append(GetUiData());
    serial->AddToList(CMD_SEND_SET ,arreyTemp);

    arreyTemp.clear();
    arreyTemp.append(char(0x2B));
    if(arreyTemp.length() == 0)
    {
        return;
    }
    serial->AddToList(CMD_SYS_SAVE ,arreyTemp);
    serial->AddToList(CMD_SYS_RESET ,arreyTemp);

    if(serial->SerialStart(ui->COM_LB->itemData(ui->COM_LB->currentIndex()).toString()))
        EnableButton(false);

    ui->LB_Log_2->clear();
}

void MainWindow::ScanCom(void)
{
    ui->COM_LB->clear();
    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();

    if(infos.isEmpty())
    {
        ui->COM_LB->addItem("无串口");
        ui->COM_LB->setCurrentIndex(0);
        return;
    }

    foreach (QSerialPortInfo info, infos)
    {
        ui->COM_LB->addItem(info.portName()+" "+info.description(),info.portName());
    }

    for(int i = 0; i < ui->COM_LB->count(); i++)
    {
        QString temp = ui->COM_LB->itemText(i);
        temp.replace("CH340","");
        if(temp != ui->COM_LB->itemText(i)){
            ui->COM_LB->setCurrentIndex(i);
        }
    }
}



void MainWindow::PutUiData(quint8 cmd,QByteArray array)
{
    if((cmd & 0xF0) != 0xB0 || array.length() != 48)return;
    qDebug()<<"Get UI Data";

    quint16 temp0;

    //Get H and L
    temp0 = quint8(array[0]);
    ui->SL_H->setValue(temp0);
    ui->LB_H->setText(QString ::number (temp0));

    temp0 = quint8(array[1]);
    ui->SL_L->setValue(temp0);
    ui->LB_L->setText(QString ::number (temp0));

    qDebug()<<"Set H="<<ui->LB_H->text()<<" L="<<ui->LB_L->text();


    //Get id trigger mul
    for(int i=0;i<8;i++)
    {
        _id_list[i]->setText(QString ::number (array[2+i]+1));

        temp0 = quint8(array[22+2*i]) + quint8(array[23+2*i])*256;

        _id_tg_slider_list[i]->setValue(temp0);
        _id_tg_list[i]->setText(QString ::number (temp0));

        temp0 = quint8(array[10+i]);

        _id_tg_slider_list[8+i]->setValue(temp0);
        _id_tg_list[8+i]->setText(QString ::number (temp0));

        qDebug()<<"Set ID="<<_id_list[i]->text()<<" value="<<_id_tg_list[i]->text()\
               <<"MUL="<<_id_tg_list[8+i]->text();
    }

    //Get Mul set
    temp0 = quint16(array[19]);
    ui->CK_en_SSO->setChecked(temp0 != 0);

    temp0 = quint16(array[18]);
    ui->CK_en_mute->setChecked(temp0 != 0);

    temp0 = quint16(array[20]);
    ui->SL_MT_TIME->setValue(temp0);
    ui->LB_MT_TIME->setText(QString ::number (temp0));

    qDebug()<<"Set Mute EN="<<ui->CK_en_mute->isChecked()\
           <<" SSO="<<ui->CK_en_mute->isChecked()\
          <<" Time="<<ui->LB_MT_TIME->text();

    DataOK = true;
}



QByteArray MainWindow::GetUiData(void)
{
    QByteArray SendData;
    quint16 temp0,temp1;

    SendData.clear();

    if(CheckUIData())
    {
        DispResult(false);
        return SendData;
    }

    temp0 = quint16(ui->LB_H->text().toUInt());
    temp1 = quint16(ui->LB_L->text().toUInt());
    SendData.append(char(temp0 & 0xFF));    //Hi
    SendData.append(char(temp1 & 0xFF));    //Lo

    //ID
    for(quint8 i=0;i<8;i++)
    {
        temp0 = quint16(_id_list[i]->text().toUInt() - 1);
        SendData.append(char(temp0&0xFF));
    }

    //MUL
    for(quint8 i=0;i<8;i++)
    {
        temp0 = quint16(_id_tg_list[8+i]->text().toUInt());
        SendData.append(char(temp0&0xFF));
    }

    //MuteEN
    temp0 = ui->CK_en_mute->isChecked() == true;
    SendData.append(char(temp0&0xFF));

    //SSO_EN
    temp0 = ui->CK_en_SSO->isChecked() == true;
    SendData.append(char(temp0&0xFF));


    //Mute Time
    temp0 = quint16(ui->LB_MT_TIME->text().toUInt());
    SendData.append(char(temp0&0xFF));

    SendData.append(char(0xCC));

    //Trigger
    for(quint8 i=0;i<8;i++)
    {
        temp0 = quint16(_id_tg_list[i]->text().toUInt());
        SendData.append(char(temp0&0xFF));
        SendData.append(char((temp0>>8)&0xFF));
    }


    //Flush data
    for(quint8 i=0;i<10;i++)
    {
        SendData.append(char(0xCC));
    }

    qDebug()<<"Length = "<<SendData.length();

    return SendData;
}

void MainWindow::DispResult(bool state)
{
    if(state)
    {
        ui->LB_Log_2->setStyleSheet("color:green;");
        ui->LB_Log_2->setText("Success");
    }
    else
    {
        ui->LB_Log_2->setStyleSheet("color:red;");
        ui->LB_Log_2->setText("Failed");
    }
}

bool MainWindow::CheckUIData(void)
{
    //Check ID Num
    for(quint8 i=0;i<8;i++){
      quint8 count = 0;
      for(quint8 j=0;j<8;j++){
        if(quint16(_id_list[j]->text().toUInt() - 1) == i)count++;
      }
      if(count != 1)
      {
          //data err
          qDebug()<<"ID="<<i+1<<" count="<<count;
          ui->LB_Log->setText("ID=1 to 8");
          return true;
      }
    }

    //Check mul
    for(quint16 i=0;i<8;i++)
    {
        if(_id_tg_list[i+8]->text().toUInt() > 255)
        {
            //data err
            ui->LB_Log->setText("MUL=0~255");
            return true;
        }
    }

    //check H and L
    quint16 temp0 = quint16(ui->LB_H->text().toUInt());
    quint16 temp1 = quint16(ui->LB_L->text().toUInt());
    if(temp0 > 50 || temp0 < 2 || temp1 > 50 || temp1 < 2)
    {
        //data err
        ui->LB_Log->setText("H or L=2~50");
        return true;
    }

    //Check mute time
    if(ui->LB_MT_TIME->text().toUInt() > 50)// || ui->LB_MT_TIME->text().toUInt() < 1)
    {
        //data err
        ui->LB_Log->setText("Time=0~50");
        return true;
    }

    return false;
}



void MainWindow::on_BT_ClearMul_clicked(bool checked)
{
    Q_UNUSED(checked)
    qDebug()<<"ClearMul Button";
    ui->SL1_mul->setValue(100);
    ui->SL2_mul->setValue(100);
    ui->SL3_mul->setValue(100);
    ui->SL4_mul->setValue(100);
    ui->SL5_mul->setValue(100);
    ui->SL6_mul->setValue(100);
    ui->SL7_mul->setValue(100);
    ui->SL8_mul->setValue(100);

    ui->ID1_TG_mul->setText("100");
    ui->ID2_TG_mul->setText("100");
    ui->ID3_TG_mul->setText("100");
    ui->ID4_TG_mul->setText("100");
    ui->ID5_TG_mul->setText("100");
    ui->ID6_TG_mul->setText("100");
    ui->ID7_TG_mul->setText("100");
    ui->ID8_TG_mul->setText("100");
}


void MainWindow::on_pushButton_2_clicked()
{
    //大力鼓
    ui->ID1->setText("1");
    ui->ID2->setText("4");
    ui->ID3->setText("2");
    ui->ID4->setText("3");
    ui->ID5->setText("5");
    ui->ID6->setText("8");
    ui->ID7->setText("6");
    ui->ID8->setText("7");

    for(int i=0;i<8;i++)
    {
        _id_tg_slider_list[i]->setValue(15);
        _id_tg_list[i]->setText(QString::number(15));
    }
    on_BT_ClearMul_clicked(NULL);

    ui->SL_H->setValue(3);
    ui->LB_H->setText(QString::number(3));
    ui->SL_L->setValue(3);
    ui->LB_L->setText(QString::number(3));
    ui->CK_en_mute->setCheckState(Qt::Checked);
    ui->CK_en_SSO->setCheckState(Qt::Checked);
    ui->SL_MT_TIME->setValue(12);
    ui->LB_MT_TIME->setText(QString::number(12));

    ScanCom();

    QString temp = ui->COM_LB->currentText();
    temp.replace("CH340","");
    if(temp == ui->COM_LB->currentText()){
        for(int i = 0; i < ui->COM_LB->count(); i++)
        {
            temp = ui->COM_LB->itemText(i);
            temp.replace("CH340","");
            if(temp != ui->COM_LB->itemText(i)){
                ui->COM_LB->setCurrentIndex(i);
            }
        }
    }

    waitdown_t->start(20);

}



void MainWindow::on_pushButton_clicked()
{
    //街机鼓
    ui->ID1->setText("5");
    ui->ID2->setText("6");
    ui->ID3->setText("7");
    ui->ID4->setText("8");
    ui->ID5->setText("1");
    ui->ID6->setText("2");
    ui->ID7->setText("3");
    ui->ID8->setText("4");

    for(int i=0;i<8;i++)
    {
        _id_tg_slider_list[i]->setValue(15);
        _id_tg_list[i]->setText(QString::number(15));
    }
    on_BT_ClearMul_clicked(NULL);

    ui->SL_H->setValue(3);
    ui->LB_H->setText(QString::number(3));
    ui->SL_L->setValue(3);
    ui->LB_L->setText(QString::number(3));
    ui->CK_en_mute->setCheckState(Qt::Checked);
    ui->CK_en_SSO->setCheckState(Qt::Checked);
    ui->SL_MT_TIME->setValue(12);
    ui->LB_MT_TIME->setText(QString::number(12));

    ScanCom();

    QString temp = ui->COM_LB->currentText();
    temp.replace("CH340","");
    if(temp == ui->COM_LB->currentText()){
        for(int i = 0; i < ui->COM_LB->count(); i++)
        {
            temp = ui->COM_LB->itemText(i);
            temp.replace("CH340","");
            if(temp != ui->COM_LB->itemText(i)){
                ui->COM_LB->setCurrentIndex(i);
            }
        }
    }

    waitdown_t->start(20);


}


///////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
///
///////////////////////////////////////////////////////////////////////////////////////////////




Serial::Serial(void)
{
    _sendTimer = new QTimer;
    _serial = new QSerialPort;

    SendList.clear();

    connect(_serial,&QSerialPort::errorOccurred,this,[=](QSerialPort::SerialPortError error){
        if( error == QSerialPort::ResourceError)
        {
            qDebug()<<"errorOccurred";
            SerialAbrot();
            emit serialMessage("连接失败");
        }
    },Qt::AutoConnection);

    connect(_sendTimer,&QTimer::timeout,this,&Serial::SendFream);

    connect(_serial,&QSerialPort::readyRead,this,&Serial::serialRead);


}


void Serial::SerialPortConnect(bool state)
{
    if(_serial->isOpen()){
        if(state == false)
        {
            qDebug()<<"Seria Close";
            _serial->close();
            emit serialMessage("Stop");
            SerialState = false;
        }
    }
    else
    {
        if(state == true)
        {
            qDebug()<<"Seria Opening";
            _serial->setBaudRate(38400,QSerialPort::AllDirections);
            _serial->setDataBits(QSerialPort::Data8);
            _serial->setFlowControl(QSerialPort::NoFlowControl);
            _serial->setParity(QSerialPort::NoParity);
            _serial->setStopBits(QSerialPort::OneStop);
            _serial->setPortName(PortName);
            if( !_serial->open(QIODevice::ReadWrite))
            {
                emit serialMessage("连接失败");
                SerialAbrot();
                return ;
            }
            _serial->clear();
            emit serialMessage("Running");
        }
    }
}

bool Serial::SerialStart(QString text)
{
    if(SendList.isEmpty())return 0;
    PortName = text;
    qDebug()<<text;
    SerialPortConnect(true);
    if(!_serial->isOpen())return 0;

    _sendTimer->start(500);
    return 1;
}


void Serial::SendFream(void)
{
    if(!SendList.isEmpty())
    {
        QString datastr;
        for(char data : SendList.first().Data)
        {
            datastr.append(QString("%1 ").arg(0xFF&data,2,16,QLatin1Char('0')));
        }
        qDebug()<<"[T]Send"<<SendList.first().count<<SendList.first().times<<datastr;


        if(SendList.first().count < 1)
        {
            if(quint8(SendList.first().Data[0]) == CMD_SYS_RESET){
                RecData.clear();
                SendList.removeFirst();
                if(SendList.isEmpty())
                {
                    qDebug()<<"CMD_SYS_RESET at END";
                    SerialPortConnect(false);
                    _sendTimer->stop();
                    emit serialCplt(true);
                }
                else
                {
                    _serial->write(SendList.first().Data);
                    _sendTimer->start(SendList.first().times);
                }

            }
            else
            {
                qDebug()<<"TimeOut";
                SerialAbrot();
            }
        }
        else
        {
            RecData.clear();
            SendList.first().count--;
            _serial->write(SendList.first().Data);
            _sendTimer->start(SendList.first().times);
            SerialState = true;
        }
    }
    else
    {
        qDebug()<<"List Empty";
        SerialAbrot();
    }
}

void Serial::AddToList(quint8 cmd,QByteArray array)
{
    struct SendPack_DEF Temp;
    quint16 CrcTemp;

    if(cmd == CMD_SYS_RESET){
        Temp.count = 2;
        Temp.times = 200;
    }
    else
    {
        Temp.count = 10;
        Temp.times = 1000;
    }
    Temp.Data.append(char(cmd));
    Temp.Data.append(array);

    //crc
    CrcTemp = CRCCheck(Temp.Data);
    Temp.Data.append(char(CrcTemp>>8));
    Temp.Data.append(char(CrcTemp));

    SendList.append(Temp);
}




quint16 Serial::CRCCheck(QByteArray data){
    quint16 tempCrcHigh = 0xFF;
    quint16 tempCrcLow = 0xFF;
    quint16 tempIndex = 0;

    const quint16 CRCVALUE[256]=
    {
        0x0000,0xC1C0,0x81C1,0x4001,0x01C3,0xC003,0x8002,0x41C2,0x01C6,0xC006,0x8007,0x41C7,
        0x0005,0xC1C5,0x81C4,0x4004,0x01CC,0xC00C,0x800D,0x41CD,0x000F,0xC1CF,0x81CE,0x400E,
        0x000A,0xC1CA,0x81CB,0x400B,0x01C9,0xC009,0x8008,0x41C8,0x01D8,0xC018,0x8019,0x41D9,
        0x001B,0xC1DB,0x81DA,0x401A,0x001E,0xC1DE,0x81DF,0x401F,0x01DD,0xC01D,0x801C,0x41DC,
        0x0014,0xC1D4,0x81D5,0x4015,0x01D7,0xC017,0x8016,0x41D6,0x01D2,0xC012,0x8013,0x41D3,
        0x0011,0xC1D1,0x81D0,0x4010,0x01F0,0xC030,0x8031,0x41F1,0x0033,0xC1F3,0x81F2,0x4032,
        0x0036,0xC1F6,0x81F7,0x4037,0x01F5,0xC035,0x8034,0x41F4,0x003C,0xC1FC,0x81FD,0x403D,
        0x01FF,0xC03F,0x803E,0x41FE,0x01FA,0xC03A,0x803B,0x41FB,0x0039,0xC1F9,0x81F8,0x4038,
        0x0028,0xC1E8,0x81E9,0x4029,0x01EB,0xC02B,0x802A,0x41EA,0x01EE,0xC02E,0x802F,0x41EF,
        0x002D,0xC1ED,0x81EC,0x402C,0x01E4,0xC024,0x8025,0x41E5,0x0027,0xC1E7,0x81E6,0x4026,
        0x0022,0xC1E2,0x81E3,0x4023,0x01E1,0xC021,0x8020,0x41E0,0x01A0,0xC060,0x8061,0x41A1,
        0x0063,0xC1A3,0x81A2,0x4062,0x0066,0xC1A6,0x81A7,0x4067,0x01A5,0xC065,0x8064,0x41A4,
        0x006C,0xC1AC,0x81AD,0x406D,0x01AF,0xC06F,0x806E,0x41AE,0x01AA,0xC06A,0x806B,0x41AB,
        0x0069,0xC1A9,0x81A8,0x4068,0x0078,0xC1B8,0x81B9,0x4079,0x01BB,0xC07B,0x807A,0x41BA,
        0x01BE,0xC07E,0x807F,0x41BF,0x007D,0xC1BD,0x81BC,0x407C,0x01B4,0xC074,0x8075,0x41B5,
        0x0077,0xC1B7,0x81B6,0x4076,0x0072,0xC1B2,0x81B3,0x4073,0x01B1,0xC071,0x8070,0x41B0,
        0x0050,0xC190,0x8191,0x4051,0x0193,0xC053,0x8052,0x4192,0x0196,0xC056,0x8057,0x4197,
        0x0055,0xC195,0x8194,0x4054,0x019C,0xC05C,0x805D,0x419D,0x005F,0xC19F,0x819E,0x405E,
        0x005A,0xC19A,0x819B,0x405B,0x0199,0xC059,0x8058,0x4198,0x0188,0xC048,0x8049,0x4189,
        0x004B,0xC18B,0x818A,0x404A,0x004E,0xC18E,0x818F,0x404F,0x018D,0xC04D,0x804C,0x418C,
        0x0044,0xC184,0x8185,0x4045,0x0187,0xC047,0x8046,0x4186,0x0182,0xC042,0x8043,0x4183,
        0x0041,0xC181,0x8180,0x4040,
    };

    for(char _data : data){
        tempIndex = tempCrcLow ^ (quint16(_data) & 0xFF);
        tempCrcLow = tempCrcHigh ^ (CRCVALUE[tempIndex] >>8);
        tempCrcHigh = CRCVALUE[tempIndex] & 0xff;
    }
    return (tempCrcHigh | quint16(tempCrcLow<<8));
}


quint8 Serial::DataLen(quint8 cmd)
{
    switch(cmd)
    {

    case CMD_SYS_RESET:
        return 0;

    default:
    case CMD_FAULT:
    case CMD_SYS_SAVE:
    case CMD_ACK:
    case CMD_SEND_SET:
    case CMD_CALL_DEF:
        return 4;

    case CMD_DEBUG_LOG:
        return 19;

    case CMD_ACK_DEF_SET:
    case CMD_ACK_SAVE_SET:
    case CMD_ACK_NOW_SET:
        return 51;

    }
}

quint8 Serial::DataCalc(quint8 cmd,QByteArray array)
{
    switch(cmd)
    {
    default:
    case CMD_FAULT:
        return 2;

    case CMD_SYS_RESET:
        return 0;

    case CMD_DEBUG_LOG:
        if(array.length() == DataLen(cmd) - 3)
        {
            return 0;
        }
        else
        {
            return 2;
        }

    case CMD_ACK_DEF_SET:
    case CMD_ACK_SAVE_SET:
    case CMD_ACK_NOW_SET:
        if(array.length() == DataLen(cmd) - 3)
        {
            return 0;
        }
        else
        {
            return 2;
        }

    case CMD_SYS_SAVE:
        if(quint8(array[0]) != 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }

    case CMD_ACK:
    case CMD_SEND_SET:
    case CMD_CALL_DEF:
        if(quint8(array[0]) != 0)
        {
            return 0;
        }
        else
        {
            return 2;
        }
    }

}

void Serial::serialRead(void)
{

    RecData.append(_serial->readAll());
    if( SendList.isEmpty() || SerialState == false)
    {
        qDebug()<<"Empty and Rec"<<RecData.data();
        RecData.clear();
        return;
    }

    //获取当前发送的cmd
    quint8 cmd = quint8(SendList.first().Data.at(0)&0xff);
    for(int i=0;i<RecData.length();i++)
    {
        if( quint8(RecData[0]) != quint8(SendList.first().Data.at(0)&0xff))
        {
            qDebug()<<"Error Head"<<quint8(RecData[0]);
            if(RecData.length() > 1)
                RecData = RecData.right(RecData.length()-1);
            else
                RecData.clear();
        }
        else
            break;
    }


    int len = DataLen(cmd);
    if( RecData.length() < len )
    {
//        qDebug()<<"Wait for reviced now "<<RecData.length();
        return;
    }

    quint16 crcdata = CRCCheck(RecData.left(len-2));
    if(((RecData.at(len-2)&0xff) != (quint8(crcdata >> 8)&0xff))||(( RecData.at(len-1)&0xff) != quint8(crcdata & 0x00ff )))
    {
        qDebug()<<"[E]Revice crc is error"<<QString("crc is %1 %2,should be %3 %4")\
                  .arg(RecData.at(len-2)&0xff,2,16,QLatin1Char('0'))\
                  .arg(RecData.at(len-1)&0xff,2,16,QLatin1Char('0'))\
                  .arg((crcdata>>8)&0xff,2,16,QLatin1Char('0'))\
                  .arg(crcdata&0xff,2,16,QLatin1Char('0'));

        RecData.clear();
        return;
    }
    qDebug()<<"CRC OK";

    cmd = quint8(RecData.at(0)&0xff);
    QByteArray reviced_array = RecData.mid(1,len-3);

    QString datastr;
    for(char data : reviced_array)
    {
        datastr.append(QString("%1 ").arg(0xFF&data,2,16,QLatin1Char('0')));
    }
    qDebug()<<"[R]Revice data is "<<datastr;

    if(2 == DataCalc(cmd,reviced_array))
    {
        qDebug()<<"Data False Stop";
        SerialAbrot();
        return;
    }
    else if(1 == DataCalc(cmd,reviced_array))
    {
        qDebug()<<"Data False Continue";
        return;
    }

    SendList.removeFirst();
    emit serialData(cmd,reviced_array);

    //接收完全了
    if( SendList.isEmpty())
    {
        SerialPortConnect(false);
        _sendTimer->stop();
        RecData.clear();
        SendList.clear();
        emit serialCplt(true);
        _sendTimer->stop();
    }
}



void Serial::SerialAbrot(void)
{
    qDebug()<<"Abrot Send";

    SerialPortConnect(false);
    _sendTimer->stop();
    RecData.clear();
    SendList.clear();
    emit serialCplt(false);
}


