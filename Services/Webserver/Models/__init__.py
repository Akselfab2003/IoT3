from Sensor import Sensor,register_sensor,read_sensors_definition
from SensorsLog import SensorsLog,add_new_sensor_log,read_sensor_log
from PeopleCounter import PeopleCounter
from base import Base
from db import DB_ENGINE

Base.metadata.create_all(DB_ENGINE)


test = Sensor(name="test2",type="test2",description="test2")
register_sensor(test)


for i in read_sensors_definition():
    print(i.id)
    print(i.name)
    print(i.type)
    print(i.description)
    if i.logs:
        print(i.logs)
