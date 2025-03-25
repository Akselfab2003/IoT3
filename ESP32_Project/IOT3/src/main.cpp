  #include <Arduino.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #include <KeyCardScanner.h>
  #include <PeopleCount.h>


  // OLED Display
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels

  // Declaration for SSD1306 display connected using I2C
  #define OLED_RESET     -1 // Reset pin
  #define SCREEN_ADDRESS 0x3C
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

  //I2C Pins




  void ReadSensorLose();
  void ReadSensorBoard();
  void updateDisplay();
  //void ReadAllValues();

  void setup() {
    // put your setup code here, to run once:

    Serial.begin(115200);
    setup1();
    setup2();
    


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

    updateDisplay();

    //loop1();
    CheckIfPersonEntered();
    loop2();  
    delay(500);
  }

  // put function definitions here:
  int myFunction(int x, int y) {
    return x + y;
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