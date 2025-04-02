import enum
import json
import paho.mqtt.client as mqtt
import logging
from Models.PeopleCounter import PeopleCounter, add_new_people_counter_to_db
from Models.Sensor import Sensor, register_sensor, get_sensor_id_by_name
from Models.SensorsLog import SensorsLog, add_new_sensor_log
from datetime import datetime

# Enum class to define MQTT topics
class Topic(enum.Enum):
    PersonDetected = "PersonDetected"  # Topic for detecting people
    RegisterSensor = "RegisterSensor"  # Topic for registering sensors
    SensorTriggered = "SensorTriggered"  # Topic for triggered sensors
    KeyCardDetected = "KeyCardDetected"  # Topic for detecting keycards

# Configure logger
logger = logging.getLogger(__name__)
BROKER = "mosquitto"
PORT = 1883

logger.info("Connecting to broker: " + BROKER)

# Callback function to handle incoming MQTT messages
def on_message(client, userdata, message):
    logger.info(f"Topic: {message.topic} Message: {str(message.payload.decode('utf-8'))}")
    
    payload = str(message.payload.decode('utf-8'))  # Decode the message payload
    topic = message.topic  # Get the topic of the message
    
    # Route the message to the appropriate handler based on the topic
    if topic == Topic.PersonDetected.value:
        PersonDetected(payload)
    elif topic == Topic.RegisterSensor.value:
        RegisterSensor(payload)
    elif topic == Topic.SensorTriggered.value:
        SensorTriggered(payload)
    elif topic == Topic.KeyCardDetected.value:
        logger.info("KeyCard Detected")

# Handler for the "PersonDetected" topic
def PersonDetected(payload: str):
    peopleCountUpdate = json.loads(payload)  # Parse the JSON payload
    logger.info("Person Detected")

    # Convert the timestamp from the payload to a datetime object
    timestamp = peopleCountUpdate["timestamp"]
    dateTimestamp = datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S")

    # Create a PeopleCounter object from the payload data
    peopleCounter: PeopleCounter = PeopleCounter(
        people=peopleCountUpdate["people"],
        timestamp=dateTimestamp
    )
    
    logger.info(f"Timestamp: {peopleCounter.timestamp} Updating people count to: {peopleCounter.people}")
    add_new_people_counter_to_db(peopleCounter)  # Save the data to the database

# Handler for the "RegisterSensor" topic
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
    register_sensor(sensor)  # Save the sensor to the database

# Handler for the "SensorTriggered" topic
def SensorTriggered(payload: str):
    sensorLogUpdate = json.loads(payload)  # Parse the JSON payload
    logger.info("Sensor Triggered")
    
    # Extract the sensor name and log details from the payload
    sensorName = sensorLogUpdate["SensorName"]
    logger.info(f"Sensor: {sensorName} triggered")
    sensor_id = get_sensor_id_by_name(sensorName)  # Get the sensor ID by name

    # Convert the timestamp from the payload to a datetime object
    timestamp = sensorLogUpdate["SensorLogObject"]["timestamp"]
    dateTimestamp = datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S")

    # Create a SensorsLog object from the payload data
    sensorLog: SensorsLog = SensorsLog(
        sensor_id=sensor_id,
        value=sensorLogUpdate["SensorLogObject"]["value"],
        timestamp=dateTimestamp
    )

    add_new_sensor_log(sensorLog)  # Save the sensor log to the database

import asyncio

# Asynchronous function to wait for a keycard detection
async def WaitforKeyCard():
    future = asyncio.Future()  # Create a future to wait for the keycard detection
    
    # Callback function to handle keycard detection
    def KeyCardDetected(client, userdata, message):
        if message.topic == Topic.KeyCardDetected.value:
            payload = str(message.payload.decode('utf-8'))  # Decode the message payload
            if not future.done():
                future.set_result(payload)  # Set the result of the future with the payload
                
            logger.info("KeyCard Detected")
            logger.info(f"KeyCardDetected: {payload}")
    
    # Create an MQTT client for keycard detection
    KeyCardClient = mqtt.Client()
    KeyCardClient.connect(BROKER, PORT, 60)  # Connect to the MQTT broker
    KeyCardClient.subscribe(Topic.KeyCardDetected.value)  # Subscribe to the "KeyCardDetected" topic
    KeyCardClient.on_message = KeyCardDetected  # Set the callback function for messages
    KeyCardClient.loop_start()  # Start the MQTT client loop
    
    try:
        # Wait for the keycard detection with a timeout of 30 seconds
        return await asyncio.wait_for(future, 30)
    except asyncio.TimeoutError:
        raise TimeoutError("Timeout waiting for KeyCard")
    finally:
        KeyCardClient.loop_stop()  # Stop the MQTT client loop
        KeyCardClient.disconnect()  # Disconnect the MQTT client

# Function to set up the MQTT client and subscribe to topics
def Setup():
    logger.info("Connecting to broker: " + BROKER)
    Client = mqtt.Client() 
    Client.on_message = on_message
    Client.connect(BROKER, PORT, 60)
    Client.subscribe([(e.value, 0) for e in Topic])
    Client.loop_start()