import uuid
from fastapi import FastAPI, Response, WebSocket,Cookie
from fastapi.responses import RedirectResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from starlette.requests import Request
from MQTT.mqtt_controller import Setup,WaitforKeyCard
import logging

from Models.PeopleCounter import PeopleCounter,read_people_counter_from_db,add_new_people_counter_to_db
from Models.base import Base
from Models.db import DB_ENGINE
from Models.Sensor import Sensor,read_sensors_definition,add_new_sensor,get_sensor_id_by_name
from Models.SensorsLog import SensorsLog,read_sensor_log_by_sensor_id,add_new_sensor_log
from Models.Login import Login,add_new_login,login

Base.metadata.create_all(bind=DB_ENGINE)

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)
Setup()

app = FastAPI()


session_store = {
    
}

# Mount static folder
app.mount("/static", StaticFiles(directory="static"), name="static")

# Set up templates
templates = Jinja2Templates(directory="templates")
def create_session():
    session_id = str(uuid.uuid4())
    session_store[session_id] = {
        "KeyCardValidated": False
    }
    return session_id

def validate_session(session_id):
    if session_id not in session_store:
        return False
    return session_store[session_id]["KeyCardValidated"]
    
@app.get("/")
def index(request: Request,session: str = Cookie(default=None)):
    response = templates.TemplateResponse("index.html",{"request":request,"test": "Hello, World!"})

    if session is None or session not in session_store:
        logger.info("No session cookie found")
        session_id = create_session()
        logger.info(f"Created new session with id {session_id}")
        response.set_cookie(key="session", value=session_id, httponly=True, max_age=3600,secure=True)
    
    return response

@app.get("/register")
def register(request: Request, session: str = Cookie(default=None)):
    response = templates.TemplateResponse("register.html", {"request": request})
    
    if session is None or session not in session_store:
        logger.info("No session cookie found or session is invalid")
        # Create a new session and allow the user to proceed
        session_id = create_session()
        logger.info(f"Created new session with id {session_id}")
        response = templates.TemplateResponse("register.html", {"request": request})
        response.set_cookie(key="session", value=session_id, httponly=True, max_age=3600, secure=True)
        return response
    return response
    
@app.get("/viewpage")
def view_page(request: Request,session: str = Cookie(default=None)):
    if session is None:
        return RedirectResponse(url="/")
    if validate_session(session) == False:
        return RedirectResponse(url="/")
    
    val :list[PeopleCounter]= read_people_counter_from_db()
    
    if not val or len(val) == 0:
        logger.info("No people counter data found")
        return templates.TemplateResponse("viewPage.html", {"request": request, "people_count": 0})
    
    return templates.TemplateResponse("viewPage.html", {"request": request, "people_count": val[-1].people})


@app.get("/api/people_counter_statistics")
def get_people_counter_statistics():
    val: list[PeopleCounter] = read_people_counter_from_db()
    
    if not val or len(val) == 0:
        return {"entered": 0, "people": 0, "exited": 0}
    
    val.sort(key=lambda x: x.timestamp)
    
    entered = 0
    exited = 0
    previous_count = val[0].people
    
    for entry in val[1:]:
        if entry.people > previous_count:
            entered += entry.people - previous_count
        elif entry.people < previous_count:
            exited += previous_count - entry.people
        previous_count = entry.people
        
    if len(val) == 1:
        entered = val[0].people
        exited = 0
    return {"entered": entered, "total_inside": val[-1].people, "exited": exited}
        
        
    
    
    


@app.get("/api/people_count")
def get_people_count():
    val: list[PeopleCounter] = read_people_counter_from_db()
    if not val:
        return {"timestamps": [], "counts": []}
    
    timestamps = [entry.timestamp.isoformat() for entry in val]
    counts = [entry.people for entry in val]
    return {"timestamps": timestamps, "counts": counts}


