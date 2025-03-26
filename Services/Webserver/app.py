from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from starlette.requests import Request
from Models.PeopleCounter import ReadPeopleCounterFromDB
from MQTT.mqtt_controller import Setup

app = FastAPI()

# Mount static folder
app.mount("/static", StaticFiles(directory="static"), name="static")

# Set up templates
templates = Jinja2Templates(directory="templates")

@app.get("/")
def index(request: Request):
    return templates.TemplateResponse("index.html",{"request":request,"test": "Hello, World!","PeopleCount":ReadPeopleCounterFromDB()})


Setup()