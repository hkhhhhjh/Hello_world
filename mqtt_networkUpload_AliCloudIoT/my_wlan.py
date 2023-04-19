from umqtt.simple import MQTTClient
from machine import Pin
import network
import time
import machine
import dht
from machine import Timer
 
SSID="nova 7 5G"
PASSWORD="1234567890.."
 
SERVER ='iot-06z00cvdueerv4n.mqtt.iothub.aliyuncs.com'
                             
CLIENT_ID = "hih0SLqDqQv.umqtt_cccccccccccc|securemode=2,signmethod=hmacsha256,timestamp=1658824219747|"   #设备ID
username="umqtt_cccccccccccc&hih0SLqDqQv"
password="059f116ced3b07530f910bbfb09756a054106d26219d5acb2946a4a7e8b9e264"
 
publish_TOPIC = '/sys/hih0SLqDqQv/umqtt_cccccccccccc/thing/event/property/post'
subscribe_TOPIC ='/sys/hih0SLqDqQv/umqtt_cccccccccccc/thing/event/property/post_reply'
 
client=None
mydht=None
def sub_cb(topic, msg):
    print((topic, msg))
 
def ConnectWifi(ssid,passwd):
    global wlan
    wlan=network.WLAN(network.STA_IF)         #create a wlan object
    wlan.active(True)                         #Activate the network interface
    wlan.disconnect()                         #Disconnect the last connected WiFi
    wlan.connect(ssid,passwd)                 #connect wifi
    while(wlan.ifconfig()[0]=='0.0.0.0'):
        time.sleep(1)
    print(wlan.ifconfig())
 
def apptimerevent(mytimer):
    try:
        sensordata=ReadTemHum()
        mymessage='{"params":{"CurrentTemperature":%d,"CurrentHumidity":%d},"method":"thing.event.property.post"}'%(sensordata[0],sensordata[1])
        client.publish(topic=publish_TOPIC,msg= mymessage, retain=False, qos=0)
    except Exception as ex_results2:
        print('exception',ex_results2)
        mytimer.deinit()
#     finally:
#         machine.reset()
 
def ReadTemHum():
    mydht.measure()
    tem=mydht.temperature()
    hum=mydht.humidity()
    data=[tem,hum]
    print(data)
    
    return data
    
if __name__=='__main__':
#     try:
    mydht=dht.DHT11(machine.Pin(4))
    ConnectWifi(SSID,PASSWORD)
    client = MQTTClient(CLIENT_ID, SERVER,0,username,password,60)     #create a mqtt client
    print(client)
    client.set_callback(sub_cb)                         #set callback
    client.connect()                                    #connect mqtt
    client.subscribe(subscribe_TOPIC)                   #client subscribes to a topic
    mytimer=Timer(0)
    mytimer.init(mode=Timer.PERIODIC, period=5000,callback=apptimerevent)
    while True:
        client.wait_msg()                            #wait message
            
#     except Exception  as ex_results:
#         print('exception1',ex_results)
#     finally:
#         if(client is not None):
#             client.disconnect()
#         wlan.disconnect()
#         wlan.active(False)
#  
