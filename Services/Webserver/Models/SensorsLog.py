from sqlalchemy import Column, Integer, DateTime, Float, ForeignKey
from db import DB_ENGINE,get_session
from base import Base
from sqlalchemy.orm import relationship

class SensorsLog(Base):
    __tablename__ = 'sensors_log'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    sensor_id = Column(Integer, ForeignKey('sensors.id') ,nullable=False)
    value = Column(Float, nullable=False)
    timestamp = Column(DateTime, nullable=False)
    
    sensor = relationship("Sensor", back_populates="logs")
        
    def __init__(self, sensor_id, value, timestamp:DateTime):
        self.sensor_id = sensor_id
        self.value = value
        self.timestamp = timestamp

def add_new_sensor_log(Sensor_log_entry:SensorsLog):
    with  get_session() as session:
        session.add(Sensor_log_entry)
      
    
def read_sensor_log():
    with get_session() as session:
        Sensor_log = session.query(SensorsLog).all()
        return Sensor_log

def read_sensor_log_by_sensor_id(sensor_id:int):
    with get_session() as session:
        Sensor_log = session.query(SensorsLog).filter(SensorsLog.sensor_id == sensor_id).all()
        return Sensor_log
