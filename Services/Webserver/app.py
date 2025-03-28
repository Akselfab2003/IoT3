from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from starlette.requests import Request
from MQTT.mqtt_controller import Setup
import logging

from Models.PeopleCounter import PeopleCounter,read_people_counter_from_db,add_new_people_counter_to_db
from Models.base import Base
from Models.db import DB_ENGINE

Base.metadata.create_all(bind=DB_ENGINE)

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)
Setup()

app = FastAPI()


# Mount static folder
app.mount("/static", StaticFiles(directory="static"), name="static")

# Set up templates
templates = Jinja2Templates(directory="templates")

@app.get("/")
def index(request: Request):
    val :list[PeopleCounter]= read_people_counter_from_db()
    return templates.TemplateResponse("index.html",{"request":request,"test": "Hello, World!","PeopleCount":val[-1].people})

@app.get("/viewpage")
def view_page(request: Request):
    val :list[PeopleCounter]= read_people_counter_from_db()
    return templates.TemplateResponse("viewPage.html", {"request": request, "people_count": val[-1].people})


@app.get("/test")
def add_people():
    import random
    from datetime import datetime,timezone
    people = random.randint(1,10)
    
    test = PeopleCounter(people=people,timestamp=datetime.now(timezone.utc))
    add_new_people_counter_to_db(test)
    
    val: list[PeopleCounter] = read_people_counter_from_db()
    
    for i in val:
        logger.info(i.people)
        
    
    return {"PeopleCount":val[-1].people}
    

@app.get("/api/people_count")
def get_people_count():
    val: list[PeopleCounter] = read_people_counter_from_db()
    if not val:
        return {"timestamps": [], "counts": []}
    
    timestamps = [entry.timestamp.isoformat() for entry in val]
    counts = [entry.people for entry in val]
    return {"timestamps": timestamps, "counts": counts}
