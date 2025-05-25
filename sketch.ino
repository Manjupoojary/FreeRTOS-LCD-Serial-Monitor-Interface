#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD configuration (for Wokwi simulator)
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// Create LCD object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Shared variable for the message
char displayMessage[32] = "Ready";

void setup() {
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Serial to LCD");
  lcd.setCursor(0, 1);
  lcd.print(displayMessage);
  
  // Create just one task for handling both serial and LCD
   xTaskCreate(MainTask,"MainTask",128,NULL,1,NULL);
  
  // Start scheduler
  vTaskStartScheduler();
}

void loop() {
  // Empty - FreeRTOS scheduler is running
}

void MainTask(void *pvParameters) {
  (void) pvParameters;
  
  char inputBuffer[32];
  uint8_t index = 0;
  
  while (1) {
    // Check for serial input
    if (Serial.available()) {
      char c = Serial.read();
      
      if (c == '\n' || index >= sizeof(inputBuffer) - 1) {
        // Terminate the string
        inputBuffer[index] = '\0';
        
        // Update the display message
        strncpy(displayMessage, inputBuffer, sizeof(displayMessage));
        
        // Update LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Serial Input:");
        lcd.setCursor(0, 1);
        lcd.print(displayMessage);
        
        // Reset buffer
        index = 0;
      } else {
        inputBuffer[index++] = c;
      }
    }
    
    // Small delay to prevent busy waiting
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}