import network
import time
from simple import MQTTClient
import machine
import dht
import wifi_credentials
import onewire, ds18x20





led = machine.Pin(3,machine.Pin.OUT)
d = dht.DHT11(machine.Pin(1))

pot = machine.Pin(0)

ds = ds18x20.DS18X20(onewire.OneWire(pot))
roms = ds.scan()



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
    
#client = MQTTClient(CLIENT_ID, SERVER,PORT,USER,PASSWORD)
client = MQTTClient("ESP32-Yang", "172.20.10.3")
client.connect()


# **************************************#
# Main loop:
while True:
    
    #if time.ticks_ms() - last_update >= UPDATE_TIME_INTERVAL:
    d.measure()
    t = d.temperature()
    h = d.humidity()
    
    ds.convert_temp()
    for rom in roms:
        temperature = ds.read_temp(rom)

    #payload = "field1=" + str(t) + "&field2=" + str(h)
    payload = "Temp={}&Humi={},Temp2={}" .format(str(t), str(h),str(round(temperature,1)))

    
    client.publish(b'temp1', str(t))
    client.publish(b'Humi', str(h))
    client.publish(b'temp2', str(round(temperature,1)))
    #client.disconnect()

    print(payload)
    led.value(not led.value())
    time.sleep_ms(300)