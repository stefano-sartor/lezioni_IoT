import datetime as dt
import time
import json
import paho.mqtt.client as mqtt
from paho.mqtt.properties import Properties
from paho.mqtt.packettypes import PacketTypes
from random import gauss

TOPIC_BASE = "lezioni_iot/ambient/"
TOPIC_SENSOR = TOPIC_BASE+'stefano'
TOPIC_STATUS = TOPIC_SENSOR+'_status'
TOPIC_SET_TEMP = TOPIC_BASE+'stefano_set_temp'

temp = 20

def on_connect(client, userdata, flags, reasonCode, properties):
    print("Connected with result code "+str(reasonCode))
    client.subscribe(TOPIC_SET_TEMP, qos=2)

def on_disconnect(client, userdata, reasonCode, properties):
    print("Disconnected with result code "+str(reasonCode))

def on_message(client, userdata, msg):
    global temp
    response_topic = msg.properties.ResponseTopic
    data = json.loads(msg.payload)
    new_temp = data.get('set_temp',temp)
    temp = new_temp

    properties=Properties(PacketTypes.PUBLISH)
    properties.CorrelationData=msg.properties.CorrelationData
    properties.ContentType='application/json'

    reply_data = {}
    reply_data['result'] = 'ok'
    reply_data['set_temp'] = temp

    print(f'set temp to {temp}, reply to {response_topic}, corr_data {properties.CorrelationData}')
    client.publish(response_topic,json.dumps(reply_data),properties=properties,qos=1)


client = mqtt.Client('stefano_ambient_actuator',protocol=mqtt.MQTTv5)
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_message = on_message


client.will_set(TOPIC_STATUS,'offline',qos=1,retain=True)
client.connect("test.mosquitto.org", 1883, 60)

client.loop_start()

client.publish(TOPIC_STATUS,'online',qos=2,retain=True)

while True:
    data = {}
    data['timestamp'] = dt.datetime.now().isoformat()
    data['temp'] = gauss(temp,2)
    data['relhum'] = gauss(50,1)
    data['press'] = gauss(950,5)
    client.publish(TOPIC_BASE+'stefano',json.dumps(data),qos=1)
    time.sleep(10)

client.disconnect()
client.loop_stop()
