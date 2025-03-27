from Models.base import Base
from Models.db import DB_ENGINE

Base.metadata.create_all(DB_ENGINE)
