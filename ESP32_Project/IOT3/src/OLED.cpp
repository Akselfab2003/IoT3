#include "OLED.h"

// Initialize the OLED display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initializeOLED() {
    Serial.println("Initializing OLED display...");
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }

    // Clear the buffer
    display.clearDisplay();

    // Display ASCII Characters
    display.setTextColor(WHITE);
    display.setCursor(0.5, 0.5);
    display.setTextSize(3);
    display.write(3);
    display.display();
    delay(2000);
    display.clearDisplay();
}

void updateDisplay() {
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