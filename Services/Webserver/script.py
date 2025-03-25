import os
import paho.mqtt.client as mqtt
import time

print(f"Connecting to broker: {os.getenv('BROKER', 'Not Set')}")
BROKER = os.getenv("BROKER", "localhost")  # Default to localhost if BROKER is not set
PORT = 1883
TOPIC = "test/topic"

# Callback when a message is received
def on_message(client, userdata, message):
    print(f"Received: {message.payload.decode()} on topic {message.topic}")

# Callback when a message is published
def on_publish(client, userdata, mid):
    print(f"Message published with mid: {mid}")

# Setup MQTT client
client = mqtt.Client()
client.on_message = on_message
client.on_publish = on_publish  # Add publish callback

# Connect to the broker
client.connect(BROKER, PORT, 60)
client.subscribe(TOPIC)

# Start the loop in a separate thread
client.loop_start()

# Publish messages every 2 seconds
try:
    while True:
        message = "Hello from MQTT!"
        print(f"Publishing: {message}")
        result = client.publish(TOPIC, message)
        if result.rc != 0:
            print(f"Failed to publish message. Result code: {result.rc}")
        time.sleep(2)
except KeyboardInterrupt:
    print("\nStopping...")
    client.loop_stop()
    client.disconnect()