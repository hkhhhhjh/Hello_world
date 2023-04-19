import time
import network
import machine
from machine import Pin,UART  # ---- 添加 --------
from umqttsimple import MQTTClient


uart = machine.UART(1, 9600)
strmsg = ''
def do_connect():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        wlan.connect('看到我请让我去学习', '47578ca55851')
        i = 1
        while not wlan.isconnected():
            print("正在链接...{}".format(i))
            i += 1
            time.sleep(1)
    print('network config:', wlan.ifconfig())


def sub_cb(topic, msg): # 回调函数，收到服务器消息后会调用这个函数
    print(topic, msg)
    # ---- 添加 --------
    if topic.decode("utf-8") == "ledctl" and msg.decode("utf-8") == "on":
        print('ON')
    elif topic.decode("utf-8") == "ledctl" and msg.decode("utf-8") == "off":
        print('OFF')
    # ---- 添加 --------


# 1. 联网
do_connect()

# 2. 创建mqt

c = MQTTClient("umqtt_client", "192.168.43.239")  # 建立一个MQTT客户端
c.set_callback(sub_cb)  # 设置回调函数
c.connect()  # 建立连接
c.subscribe(b"wrt")  # 监控这个通道，接收控制命令



while True:
    ds = ''
    dht = ''
    rp = ''
    dhh = ''
    dhh1 = ''
    if uart.any() > 0:
        strmsg = uart.readline()
    print(strmsg)
    ds = strmsg[2:6]
    dht = strmsg[9:11]
    rp = strmsg[13:17]
    dhh = strmsg[20:22]
    
    c.publish(b"DS18B20",ds)
    c.publish(b"DHT11_temp",dht)
    c.publish(b"RP2040",rp)
    c.publish(b"DHT11_hum",dhh)

    c.check_msg()
    time.sleep(1)
