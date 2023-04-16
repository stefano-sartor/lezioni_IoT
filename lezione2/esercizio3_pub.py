import datetime as dt
import time
import json
import paho.mqtt.client as mqtt
from random import gauss

TOPIC_BASE = "lezioni_iot/ambient/"
TOPIC_SENSOR = TOPIC_BASE+'stefano'
TOPIC_STATUS = TOPIC_SENSOR+'_status'

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

def on_disconnect(client, userdata, rc):
    print("Disconnected with result code "+str(rc))


client = mqtt.Client('stefano_ambient')
client.on_connect = on_connect
client.on_disconnect = on_disconnect


client.will_set(TOPIC_STATUS,'offline',qos=2,retain=True)
client.connect("test.mosquitto.org", 1883, 60)

client.loop_start()

client.publish(TOPIC_STATUS,'online',qos=2,retain=True)

while True:
    data = {}
    data['timestamp'] = dt.datetime.now().isoformat()
    data['temp'] = gauss(20,2)
    data['relhum'] = gauss(50,1)
    data['press'] = gauss(950,5)
    client.publish(TOPIC_BASE+'stefano',json.dumps(data),qos=1)
    time.sleep(2)

client.disconnect()
client.loop_stop()
