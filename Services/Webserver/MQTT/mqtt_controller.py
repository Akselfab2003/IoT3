import os
import paho.mqtt.client as mqtt

BROKER = "mosquitto"  # Default to localhost if BROKER is not set
PORT = 1883
TOPIC = "PersonDetected"


def on_PersonDetected(client, userdata, message):
    print("message received ", str(message.payload.decode("utf-8")))
    print("message topic=", message.topic)
    print("message qos=", message.qos)
    print("message retain flag=", message.retain)
    
    




def Setup():
    Client = mqtt.Client()
    Client.connect(BROKER, PORT, 60)
    Client.on_message = on_PersonDetected

    Client.subscribe(TOPIC)

    Client.loop_start()