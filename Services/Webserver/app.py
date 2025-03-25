from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from starlette.requests import Request
from PeopleCounter import ReadPeopleCounterFromDB

app = FastAPI()

# Mount static folder
app.mount("/static", StaticFiles(directory="static"), name="static")

import os
print("Current working directory:", os.getcwd())

app.mount("..\\..\\DB\\", StaticFiles(directory="DB"), name="DB")
# Set up templates
templates = Jinja2Templates(directory="templates")

@app.get("/")
def index(request: Request):
    return templates.TemplateResponse("index.html",{"request":request,"test": "Hello, World!","PeopleCount":ReadPeopleCounterFromDB()})