@app.websocket("/registerKeycard")
async def websocket_endpoint(websocket: WebSocket, session: str = Cookie(default=None)):
    if session is None:
        logger.info("No session cookie found")
        await websocket.close()
        return
    
    await websocket.accept()
    
    userNotdefinedMsg = {"message": "UsernameNotDefined"}
    plsScanKeycardMsg = {"message": "PleaseScanKeycard"}
    sessionValidatedMsg = {"message": "SessionValidated"}
    
    try:
        
        await websocket.send_json(userNotdefinedMsg)
        response = await websocket.receive_json()
        
        if response["username"] == "" or response["username"] is None:
            await websocket.close()
            return
        else:
            logger.info(f"Received username: {response}")
        
        username = response["username"]
        logger.info(f"Username: {username}")
        session_store[session]["username"] = username
        
        await websocket.send_json(plsScanKeycardMsg)
        
        response = await WaitforKeyCard()
        logger.info(f"Received keycard: {response}")

        if response is None:
            await websocket.close()
            return
        
        logger.info(f"Received keycard: {response}")
        newLogin = Login(username=username, keycard=response)
        add_new_login(newLogin)
        logger.info(f"Added new login: {newLogin}")
        
        await websocket.send_json(sessionValidatedMsg)
        
    except TimeoutError as e:
        logger.error(e)
        await websocket.send_json("{TimeoutWaiting}")
        await websocket.close()
    except Exception as e:
        logger.error(e)
        await websocket.send_json("{Error}")
        await websocket.close()
    finally:
        await websocket.close()
    

@app.get("/sensors")
def sensor_triggered(request: Request,session: str = Cookie(default=None)):
    if session is None:
        return RedirectResponse(url="/")
    if validate_session(session) == False:
        return RedirectResponse(url="/")
    
    return templates.TemplateResponse("sensorTriggered.html", {"request": request})


@app.get("/api/sensors")
def get_sensors():
    sensors:list[Sensor] = read_sensors_definition()
    if not sensors:
        return {"sensors": []}
    
    return {"sensors": [ {"id":sensor.id,"name":sensor.name,"type":sensor.type,"description":sensor.description} for sensor in sensors]}
    
@app.get("/api/sensorLogdata/{id}")
def get_sensors(request: Request,id: int,session: str = Cookie(default=None)):
    sensorLogData:list[SensorsLog] =read_sensor_log_by_sensor_id(id)
    if not sensorLogData:
        return {"sensorLogdata": []}

    timestamps = [entry.timestamp.isoformat() for entry in sensorLogData]
    values = [entry.value for entry in sensorLogData]
    return {"timestamps": timestamps, "values": values} 

from datetime import datetime

@app.get("/api/add_test_sensor")
def Create_test_sensor():
    sensor = Sensor(name="Test Sensor", type="Test Type", description="Test Description")
    add_new_sensor(sensor)
    add_new_sensor_log(SensorsLog(sensor_id=1, value=1.0, timestamp=datetime.now()))
    add_new_sensor_log(SensorsLog(sensor_id=1, value=0.0, timestamp=datetime.now()))
    add_new_sensor_log(SensorsLog(sensor_id=1, value=1.0, timestamp=datetime.now()))
    add_new_sensor_log(SensorsLog(sensor_id=1, value=0.0, timestamp=datetime.now()))
    add_new_sensor_log(SensorsLog(sensor_id=1, value=1.0, timestamp=datetime.now()))
    add_new_sensor_log(SensorsLog(sensor_id=1, value=0.0, timestamp=datetime.now()))
    return {"message": "Sensor added successfully"}



@app.websocket("/keycard")
async def websocket_endpoint(websocket: WebSocket, session: str = Cookie(default=None)):
    if session is None:
        logger.info("No session cookie found")
        await websocket.close()
        return
    
    await websocket.accept()
    
    userNotdefinedMsg = {"message": "UsernameNotDefined"}
    plsScanKeycardMsg = {"message": "PleaseScanKeycard"}
    sessionValidatedMsg = {"message": "SessionValidated"}
    InvalidKeyCard = {"message": "InvalidKeyCard"}
    
    try:
        
        await websocket.send_json(userNotdefinedMsg)
        response = await websocket.receive_json()
        
        if response["username"] == "" or response["username"] is None:
            return
        else:
            logger.info(f"Received username: {response}")
        
        username = response["username"]
        logger.info(f"Username: {username}")
        session_store[session]["username"] = username
        
        await websocket.send_json(plsScanKeycardMsg)
        
        response = await WaitforKeyCard()
        logger.info(f"Received keycard: {response}")

        if response is None:
            return
        
        logger.info(f"Received keycard: {response}")
        
        if login(Login(username=username, keycard=response)) == False:
            await websocket.send_json("{InvalidKeyCard}")
            logger.info("Invalid keycard")
            return
        else:
            session_store[session]["KeyCardValidated"] = True
            logger.info("Keycard validated")
        await websocket.send_json(sessionValidatedMsg)
        logger.info("Session validated")

        
    except TimeoutError as e:
        logger.error(e)
        await websocket.send_json("{TimeoutWaiting}")
    except Exception as e:
        logger.error(e)
        await websocket.send_json("{Error}")