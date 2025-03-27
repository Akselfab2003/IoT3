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
        
    def __init__(self, sensor_id, value, timestamp):
        self.sensor_id = sensor_id
        self.value = value
        self.timestamp = timestamp

def add_new_sensor_log(Sensor_log_entry:SensorsLog):
    Base.metadata.create_all(DB_ENGINE)
    session = get_session()
    session.add(Sensor_log_entry)
    session.commit()
    session.close()
    
def read_sensor_log():
    Base.metadata.create_all(DB_ENGINE)
    session = get_session()
    Sensor_log = session.query(SensorsLog).all()
    session.close()
    return Sensor_log

def read_sensor_log_by_sensor_id(sensor_id:int):
    Base.metadata.create_all(DB_ENGINE)
    session = get_session()
    Sensor_log = session.query(SensorsLog).filter(SensorsLog.sensor_id == sensor_id).all()
    session.close()
    return Sensor_log



if __name__ == "__main__":
    Base.metadata.create_all(DB_ENGINE)
    
    NewLogEntry = SensorsLog(sensor_id=1,value=1.0,timestamp="2020-01-01 00:00:00")
    add_new_sensor_log(NewLogEntry)
    
    for i in read_sensor_log():
        print(i.sensor_id)
        print(i.value)
        print(i.timestamp)
        print(i.sensor.name) 