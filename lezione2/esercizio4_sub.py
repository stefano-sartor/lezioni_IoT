import paho.mqtt.client as mqtt
from paho.mqtt.properties import Properties
from paho.mqtt.packettypes import PacketTypes
import json
import random
import time
import string

TOPIC_BASE = "lezioni_iot/ambient/"
TOPIC_SENSOR = TOPIC_BASE+'stefano'
TOPIC_STATUS = TOPIC_SENSOR+'_status'
TOPIC_SET_TEMP = TOPIC_BASE+'stefano_set_temp'

TOPIC_REPLY = 'reply/'+TOPIC_SET_TEMP

req_no = 'a'

def on_connect(client, userdata, flags, reasonCode, properties):
    print("Connected with result code "+str(reasonCode))

    client.subscribe(TOPIC_SENSOR, qos=1)
    client.subscribe(TOPIC_STATUS, qos=1)
    client.subscribe(TOPIC_REPLY, qos=1)

def on_message(client, userdata, msg):
    global req_no
    if msg.topic == TOPIC_REPLY:
        corr_data = msg.properties.CorrelationData.decode('ascii')
        data = json.loads(msg.payload)
        if corr_data == req_no:
            print(f'correlation data ok, result: {data}')
        else:
            print(f'correlation data mismatch, result: {data}')
        return

    if msg.topic.endswith('_status'):
        print(f"{msg.topic} {msg.payload.decode('utf-8')}")
        return

    data = json.loads(msg.payload)
    print(f"{data.get('timestamp')} {msg.topic} temp {data.get('temp',-32768):.2f}°C,  rh {data.get('relhum',-32768):.2f}%  pres {data.get('press',-32768):.2f} hPa")

def on_disconnect(client, userdata, reasonCode, properties):
    print("Disconnected with result code "+str(reasonCode))

def set_temperature(client):
    global req_no
    req_no = random.choice(string.ascii_letters)
    temp = random.randint(15,30)

    print(f'request temp {temp} with correlation data {req_no}')
    properties=Properties(PacketTypes.PUBLISH)
    properties.ResponseTopic=TOPIC_REPLY
    properties.CorrelationData=req_no.encode('ascii')
    properties.ContentType='application/json'

    client.publish(TOPIC_SET_TEMP,json.dumps({'set_temp':temp}),properties=properties,qos=1)

client = mqtt.Client('stefano_ambient_control',protocol=mqtt.MQTTv5)
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

client.connect("test.mosquitto.org", 1883, 60)

client.loop_start()

client.publish(TOPIC_STATUS,'online',qos=2,retain=True)

while True:
    time.sleep(5)
    set_temperature(client)
