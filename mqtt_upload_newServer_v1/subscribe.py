import network
import time
from simple import MQTTClient
import machine
import dht
import wifi_credentials
from machine import Pin, I2C
from ssd1306 import SSD1306_I2C
#SERVER = "mqtt-v2.thingspeak.com" # Enter Mqtt Broker Name
#SERVER = "mqtt3.thingspeak.com" # Enter Mqtt Broker Name

# PORT = 1883
# CHANNEL_ID = "1810933" #Enter Channel Id here
# USER = "LA8UJDAWOjcpFBEKADIpHzQ"  # Enter User Id here
# CLIENT_ID = "LA8UJDAWOjcpFBEKADIpHzQ" #Enter Client Id here
# PASSWORD = "12DnGMY/CxSL//Gn3MZdwrGZ" #Enter Password here



#create topic to publish the message
#topicOut = "channels/" + CHANNEL_ID + "/publish" 

#create topic to publish the message
#topicIn = "channels/" + CHANNEL_ID  + "/subscribe/fields/field2"


# Function to implement wifi connection
sta = network.WLAN(network.STA_IF)
if not sta.isconnected():
  print('connecting to network...')
  sta.active(True)
  #sta.connect('wifi ssid', 'wifi password')
  sta.connect(wifi_credentials.ssid, wifi_credentials.password)
  while not sta.isconnected():
    pass
print('network config:', sta.ifconfig())


def call_back_function(topic, msg):
    global temperature
    temperature = float(msg.decode())
#     oled.fill(0)  
#     oled.text(msg.decode().strip("'\n"), 5, 5)  
#     oled.show()  
    print((topic, msg))
    
    
def plot_time(yp, t, x, y, var = [27,36], vpts=[25, 16, 40], hpts = [25, 55, 112]):
    """"
    Graph function of the Cartesian plane in relation to time:
    by: Sergio Andres Castaño Giraldo
    
    plot_time(yp, t, x, y, var = [30,60], vpts=[25, 16, 40], hpts = [25, 55, 112]):

    yp: dependent variable
    t: time (used while the Cartesian plane is not complete)
    x: List of two positions of variable x, x[0] is the position in past x and x[1] position of current x.
    y: List of two positions of the variable y, y[0] is the position in y past and y[1] position of current x.
    var = [0.0,3.3]: Magnitude of the variable (default voltage)
    vpts = [25, 16, 40]: points on the vertical y axis.
    hpts = [25, 55, 112]: points on the vertical x axis.

    Example:
    #Global Variables
    t = 0
    y = [55, 55]
    x = [25, 25]
    #Function:
    volts = pot.read_u16 () * FACTOR
    t, x, y = plot_time (volts, t, x, y)
    sleep_ms (500)
    """
    #Axis
    oled.vline(vpts[0], vpts[1], vpts[2], 1) #x, y, h
    oled.hline(hpts[0], hpts[1], hpts[2], 1) #x, y, w
    oled.text(str(round(var[0],1)), vpts[0]-25, hpts[1]-5)
    oled.text(str(round(var[1],1)), vpts[0]-25, vpts[1])
    #y - axis
    y[1] = int((yp-var[0])/(var[1]-var[0]) * (vpts[1]-hpts[1]) + hpts[1]) #Interpolation
    if t < hpts[2] - hpts[0]:
        x[1] = x[0]+1
    else:
        x[1] = hpts[2]
    
    #Plot the line
    oled.line(x[0],y[0],x[1],y[1],1)
    oled.show()
    
    #Update past values
    y[0] = y[1]
    x[0] = x[1]
    
    #If you have already reached the end of the graph then ...
    if t > hpts[2] - hpts[0]:
        #Erases the first few pixels of the graph and the y-axis.
        oled.fill_rect(vpts[0],vpts[1],2,vpts[2],0)
        #Clears the entire y-axis scale
        oled.fill_rect(vpts[0]-25, vpts[1],vpts[0],vpts[2]+5,0)
        #shifts the graph one pixel to the left
        oled.scroll(-1,0)
        #Axis
        oled.vline(vpts[0], vpts[1], vpts[2], 1) #x, y, h
        oled.hline(hpts[0], hpts[1], hpts[2], 1) #x, y, w
        oled.text(str(round(var[0],1)), vpts[0]-25, hpts[1]-5)
        oled.text(str(round(var[1],1)), vpts[0]-25, vpts[1])
    else:
        t += 1
    
    return t,x,y

led = machine.Pin(3,machine.Pin.OUT)
temperature = 0
i2c = I2C(0, scl = Pin(1), sda = Pin(2), freq = 400000)
WIDTH = 128
HEIGHT = 64
oled = SSD1306_I2C(WIDTH, HEIGHT, i2c)

t = 0
y = [55, 55]
x = [25, 25]
    
#client = MQTTClient(CLIENT_ID, SERVER,PORT,USER,PASSWORD)
client = MQTTClient("ESP32-Luo", "172.20.10.3")

client.set_callback(call_back_function)  
client.connect()
#UPDATE_TIME_INTERVAL = 500 # in ms unit
#last_update = time.ticks_ms()
client.subscribe(b'temp2')
# **************************************#
# Main loop:
while True:  
    try:  
        #client.wait_msg() #blocking  
        client.check_msg()
        
        t,x,y = plot_time(temperature,t,x,y)
        
        oled.fill_rect(0,0,120,15,0)
        oled.text("Temp: ", 0, 0)
        oled.text(str(round(temperature,1)), 52, 0)
        oled.show()
        led.value(not led.value())
        time.sleep_ms(300)
        #sleep_ms(100)
        #non-blocking  
    except KeyboardInterrupt:  
        print('Ctrl-C pressed...exiting')  
        client.disconnect()  
        sys.exit()
    
    
    #if time.ticks_ms() - last_update >= UPDATE_TIME_INTERVAL:

    
