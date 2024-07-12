#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Define pin connections
#define TFT_CS   D1
#define TFT_RST  D2
#define TFT_DC   D3
#define TFT_MOSI D7
#define TFT_SCK  D5
#define TFT_MISO D6

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// Create Adafruit ILI9341 object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the TFT screen
  tft.begin();

  // Rotate the screen (optional)
  tft.setRotation(1);

  // Fill screen with black color
  tft.fillScreen(ILI9341_BLACK);

  // Set text color to white
  tft.setTextColor(ILI9341_WHITE);

  // Set text size
  tft.setTextSize(2);

  // Print "Hello, World!" to the center of the screen
  //tft.setCursor(50, 120); // Adjust the position as needed
  //tft.println("Hello, World!");

  // Get screen dimensions
  int screenWidth = tft.width();
  int screenHeight = tft.height();

  
  tft.drawRect(0, 0, screenWidth, screenHeight, RED);
  tft.drawLine(screenWidth/2, 0, screenWidth/2, screenHeight, BLUE);
  tft.drawLine(0, screenHeight/2, screenWidth, screenHeight/2, GREEN);

  
  // Create the text to display
  String screenSizeText = String(screenWidth) + "x" + String(screenHeight);

  // Calculate the position to center the text
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(screenSizeText, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (screenWidth - w) / 2;
  int16_t y = (screenHeight - h) / 2;

  // Print the screen size to the center of the screen
  tft.setCursor(x, y);
  tft.println(screenSizeText);
  
}

void loop() {
  // No need to repeat anything in the loop
}
