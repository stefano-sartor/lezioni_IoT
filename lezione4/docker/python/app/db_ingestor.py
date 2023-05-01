import paho.mqtt.client as mqtt
from paho.mqtt.properties import Properties
from paho.mqtt.packettypes import PacketTypes
import json
import random
import time
import string
from sqlalchemy import insert
from sensor_tables import engine,metadata_obj,ambient_sensor
import datetime as dt

TOPIC_BASE = "lezioni_iot/ambient/"


def on_connect(client, userdata, flags, reasonCode, properties):
    print("Connected with result code "+str(reasonCode))
    client.subscribe(TOPIC_BASE+'#', qos=1)


def on_message(client, userdata, msg):
    try:
        sensor_name = msg.topic.split('/')[-1]

        data = json.loads(msg.payload)
        data['sensor_name'] = sensor_name

        with engine.connect() as conn:
            conn.execute(insert(ambient_sensor), data)
            conn.commit()
        print(data)
    except Exception as e:
        print(f'on_message Error: {e}')

def on_disconnect(client, userdata, reasonCode, properties):
    print("Disconnected with result code "+str(reasonCode))



client = mqtt.Client('stefano_ambient_postgres',protocol=mqtt.MQTTv5)
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

client.connect("mosquitto", 1883, 60)

client.loop_forever()
