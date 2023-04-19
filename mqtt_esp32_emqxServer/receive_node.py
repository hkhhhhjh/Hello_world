import time
import network
from umqttsimple import MQTTClient

from machine import Pin,UART

#uart1=UART(1, baudrate=9600)#rx GP09 txGP10


def do_connect():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        wlan.connect('China-CMCC', '12345678')
        i = 1
        while not wlan.isconnected():
            print("正在链接...{}".format(i))
            i += 1
            time.sleep(1)
    print('network config:', wlan.ifconfig())


def sub_cb(topic, msg): # 回调函数，收到服务器消息后会调用这个函数
    getn=msg.decode()
    print(getn)
#     uart1.write(getn)

# 1. 联网
do_connect()
# 2. 创建mqt
c = MQTTClient("recieve_node", "192.168.43.48")  # 建立一个MQTT客户端
c.set_callback(sub_cb)  # 设置回调函数
c.connect()  # 建立连接
c.subscribe(b"topic")
msg="Hello World"

while True:
    c.check_msg()
    time.sleep(2)





