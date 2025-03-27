from base import Base
from sqlalchemy import Column, Integer, String, Float
from sqlalchemy.orm import relationship
from dataclasses import dataclass
from db import DB_ENGINE,get_session

@dataclass
class Sensor(Base):
    __tablename__ = 'sensors'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    name = Column(String(255), nullable=False)
    type = Column(String(255), nullable=False)
    description = Column(String(255), nullable=False)
    
    logs = relationship("SensorsLog", back_populates="sensor")
    
    
    # def __init__(self, name, type, description):
    #     self.name = name
    #     self.type = type
    #     self.description = description

def add_new_sensor(Sensor:Sensor):
    Base.metadata.create_all(DB_ENGINE)
    session = get_session()
    session.add(Sensor)
    session.commit()
    session.close()
    
def read_sensors_definition():
    Base.metadata.create_all(DB_ENGINE)
    session = get_session()
    Sensors = session.query(Sensor).all()
    session.close()
    return Sensors

def register_sensor(sensor:Sensor):
    DoesSensorExist = check_if_sensor_definition_exists(sensor.name)
    if DoesSensorExist == False:
        add_new_sensor(sensor)
    

def check_if_sensor_definition_exists(sensor_name:str):
    Base.metadata.create_all(DB_ENGINE)
    session = get_session()
    Sensors = session.query(Sensor).filter(Sensor.name == sensor_name).all()
    session.close()
    
    return len(Sensors) > 0
    
    
    
if __name__ == "__main__":
    Base.metadata.create_all(DB_ENGINE)
    test = Sensor(name="test2",type="test2",description="test2")
    register_sensor(test)
    for i in read_sensors_definition():
        print(i.id)
        print(i.name)
        print(i.type)
        print(i.description)
    

