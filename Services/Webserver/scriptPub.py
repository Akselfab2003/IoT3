import os
import paho.mqtt.client as mqtt

BROKER = os.getenv("BROKER", "localhost")  # Default to localhost if BROKER is not set
PORT = 1883
TOPIC = "test/topic"
# Callback when a message is received
def on_message(client, userdata, message):
    print(f"Received: {message.payload.decode()} on topic {message.topic}")


# Setup MQTT client
client = mqtt.Client()
client.on_message = on_message

# Connect to the broker
client.connect(BROKER, PORT, 60)
client.subscribe(TOPIC)

# Start the loop in a separate thread
client.loop_forever()
