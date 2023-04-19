#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include<QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWidget; }
QT_END_NAMESPACE

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    ClientWidget(QWidget *parent = nullptr);
    ~ClientWidget();


private slots:
    void on_buttonconnect_clicked();
    void on_buttonsend_clicked();
    void on_buttonclose_clicked();
    void joint1Pwm_valueChanged();
    void joint2Pwm_valueChanged();
    void joint3Pwm_valueChanged();
    void joint4Pwm_valueChanged();
    void joint5Pwm_valueChanged();
    void joint6Pwm_valueChanged();
    void reachOut_grasp(void);
    void getUP_show(void);
    void turnLeft_release(void);
    void turnRight_release(void);
    void resetDestination(void);
    QString toActionstring(int joint_num,int time,int tcp_flag,int adjust_flag=0,int pwm_value=1500);

private:
    Ui::ClientWidget *ui;
    QTcpSocket *tcpsocket;//声明套接字 客户端只有一个通信套接字
    int times[6]={1000,1000,1000,1000,1000,1000};
    int actionGroup_flag=0;
};
#endif // CLIENTWIDGET_H
