import paho.mqtt.client as mqtt
import json
import time

TOPIC_BASE = "lezioni_iot/ambient/"
TOPIC_SENSOR = TOPIC_BASE+'stefano'
TOPIC_STATUS = TOPIC_SENSOR+'_status'

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(TOPIC_SENSOR, qos=1)
    client.subscribe(TOPIC_STATUS, qos=1)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    if msg.topic.endswith('_status'):
        print(f"{msg.topic} {msg.payload.decode('utf-8')}")
        return

    data = json.loads(msg.payload)
    print(f"{data.get('timestamp')} {msg.topic} temp {data.get('temp',-32768):.2f}°C,  rh {data.get('relhum',-32768):.2f}%  pres {data.get('press',-32768):.2f} hPa")

def on_disconnect(client, userdata, rc):
    print("Disconnected with result code "+str(rc))


client = mqtt.Client('pippo_sub',clean_session=False)
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

client.connect("test.mosquitto.org", 1883, 60)
client.loop_forever()
