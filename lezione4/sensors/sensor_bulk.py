import datetime as dt
import time
import json
import paho.mqtt.client as mqtt
from paho.mqtt.properties import Properties
from paho.mqtt.packettypes import PacketTypes
from random import gauss

TOPIC_BASE = "lezioni_iot/ambient/"


def on_connect(client, userdata, flags, reasonCode, properties):
    print("Connected with result code "+str(reasonCode))

def on_disconnect(client, userdata, reasonCode, properties):
    print("Disconnected with result code "+str(reasonCode))

client = mqtt.Client('stefano_ambient_bulk',protocol=mqtt.MQTTv5)
client.on_connect = on_connect
client.on_disconnect = on_disconnect


client.connect("localhost", 1883, 60)
client.loop_start()

sensors = [
    (10,30,900,'amb_001'),
    (12,35,910,'amb_002'),
    (14,40,920,'amb_003'),
    (18,50,930,'amb_004'),
    (20,48,950,'amb_005'),
]


properties=Properties(PacketTypes.PUBLISH)
properties.ContentType='application/json'

while True:
    for t,h,p,name in sensors:
        data = {}
        data['time'] = dt.datetime.utcnow().isoformat()+'+00'
        data['temp'] = gauss(t,2)
        data['relhum'] = gauss(h,1)
        data['press'] = gauss(p,5)
        info = client.publish(TOPIC_BASE+name,json.dumps(data),properties=properties,qos=1)
        info.wait_for_publish()
        print(info)
        time.sleep(1)
    time.sleep(5)
