from datetime import datetime,timezone
from sqlalchemy.orm import declarative_base
from sqlalchemy import Column, Integer, String, DateTime
from config import Config

Base = declarative_base()

DB_PATH = Config().DB_PATH

class PeopleCounter(Base):
    __tablename__ = 'peoplecounter'
    id = Column(Integer, primary_key=True,autoincrement=True)
    timestamp = Column(DateTime,default=datetime.now(timezone.utc),nullable=False)
    people = Column(Integer,nullable=False)




def AddNewPeopleCounterToDB(peopleCounter:PeopleCounter):

    from sqlalchemy import create_engine
    from sqlalchemy.orm import sessionmaker

    engine = create_engine(DB_PATH)
    Base.metadata.create_all(engine)

    Session = sessionmaker(bind=engine)
    session = Session()

    session.add(peopleCounter)
    session.commit()
    session.close()
    

def ReadPeopleCounterFromDB():
    from sqlalchemy import create_engine
    from sqlalchemy.orm import sessionmaker

    engine = create_engine(DB_PATH)
    Base.metadata.create_all(engine)

    Session = sessionmaker(bind=engine)
    session = Session()

    peopleCounter = session.query(PeopleCounter).all()
    session.close()
    return peopleCounter


#test = PeopleCounter(people=5,timestamp=datetime.now(timezone.utc))

#AddNewPeopleCounterToDB(test)


entries = ReadPeopleCounterFromDB()
for entry in entries:
    print(entry.id,entry.timestamp,entry.people)