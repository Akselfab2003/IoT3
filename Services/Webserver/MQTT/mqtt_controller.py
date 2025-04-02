import enum
import json
import paho.mqtt.client as mqtt
import logging
import time
import asyncio
from Models.PeopleCounter import PeopleCounter,add_new_people_counter_to_db
from Models.Sensor import Sensor,register_sensor,get_sensor_id_by_name
from Models.SensorsLog import SensorsLog,add_new_sensor_log
from datetime import datetime
from paho.mqtt.reasoncodes import ReasonCode,ReasonCodes 

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
    peopleCountUpdate = json.loads(payload)
    logger.info("Person Detected")

    timestamp = peopleCountUpdate["timestamp"]
    dateTimestamp = datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S")

    peopleCounter : PeopleCounter = PeopleCounter(
        people=peopleCountUpdate["people"],
        timestamp=dateTimestamp
    )
    
    logger.info(f"Timestamp: {peopleCounter.timestamp} Updating people count to: {peopleCounter.people}")
    add_new_people_counter_to_db(peopleCounter)
    
    
def RegisterSensor(payload: str):
    sensor_data = json.loads(payload)  # Parse the JSON payload into a dictionary
    logger.info("Register Sensor")
    
    # Create a Sensor object from the dictionary
    sensor = Sensor(
        name=sensor_data["name"],
        type=sensor_data["type"],
        description=sensor_data.get("description", "")  # Default to an empty string if 'description' is not provided
    )
    
    logger.info(f"Sensor: {sensor} registered")
    register_sensor(sensor)
    
def SensorTriggered(payload:str):
    sensorLogUpdate = json.loads(payload)
    logger.info("Sensor Triggered")
    
    sensorName = sensorLogUpdate["SensorName"]
    logger.info(f"Sensor: {sensorName} triggered")
    sensor_id = get_sensor_id_by_name(sensorName)

    timestamp = sensorLogUpdate["SensorLogObject"]["timestamp"]
    dateTimestamp = datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S")

    sensorLog : SensorsLog = SensorsLog(
        sensor_id=sensor_id,
        value=sensorLogUpdate["SensorLogObject"]["value"],
        timestamp=dateTimestamp
    )
    add_new_sensor_log(sensorLog)


def on_disconnect(client, userdata, rc):
    logger.info("Disconnected from broker")
    reconnect_delay = 2 # seconds
    reconnect_count = 0
    while True:
        logger.info(f"Attempting to reconnect... {reconnect_count}")
        time.sleep(reconnect_delay)
        try:
            client.reconnect()
            logger.info("Reconnected successfully")
            break
        except Exception as e:
            logger.error(f"Reconnect failed: {e}")
        reconnect_count += 1



async def WaitforKeyCard():
    
    future = asyncio.Future()
    
    def KeyCardDetected(client, userdata, message):
        if message.topic == Topic.KeyCardDetected.value:
            payload = str(message.payload.decode('utf-8'))
            if not future.done():
                future.set_result(payload)
                
            logger.info("KeyCard Detected")
            logger.info(f"KeyCardDetected: {payload}")
    
    KeyCardClient = mqtt.Client()
    KeyCardClient.connect(BROKER, PORT, 60)
    KeyCardClient.subscribe(Topic.KeyCardDetected.value)
    KeyCardClient.on_message = KeyCardDetected
    KeyCardClient.loop_start()
    
    try:
        return await asyncio.wait_for(future,30)
    except asyncio.TimeoutError:
        raise TimeoutError("Timeout waiting for KeyCard")
    finally:
        KeyCardClient.loop_stop()
        KeyCardClient.disconnect()
    
    
    


def Setup():
    logger.info("Connecting to broker: " + BROKER)
    Client = mqtt.Client()
    Client.on_message = on_message
    Client.on_disconnect = on_disconnect

    Client.connect(BROKER, PORT, 60)
    Client.subscribe([(e.value, 0) for e in Topic])
    Client.loop_start()