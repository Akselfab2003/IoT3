import enum
import json
import paho.mqtt.client as mqtt
import logging
from Models.PeopleCounter import PeopleCounter,add_new_people_counter_to_db
from Models.Sensor import Sensor,add_new_sensor,get_sensor_id_by_name
from Models.SensorsLog import SensorsLog,add_new_sensor_log

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
    logger.info(f"Topic: {message.topic} Message: {str(message.payload.decode('utf-8'))}")
    
    payload = str(message.payload.decode('utf-8'))
    topic = message.topic
    
    if topic == Topic.PersonDetected.value:
        PersonDetected(payload)
        
    elif topic == Topic.RegisterSensor.value:
        RegisterSensor(payload)
    elif topic == Topic.SensorTriggered.value:
        SensorTriggered(payload)
    elif topic == Topic.KeyCardDetected.value:
        logger.info("KeyCard Detected")

    
def PersonDetected(payload:str):
    peopleCountUpdate: PeopleCounter = json.loads(payload)
    logger.info("Person Detected")
    #logger.log(json.dumps(peopleCountUpdate, indent=4))

    peopleCountUpdate.id = 0
    logger.info(f"Timestamp: {peopleCountUpdate.timestamp} Updating people count to: {peopleCountUpdate.people}")
    add_new_people_counter_to_db(peopleCountUpdate)
    
    
def RegisterSensor(payload:str):
    sensor: Sensor = json.loads(payload)
    logger.info("Register Sensor")
    #logger.log(json.dumps(sensor, indent=4))
    
    #sensor.id = 0
    logger.info(f"Sensor: {sensor.name} registered")
    add_new_sensor(sensor)
    
def SensorTriggered(payload:str):
    sensorLogUpdate = json.loads(payload)
    logger.info("Sensor Triggered")
    #logger.log(json.dumps(sensorLogUpdate, indent=4))
    
    sensorName = sensorLogUpdate["SensorName"]
    sensor_id = get_sensor_id_by_name(sensorName)
    sensorLog : SensorsLog = sensorLogUpdate["SensorLog"]
    sensorLog.id = sensor_id
    #logger.info(f"Sensor: {sensorLog.sensor_name} triggered")
    add_new_sensor_log(sensorLog)



def Setup():
    logger.info("Connecting to broker: " + BROKER)
    Client = mqtt.Client()
    Client.on_message = on_message

    Client.connect(BROKER, PORT, 60)
    Client.subscribe([(e.value, 0) for e in Topic])
    Client.loop_start()