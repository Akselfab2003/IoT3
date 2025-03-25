  #include <Arduino.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>


  // OLED Display
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels

  // Declaration for SSD1306 display connected using I2C
  #define OLED_RESET     -1 // Reset pin
  #define SCREEN_ADDRESS 0x3C
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

  //I2C Pins
  #define SENSOR_PIN_Lose 34    
  #define SENSOR_PIN_Board 35

  int peopleCount = 0; //change to extern when PeopleCounter is implemented




  void ReadSensorLose();
  void ReadSensorBoard();
  void updateDisplay();
  //void ReadAllValues();

  void setup() {
    // put your setup code here, to run once:

    Serial.begin(9600);
    pinMode(SENSOR_PIN_Lose, INPUT);
    pinMode(SENSOR_PIN_Board, INPUT);

    Serial.println("Initializing OLED display...");
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
      // Clear the buffer.
    display.clearDisplay();

    // Display ASCII Characters
    display.setTextColor(WHITE);
    display.setCursor(0.5,0.5);
    display.setTextSize(3);
    display.write(3);
    display.display();
    delay(2000);
    display.clearDisplay();

      
    }

  void loop() {
    // put your main code here, to run repeatedly:
    //ReadAllValues();
    //ReadSensorLose();
    //ReadSensorBoard();

    updateDisplay();
    delay(500);
  }

  // put function definitions here:
  int myFunction(int x, int y) {
    return x + y;
  }



  void ReadSensorLose(){
    int state = digitalRead(SENSOR_PIN_Lose);
    if (state == LOW)
    {
      Serial.println("Object Detected Lose");
    }
    else
    {
      Serial.println("No Object Detected Lose");
    }
  }

  void ReadSensorBoard(){
    int state = digitalRead(SENSOR_PIN_Board);
    if (state == LOW)
    {
      Serial.println("Object Detected Board");
    }
    else
    {
      Serial.println("No Object Detected Board");
    }
  }

  void updateDisplay(){
    display.clearDisplay();

    
    // Display the people count next to the icon
  display.setTextSize(1);
  display.setCursor(20, 0);
  display.println("People Count:");
  display.setCursor(20, 16);
  display.setTextSize(2);
  display.println(peopleCount);
  
  // Show on display
  display.display();
    
  }

  void ReadAllValues(){
    int lightValue_Board = analogRead(SENSOR_PIN_Board);
    int lightValue_Lose = analogRead(SENSOR_PIN_Lose);
    Serial.println("Board: " + String(lightValue_Board) + " \nLose: " + String(lightValue_Lose));

  }



  