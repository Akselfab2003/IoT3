from .base import Base
from sqlalchemy import Column, Integer, String, Float
from sqlalchemy.orm import relationship
from db import DB_ENGINE,get_session

class Sensor(Base):
    __tablename__ = 'sensors'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    name = Column(String(255), nullable=False)
    type = Column(String(255), nullable=False)
    description = Column(String(255), nullable=False)
    
    logs = relationship("SensorsLog", back_populates="sensor")
    
    
    def __init__(self, name, type, description):
        self.name = name
        self.type = type
        self.description = description

def add_new_sensor(Sensor:Sensor):
    with get_session() as session:
        session.add(Sensor)
    
def read_sensors_definition():
    with get_session() as session:
        Sensors = session.query(Sensor).all()
        return Sensors
   

def register_sensor(sensor:Sensor):
    DoesSensorExist = check_if_sensor_definition_exists(sensor.name)
    if DoesSensorExist == False:
        add_new_sensor(sensor)
    

def check_if_sensor_definition_exists(sensor_name:str):
    with get_session() as session:
        Sensors = session.query(Sensor).filter(Sensor.name == sensor_name).all()
        return len(Sensors) > 0
    
