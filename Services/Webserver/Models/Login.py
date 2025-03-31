from .base import Base
from sqlalchemy import Column, Integer, String
from .db import get_session

class Login(Base):
    __tablename__ = 'logins'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    username = Column(String(255), nullable=False)
    keycard = Column(String(255), nullable=False)
    

    def __init__(self, username, keycard):
        self.username = username
        self.keycard = keycard

def add_new_login(login:Login):
    with get_session() as session:
        session.add(login)


def login(login:Login):
    with get_session() as session:
        logins = session.query(Login).filter(Login.username == login.username).filter(Login.keycard == login.keycard).all()
        return len(logins) > 0
    