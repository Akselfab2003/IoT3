  #include <Arduino.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #include <KeyCardScanner.h>
  #include <PeopleCount.h>
  #include <wifi_configuration.h>
  #include <DataTransporter.h>
  #include <NTPTime.h>
  #include <OLED.h>


  void setup() {
    // put your setup code here, to run once:

    Serial.begin(115200);

    initialize_WiFi();

    initializeNTP();

    InitializeMQTT();

    xTaskCreate(ProcessMQTT,"ProcessMQTT", 10000, NULL, 1, NULL);
    
    setup1();
    setup2();

    // Initialize the OLED display
    initializeOLED();
    }

  void loop() {

    updateDisplay();
    //PublishData(Topics::PersonDetected, "System Initialized");
    //loop1();
    CheckIfPersonEntered();

    loop2();  
  }