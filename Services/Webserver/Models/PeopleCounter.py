from datetime import datetime,timezone
from sqlalchemy import Column, Integer, DateTime
from base import Base
from db import DB_ENGINE,SESSION_MAKER

class PeopleCounter(Base):
    __tablename__ = 'peoplecounter'
    
    id = Column(Integer, primary_key=True,autoincrement=True)
    timestamp = Column(DateTime,default=datetime.now(timezone.utc),nullable=False)
    people = Column(Integer,nullable=False)
    
    def __init__(self,people:int,timestamp:datetime):
        self.people = people
        self.timestamp = timestamp


def add_new_people_counter_to_db(peopleCounter:PeopleCounter):
    Base.metadata.create_all(DB_ENGINE)
    session = SESSION_MAKER()
    session.add(peopleCounter)
    session.commit()
    session.close()
    

def read_people_counter_from_db():
    Base.metadata.create_all(DB_ENGINE)
    session = SESSION_MAKER()
    peopleCounter = session.query(PeopleCounter).all()
    session.close()
    return peopleCounter