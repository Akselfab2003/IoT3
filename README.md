# IOT 3 Persontæller

# Setup

### Requirements
- docker desktop
- docker-compose
- esp32


# Configuration

IMPORTANT NOTE: The computer running the docker containers and the ESP32 must be on the same network.

Database is automatically created on docker startup and can be reset by deleting the DB file located at: `Services\Webserver\DB\IOT3_DB.db`

### ESP32

before you upload the code to the ESP32, you need to create a file called `env_secrets.ini` in the `ESP32_Project\IOT3` directory. this file should contain the following information:
```ini
[env:esp32dev]
build_flags =
  -D WIFI_SSID=\"TempWifiSSID\"
  -D WIFI_PASS=\"TempWifiPass\"
  -D mqtt_server=\"127.0.0.1\"
```
where the `WIFI_SSID` and `WIFI_PASS` are the name and password of your wifi network. (Leave the "\" at the end)
Replace mqtt_server with your pc's ip address

### Docker

We use docker to run the MQTT broker to communicate between the ESP32 and the web application.
To run the docker compose file you need to run the following command in the root directory of the project
```bash 
docker compose -f 'Services\docker-compose.yaml' up -d --build 
```
### ESP32 Pin Configuration

Below is the list of pins used on the ESP32 and their respective connections:

#### **KeyCardScanner (RFID Reader)**
- **SS_PIN (GPIO 5)**: Connected to the SDA/SS pin of the RC522 RFID reader.
- **RST_PIN (GPIO 27)**: Connected to the RST pin of the RC522 RFID reader.
- **SPI Pins**:
  - **SCK (GPIO 18)**: Serial Clock for SPI communication.
  - **MISO (GPIO 19)**: Master In Slave Out for SPI communication.
  - **MOSI (GPIO 23)**: Master Out Slave In for SPI communication.

#### **PeopleCount (Movement Sensors)**
- **SENSOR_PIN_Lose (GPIO 34)**: Connected to the first movement sensor (e.g., entry sensor).
- **SENSOR_PIN_Board (GPIO 35)**: Connected to the second movement sensor (e.g., exit sensor).

#### **OLED Display**
- **SDA (GPIO 21)**: Connected to the SDA pin of the OLED display for I2C communication.
- **SCL (GPIO 22)**: Connected to the SCL pin of the OLED display for I2C communication.
- **RESET (GPIO -1)**: Not connected (default reset pin is used internally by the library).

Ensure that the above pins are correctly wired to the respective components for proper functionality.
![alt text](image.png)
