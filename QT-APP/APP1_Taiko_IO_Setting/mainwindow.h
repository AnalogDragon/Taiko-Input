#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QSerialPortInfo>
#include "QIntValidator"
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QThread>
#include <QList>

#include <QLocale>


#define CMD_FAULT             0x10
#define CMD_ACK               0x20

#define CMD_SYS_RESET         0xA0
#define CMD_SYS_SAVE          0xA1

#define CMD_ACK_DEF_SET       0xB0
#define CMD_ACK_SAVE_SET      0xB1
#define CMD_ACK_NOW_SET       0xB2

#define CMD_SEND_SET          0xC0
#define CMD_CALL_DEF          0xC1

#define CMD_DEBUG_LOG         0xD1


class Serial;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ScanCom(void);
    void EnableButton(bool state);
    void PutUiData(quint8 cmd,QByteArray array);
    QByteArray GetUiData(void);
    void DispResult(bool state);
    void ClearResult(void);
    bool CheckCom(void);
    bool CheckUIData(void);



    QList<QLineEdit *> _id_list;
    QList<QLineEdit *> _id_tg_list;
    QList<QSlider *> _id_tg_slider_list;
    int select = -1;
    QTimer *selall_t;
    QTimer *waitdown_t;
    bool TextSetFlag1 = false;
    bool DataOK = false;
    Serial *serial;



private:
    Ui::MainWindow *ui;

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_BT_UP_clicked(bool checked);
    void on_BT_DN_clicked(bool checked);
    void on_BT_SV_clicked(bool checked);

    void on_BT_ClearMul_clicked(bool checked);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};




class Serial : public QObject
{
    Q_OBJECT

public:
    Serial();
    ~Serial(){}

    void SerialPortConnect(bool state);
    void SerialSend(QString text, bool state);
    void AddToList(quint8 cmd,QByteArray array);
    void SerialTimer(void);
    quint16 CRCCheck(QByteArray data);
    bool SerialStart(QString text);
    void SendFream(void);
    quint8 DataCalc(quint8 cmd,QByteArray array);
    void SerialAbrot(void);
    quint8 DataLen(quint8 cmd);


    struct SendPack_DEF{
        QByteArray Data;
        int count;
        int times;
    };

    QSerialPort* _serial;
    bool SerialState = false;
    QList<SendPack_DEF> SendList;
    QTimer* _sendTimer;
    QString PortName;
    QByteArray RecData;

signals:
    void serialCplt(bool state);
    void serialMessage(QString text);
    void ackToUi(quint8 cmd,QByteArray array);
    void serialData(quint8 cmd,QByteArray array);

public slots:
    void serialRead();
};





#endif // MAINWINDOW_H
