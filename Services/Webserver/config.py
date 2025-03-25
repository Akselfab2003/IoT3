import os
class Config:
    def __init__(self):
        self.DB_PATH = os.getenv("DB_PATH")


print(Config().DB_PATH)