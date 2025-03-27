from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker,scoped_session
from config import Config
import os
from contextlib import contextmanager

def Create_Database_Path_If_Needed(PATH=None):
    db_file_path = PATH.replace("sqlite:///", "")  
    db_dir = os.path.dirname(db_file_path)

    if not os.path.exists(db_dir):
        os.makedirs(db_dir)  # Create the directory if it does not exist

DB_PATH = Config().DB_PATH

Create_Database_Path_If_Needed(DB_PATH)

DB_ENGINE = create_engine(DB_PATH)

SESSION_maker = sessionmaker(bind=DB_ENGINE,autoflush=True)

Session = scoped_session(SESSION_maker)

@contextmanager
def get_session():
    session = Session()
    try:
        yield session
        session.commit()
    except:
        session.rollback()
        raise


