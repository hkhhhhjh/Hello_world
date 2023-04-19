#include "clientwidget.h"
#include "ui_clientwidget.h"
#include "qspinbox.h"
#include "QDebug"
#include "windows.h"

ClientWidget::ClientWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWidget)
{
    //这一行是新加上去的
    ui->setupUi(this);
    tcpsocket=nullptr;
    setWindowTitle("客户端");
    //初始化关节pwm滑块
    ui->joint_1->setRange(500,2500);
    ui->joint_2->setRange(500,2500);
    ui->joint_3->setRange(500,2500);
    ui->joint_4->setRange(500,2500);
    ui->joint_5->setRange(500,2500);
    ui->joint_6->setRange(500,2500);
    ui->joint_1->setValue(1500);
    ui->joint_2->setValue(1500);
    ui->joint_3->setValue(1500);
    ui->joint_4->setValue(1500);
    ui->joint_5->setValue(1500);
    ui->joint_6->setValue(1500);
    ui->joint_1->setEnabled(true);
    ui->joint_2->setEnabled(true);
    ui->joint_3->setEnabled(true);
    ui->joint_4->setEnabled(true);
    ui->joint_5->setEnabled(true);
    ui->joint_6->setEnabled(true);
    ui->spinBox->setRange(0,9999);
    ui->spinBox_2->setRange(0,9999);
    ui->spinBox_3->setRange(0,9999);
    ui->spinBox_4->setRange(0,9999);
    ui->spinBox_5->setRange(0,9999);
    ui->spinBox_6->setRange(0,9999);
    ui->spinBox->setValue(1500);
    ui->spinBox_2->setValue(1500);
    ui->spinBox_3->setValue(1500);
    ui->spinBox_4->setValue(1500);
    ui->spinBox_5->setValue(1500);
    ui->spinBox_6->setValue(1500);
    //初始化一个socket
    tcpsocket=new QTcpSocket(this);
    //函数地址
    void  (ClientWidget:: *reachOut_grasp_pt)(void) = &ClientWidget::reachOut_grasp;
    void  (ClientWidget:: *getUP_show_pt)(void) = &ClientWidget::getUP_show;
    void  (ClientWidget:: *turnLeft_release_pt)(void) = &ClientWidget::turnLeft_release;
    void  (ClientWidget:: *turnRight_release_pt)(void) = &ClientWidget::turnRight_release;
    void  (ClientWidget:: *resetDestination_pt)(void) = &ClientWidget::resetDestination;
    //连接事件
    connect(ui->joint_1,SIGNAL(valueChanged(int)),this,SLOT(joint1Pwm_valueChanged()));
    connect(ui->joint_2,SIGNAL(valueChanged(int)),this,SLOT(joint2Pwm_valueChanged()));
    connect(ui->joint_3,SIGNAL(valueChanged(int)),this,SLOT(joint3Pwm_valueChanged()));
    connect(ui->joint_4,SIGNAL(valueChanged(int)),this,SLOT(joint4Pwm_valueChanged()));
    connect(ui->joint_5,SIGNAL(valueChanged(int)),this,SLOT(joint5Pwm_valueChanged()));
    connect(ui->joint_6,SIGNAL(valueChanged(int)),this,SLOT(joint6Pwm_valueChanged()));

    connect(ui->reachOut,&QPushButton::clicked,this,reachOut_grasp_pt);
    connect(ui->getUP,&QPushButton::clicked,this,getUP_show_pt);
    connect(ui->turnLeft,&QPushButton::clicked,this,turnLeft_release_pt);
    connect(ui->turnRight,&QPushButton::clicked,this,turnRight_release_pt);
    connect(ui->reset,&QPushButton::clicked,this,resetDestination_pt);

    connect(tcpsocket,&QTcpSocket::connected,[=](){
        ui->textEditRead->setText("服务器连接成功！");
    });

    connect(tcpsocket,&QTcpSocket::readyRead,[=](){
       //获取通信套接字的内容
        QString str=tcpsocket->readAll();
        //在显示编辑区域显示
       ui->textEditRead->append("服务器端："+str);//不用settext 这样会覆盖之前的消息
    });

}

ClientWidget::~ClientWidget()
{
    delete ui;
}

//取10的整数倍
int convertToOCT(int num)
{
    if(num%10<5){
        return (num/10)*10;
    }else if(num%10>=5){
        return ((num/10)+1)*10;
    }
}

void ClientWidget::on_buttonconnect_clicked()
{
    if(nullptr==ui->lineEditIP || nullptr==ui->lineEditPort)
        return ;
    //获取IP地址和端口号
    QString IP=ui->lineEditIP->text();
    quint16 Port=ui->lineEditPort->text().toInt();

    //与服务器连接
    tcpsocket->connectToHost(IP,Port);
}

