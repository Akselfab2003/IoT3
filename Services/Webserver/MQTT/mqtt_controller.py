import os
import time
import paho.mqtt.client as mqtt
import logging

logger = logging.getLogger(__name__)
BROKER = "mosquitto" 
PORT = 1883
TOPIC = "PersonDetected"

logger.info("Connecting to broker: " + BROKER)

def on_PersonDetected(client, userdata, message):
    logger.info("message received ", str(message.payload.decode("utf-8")))
    logger.info("message topic=", message.topic)
    logger.info("message qos=", message.qos)
    logger.info("message retain flag=", message.retain)
    
def on_connect(client, userdata, flags, rc):
    logger.info("Connected with result code "+str(rc))
    client.subscribe(TOPIC)
    

def on_subscribe(client, userdata, mid, granted_qos):
    logger.info("Subscribed: "+str(mid)+" "+str(granted_qos))



def Setup():
    logger.info("Connecting to broker: " + BROKER)
    Client = mqtt.Client()
    Client.on_message = on_PersonDetected
    Client.on_connect = on_connect
    Client.on_subscribe = on_subscribe
    Client.connect(BROKER, PORT, 60)
    Client.subscribe(TOPIC)
    Client.loop_start()