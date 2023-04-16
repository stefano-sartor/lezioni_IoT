import paho.mqtt.client as mqtt
import json

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe('lezioni_iot/ambient/stefano')

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    data = json.loads(msg.payload)
    print(f"{msg.topic} temp {data.get('temp'):.2f}°C,  rh {data.get('relhum'):.2f}%  pres {data.get('press'):.2f} hPa")

def on_disconnect(client, userdata, rc):
    print("Disconnected with result code "+str(rc))


client = mqtt.Client('pippo_sub')
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

client.connect("test.mosquitto.org", 1883, 60)

client.loop_forever()