void ClientWidget::on_buttonsend_clicked()
{
    if(nullptr==tcpsocket)//连接失败则不发送
        return;

    //获取发送的信息
    QString str=ui->textEditWrite->toPlainText();

    //将信息写入到通信套接字
    tcpsocket->write(str.toUtf8().data());

    //将自己的信息显示在聊天窗口
    ui->textEditRead->append("客户端："+str);//不用settext 这样会覆盖之前的消息
}


void ClientWidget::on_buttonclose_clicked()
{
    if(nullptr==tcpsocket)
        return;
    tcpsocket->disconnectFromHost();//断开与服务器的连接
    tcpsocket->close();//关闭通信套接字
}

//封装执行一次动作所需要的数据报文
//p1 第几个关节 p2 调节时间 p3 需不需要tcp发送
QString ClientWidget::toActionstring(int joint_num,int time,int tcp_flag,int adjust_flag,int pwm_value)
{
    QString str,temp;
    str.append("#00");
    temp.sprintf("%d",joint_num-1);
    str.append(temp);
    str.append("P");
    //需要调节
    if(adjust_flag==1){
        switch(joint_num)
        {
        case 1:
            ui->joint_1->setValue(pwm_value);
            break;
        case 2:
            ui->joint_2->setValue(pwm_value);
            break;
        case 3:
            ui->joint_3->setValue(pwm_value);
            break;
        case 4:
            ui->joint_4->setValue(pwm_value);
            break;
        case 5:
            ui->joint_5->setValue(pwm_value);
            break;
        case 6:
            ui->joint_6->setValue(pwm_value);
            break;
        }
    }
    switch(joint_num)
    {
    case 1:
        temp.sprintf("%04d",ui->joint_1->value());
        break;
    case 2:
        temp.sprintf("%04d",ui->joint_2->value());
        break;
    case 3:
        temp.sprintf("%04d",ui->joint_3->value());
        break;
    case 4:
        temp.sprintf("%04d",ui->joint_4->value());
        break;
    case 5:
        temp.sprintf("%04d",ui->joint_5->value());
        break;
    case 6:
        temp.sprintf("%04d",ui->joint_6->value());
        break;
    }
    str.append(temp);
    str.append("T");
    temp.sprintf("%d",time);
    str.append(temp);
    str.append("!");

    //如果使能了tcp_flag
    if(tcp_flag==1){
        //将信息写入通信套接字
        tcpsocket->write(str.toUtf8().data());
        //显示发送的报文
        ui->textEditRead->append(str);
        return str;
    }
    else{
        return str;
    }
}

void ClientWidget::joint1Pwm_valueChanged()
{
//    static int cnt1=0;
    if(ui->joint_1->value()>2500){
        ui->joint_1->setValue(2500);
    }else if(ui->joint_1->value()<500){
        ui->joint_1->setValue(500);
    }
    //取整
    ui->joint_1->setValue(convertToOCT(ui->joint_1->value()));
    //显示
    ui->spinBox->setValue(ui->joint_1->value());
//    cnt1++;
//    if(cnt1>=2){
//        qDebug()<<"get 10";
//        cnt1=0;
//    }
    //经过测试 tcp通信的频率可以与一次滑块值的改变频率同步
    //如果没有在执行动作组则发送
    if(actionGroup_flag==0)
        toActionstring(1,times[0],1);
}

void ClientWidget::joint2Pwm_valueChanged()
{
    if(ui->joint_2->value()>2500){
        ui->joint_2->setValue(2500);
    }else if(ui->joint_2->value()<500){
        ui->joint_2->setValue(500);
    }
    ui->joint_2->setValue(convertToOCT(ui->joint_2->value()));
    ui->spinBox_2->setValue(ui->joint_2->value());

    if(actionGroup_flag==0)
        toActionstring(2,times[0],1);
}

void ClientWidget::joint3Pwm_valueChanged()
{
    if(ui->joint_3->value()>2500){
        ui->joint_3->setValue(2500);
    }else if(ui->joint_3->value()<500){
        ui->joint_3->setValue(500);
    }
    ui->joint_3->setValue(convertToOCT(ui->joint_3->value()));
    ui->spinBox_3->setValue(ui->joint_3->value());

    if(actionGroup_flag==0)
        toActionstring(3,times[0],1);
}

void ClientWidget::joint4Pwm_valueChanged()
{
    if(ui->joint_4->value()>2500){
        ui->joint_4->setValue(2500);
    }else if(ui->joint_4->value()<500){
        ui->joint_4->setValue(500);
    }
    ui->joint_4->setValue(convertToOCT(ui->joint_4->value()));
    ui->spinBox_4->setValue(ui->joint_4->value());

    if(actionGroup_flag==0)
        toActionstring(4,times[0],1);
}

