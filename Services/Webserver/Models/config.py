import os
from dotenv import load_dotenv
load_dotenv()
class Config:
    def __init__(self):
        self.DB_PATH = os.getenv("DB_PATH")

