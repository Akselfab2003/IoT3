from .base import Base
from sqlalchemy import Column, Integer, String
from db import get_session

class logins(Base):
    __tablename__ = 'logins'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    username = Column(String(255), nullable=False)
    keycard = Column(String(255), nullable=False)
    

    def __init__(self, username, keycard):
        self.username = username
        self.keycard = keycard

def add_new_login(login:logins):
    with get_session() as session:
        session.add(login)


def login(login:logins):
    with get_session() as session:
        logins = session.query(logins).filter(logins.username == login.username).filter(logins.keycard == login.keycard).all()
        return len(logins) > 0
    