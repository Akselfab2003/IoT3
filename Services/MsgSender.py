from kafka import KafkaConsumer, KafkaProducer, KafkaAdminClient
from kafka.admin import NewTopic
from kafka.errors import KafkaError, TopicAlreadyExistsError

KAFKA_Broker = "broker:19092"
KAFKA_Topic = "test-topic"


# # Create topic
# try:
#     admin_client = KafkaAdminClient(bootstrap_servers=KAFKA_Broker)
#     topic_list = [NewTopic(name=KAFKA_Topic, num_partitions=1, replication_factor=1)]
#     admin_client.create_topics(new_topics=topic_list, validate_only=False)
#     print(f"Topic '{KAFKA_Topic}' created")
# except TopicAlreadyExistsError:
#     print(f"Topic '{KAFKA_Topic}' already exists")
# except KafkaError as e:
#     print(f"Failed to create topic: {e}")
#     exit(1)



# Producer instance
try:
    producer = KafkaProducer(
        bootstrap_servers=KAFKA_Broker,
        value_serializer=lambda v: v.encode('utf-8')
    )
except KafkaError as e:
    print(f"Failed to create producer: {e}")
    exit(1)

# Send messages
try:
    producer.send(KAFKA_Topic, "Hello World")
    producer.send(KAFKA_Topic, "Hello World2")
    producer.flush()
    print("Messages sent")
except KafkaError as e:
    print(f"Failed to send messages: {e}")

""" 

consumer = KafkaConsumer(KAFKA_Topic, bootstrap_servers=KAFKA_Broker)
for msg in consumer:
    print(msg.value)


def testSub(msg):
    print(msg)


consumer.subscribe(KAFKA_Topic,listener=testSub) """
