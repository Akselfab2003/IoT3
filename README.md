# IOT 3 Persontæller

# Setup

### Requirements
- docker desktop
- docker-compose
- esp32


# Configuration

IMPORTANT NOTE: The computer running the docker containers and the ESP32 must be on the same network.

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

