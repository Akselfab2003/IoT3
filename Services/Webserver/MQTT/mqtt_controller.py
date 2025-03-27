import enum
import paho.mqtt.client as mqtt
import logging

class Topic(enum.Enum):
    PersonDetected = "PersonDetected"
    RegisterSensor = "RegisterSensor"
    SensorTriggered = "SensorTriggered"
    KeyCardDetected = "KeyCardDetected"

logger = logging.getLogger(__name__)
BROKER = "mosquitto" 
PORT = 1883

logger.info("Connecting to broker: " + BROKER)


def on_message(client, userdata, message):
    logger.info(f"Topic: {message.topic.decode('utf-8')} Message: {str(message.payload.decode('utf-8'))}")
    
    topic = message.topic.decode('utf-8')
    
    
    if topic == Topic.PersonDetected.value:
        logger.info("Person Detected")
    elif topic == Topic.RegisterSensor.value:
        logger.info("Register Sensor")
    elif topic == Topic.SensorTriggered.value:
        logger.info("Sensor Triggered")
    elif topic == Topic.KeyCardDetected.value:
        logger.info("KeyCard Detected")

    
def on_PersonDetected(client, userdata, message):
    logger.info(f"message received:{ str(message.payload.decode('utf-8'))}")
    
def RegisterSensor(client, userdata, message):
    logger.info(f"message received:{ str(message.payload.decode('utf-8'))}")
    
    
def on_subscribe(client, userdata, mid, granted_qos):
    logger.info("Subscribed: "+str(mid)+" "+str(granted_qos))



def Setup():
    logger.info("Connecting to broker: " + BROKER)
    Client = mqtt.Client()
    Client.on_message = on_message

    Client.connect(BROKER, PORT, 60)
    Client.subscribe([(e.value, 0) for e in Topic])
    Client.loop_start()