void ClientWidget::joint5Pwm_valueChanged()
{
    if(ui->joint_5->value()>2500){
        ui->joint_5->setValue(2500);
    }else if(ui->joint_5->value()<500){
        ui->joint_5->setValue(500);
    }
    ui->joint_5->setValue(convertToOCT(ui->joint_5->value()));
    ui->spinBox_5->setValue(ui->joint_5->value());

    if(actionGroup_flag==0)
        toActionstring(5,times[0],1);
}

void ClientWidget::joint6Pwm_valueChanged()
{
    if(ui->joint_6->value()>2500){
        ui->joint_6->setValue(2500);
    }else if(ui->joint_6->value()<500){
        ui->joint_6->setValue(500);
    }
    ui->joint_6->setValue(convertToOCT(ui->joint_6->value()));
    ui->spinBox_6->setValue(ui->joint_6->value());

    if(actionGroup_flag==0)
        toActionstring(6,times[0],1);
}
//目前的设计 即使是一个完整的动作组 也不能做先后 只能一次性做完
//问题应该是出在延时上面
//需要设置一个延时标志位 这个可以留着后期作为改进
//前倾
//{G0001#000P1500T1000!#001P1780T1000!#002P1020T1000!#003P1780T1000!#004P1500T1000!#005P1620T1000!}
//高举
//{G0002#000P1500T1000!#001P1380T1000!#002P1380T1000!#003P1820T1000!#004P0860T1000!#005P1620T1000!}
//复位
//{G0000#000P1500T1000!#001P1500T1000!#002P1500T1000!#003P1500T1000!#004P1500T1000!#005P1500T1000!}

//动作组抓取
//前倾夹紧 G001
void ClientWidget::reachOut_grasp(void)
{
    int adjust_params[6]={1500,1780,1020,1780,1500,1620};
    actionGroup_flag=1;
    QString str,temp;
    //前倾夹紧 G0001
    str.append("{G0001");
    for(int i=0;i<6;i++){
        temp=toActionstring(i+1,1000,0,1,adjust_params[i]);
        str.append(temp);
    }
    str.append("}");
    //tcp发送数据
    tcpsocket->write(str.toUtf8().data());
    ui->textEditRead->append(str);
    actionGroup_flag=0;
}

//抬起手臂
void ClientWidget::getUP_show(void)
{
    int adjust_params[6]={1500,1380,1380,1820,860,1620};\
    actionGroup_flag=1;
    QString str,temp;
    //抬起手臂 G0002
    str.append("{G0002");
    for(int i=0;i<6;i++){
        temp=toActionstring(i+1,1000,0,1,adjust_params[i]);
        str.append(temp);
    }
    str.append("}");
    //tcp发送数据
    tcpsocket->write(str.toUtf8().data());
    ui->textEditRead->append(str);
    actionGroup_flag=0;
}

//左转下放
void ClientWidget::turnLeft_release(void)
{
    int adjust_params[6]={2100,1580,740,1780,1500,1260};
    actionGroup_flag=1;
    QString str,temp;
    //左转下放 G0003
    str.append("{G0003");
    for(int i=0;i<6;i++){
        temp=toActionstring(i+1,2000,0,1,adjust_params[i]);
        str.append(temp);
    }
    str.append("}");
    //tcp发送数据
    tcpsocket->write(str.toUtf8().data());
    ui->textEditRead->append(str);
    actionGroup_flag=0;
}

//右转下放
void ClientWidget::turnRight_release(void)
{
    int adjust_params[6]={860,1580,740,1780,1500,1260};
    actionGroup_flag=1;
    QString str,temp;
    //右转下放 G0004
    str.append("{G0004");
    for(int i=0;i<6;i++){
        temp=toActionstring(i+1,2000,0,1,adjust_params[i]);
        str.append(temp);
    }
    str.append("}");
    //tcp发送数据
    tcpsocket->write(str.toUtf8().data());
    ui->textEditRead->append(str);
    actionGroup_flag=0;
}

//复位
void ClientWidget::resetDestination(void)
{
    int adjust_params[6]={1500,1500,1500,1500,1500,1500};
    actionGroup_flag=1;
    QString str,temp;
    //复位 G0005
    str.append("{G0005");
    for(int i=0;i<6;i++){
        temp=toActionstring(i+1,2000,0,1,adjust_params[i]);
        str.append(temp);
    }
    str.append("}");
    //tcp发送数据
    tcpsocket->write(str.toUtf8().data());
    ui->textEditRead->append(str);
    actionGroup_flag=0;
}

