from .base import Base
from sqlalchemy import Column, Integer, String
from .db import get_session
from argon2 import PasswordHasher

class Login(Base):
    __tablename__ = 'logins'
    
    id = Column(Integer, primary_key=True, autoincrement=True)
    username = Column(String(255), nullable=False)
    keycard = Column(String(255), nullable=False)
    

    def __init__(self, username, keycard):
        self.username = username
        self.keycard = self.hash_keycard(keycard)
    
    @staticmethod
    def hash_keycard(keycard):
        ph = PasswordHasher()
        return ph.hash(keycard)
    
    @staticmethod
    def verify_keycard(hashed_keycard, keycard):
        ph = PasswordHasher()
        try:
            return ph.verify(hashed_keycard, keycard)
        except Exception as e:
            print(f"Error verifying keycard: {e}")
            return False

def add_new_login(login:Login):
    with get_session() as session:
        session.add(login)


def login(login:Login):
    with get_session() as session:
        try:
            logins = session.query(Login).filter(Login.username == login.username).first()
            if logins is None:
                return False
            if Login.verify_keycard(logins.keycard, login.keycard):
                return True
            
            return False
        except:
            return False
        