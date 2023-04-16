import datetime as dt
import time
import json
import paho.mqtt.client as mqtt
from random import gauss

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def on_disconnect(client, userdata, rc):
    print("Disconnected with result code "+str(rc))


client = mqtt.Client('stefano_ambient')
client.on_connect = on_connect
client.on_disconnect = on_disconnect


client.connect("test.mosquitto.org", 1883, 60)

client.loop_start()

while True:
    data = {}
    data['temp'] = gauss(20,2)
    data['relhum'] = gauss(50,1)
    data['press'] = gauss(950,5)
    client.publish('lezioni_iot/ambient/stefano',json.dumps(data))
    time.sleep(10)

client.disconnect()
client.loop_stop()
