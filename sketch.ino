/**************************************************************************************************
 * NEUROMANCER 88 (DOS) PAX Decoder
 * Nov 2026
 * https://github.com/ekn394/Neuromancer-88-PAX-Decoder/
 *
 * OLED (SSD1306) and Encoder skeleton code from https://github.com/alanesq/BasicOLEDMenu
 *
 **************************************************************************************************/


#include <Arduino.h>         // required by platformIO
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SSD1306_NO_SPLASH


// ----------------------------------------------------------------
//                         S E T T I N G S
// ----------------------------------------------------------------

    //  Arduino Mega
      #define encoder0PinA  2                   // Rotary encoder gpio pin
      #define encoder0PinB  3                   // Rotary encoder gpio pin
      #define encoder0Press 4                   // Rotary encoder button gpio pin
      #define OLEDC 0                           // oled clock pin (set to 0 for default)
      #define OLEDD 0                           // oled data pin
      #define OLEDE 0                           // oled enable pin

    // oLED
      #define OLED_ADDR 0x3C                    // OLED i2c address
      #define SCREEN_WIDTH 128                  // OLED display width, in pixels (usually 128)
      #define SCREEN_HEIGHT 64                  // OLED display height, in pixels (64 for larger oLEDs)
      #define OLED_RESET -1                     // Reset pin gpio (or -1 if sharing Arduino reset pin)

    // Misc
      const int serialDebug = 1;
      const int iLED = 2;                       // onboard indicator led gpio pin
      #define BUTTONPRESSEDSTATE 0              // rotary encoder gpio pin logic level when the button is pressed (usually 0)
      #define DEBOUNCEDELAY 20                  // debounce delay for button inputs
      const int menuTimeout = 10;               // menu inactivity timeout (seconds)
      const bool menuLargeText = 0;             // show larger text when possible (if struggling to read the small text)
      const int maxmenuItems = 12;              // max number of items used in any of the menus (keep as low as possible to save memory)
      const int itemTrigger = 2;                // rotary encoder - counts per tick (varies between encoders usually 1 or 2)
      const int topLine = 18;                   // y position of lower area of the display (18 with two colour displays)
      const byte lineSpace1 = 9;                // line spacing for textsize 1 (small text)
      const byte lineSpace2 = 17;               // line spacing for textsize 2 (large text)
      const int displayMaxLines = 5;            // max lines that can be displayed in lower section of display in textsize1 (5 on larger oLeds)
      const int MaxmenuTitleLength = 10;        // max characters per line when using text size 2 (usually 10)
      int choiceA = 0; 
  	  int choiceB = 0;
  	  int a_b_sum = 0;
      int choiceC = 0; 

long arrayC[16][16] = {
  {113667,60734,131265,63124,162324,73611,61254,127170,67144,31776,31770,121777,117372,11772,115721,21655},
  {162324,73611,61254,127170,67144,31776,31770,121777,117372,11772,115721,21655,113667,60734,131265,63124},
  {152777,40327,4713,51435,157223,56201,54127,114352,126443,30654,14527,132077,67123,36160,13026,152336},
  {056201,54127,114352,126443,30654,14527,132077,67123,36160,13026,152336,152777,40327,4713,51435,157223},
  {71312,44321,76631,61543,70012,34770,43267,21400,65110,61723,51666,31270,45771,41317,61456,44312},
  {43267,21400,65110,61723,51666,31270,45771,41317,61456,44312,71312,44321,76631,61543,70012,34770},
  {61255,7362,61645,36112,50127,74655,67237,34755,23671,71724,206,22372,42074,71226,61627,1326},
  {34755,23671,71724,206,22372,42074,71226,61627,1326,61255,7362,61645,36112,50127,74655,67237},
  {5517,7107,4102,170,5116,6160,5165,17,1053,5423,3173,423,5120,6116,3121,177},
  {4102,170,5116,6160,5165,17,1053,5423,3173,423,5120,6116,3121,177,5517,7107},
  {1337,4300,72,4120,1316,3601,3347,1777,2730,4327,703,6040,5150,6162,2174,6124},
  {4327,703,6040,5150,6162,2174,6124,1337,4300,72,4120,1316,3601,3347,1777,2730},
  {512,700,254,17,533,673,333,434,300,410,54,313,733,616,167,213},
  {533,673,333,434,300,410,54,313,733,616,167,213,512,700,254,17},
  {236,714,24,424,473,614,672,321,72,507,725,571,430,775,103,45},
  {473,614,672,321,72,507,725,571,430,775,103,45,236,714,24,424}
};

const uint8_t PROGMEM frame0[1024] = {
  0x00, 0x00, 0x00, 0x00, 0x1e, 0xc0, 0x00, 0x00, 0x00, 0xcf, 0xfc, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0f, 0x86, 0x00, 0x00, 0x00, 0x4f, 0xfc, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x60, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x02, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xc6, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x01, 0xc0, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x09, 0xff, 0xfe, 0x00, 0x24, 0x00, 0x00, 0x08,
  0x00, 0x00, 0xf0, 0x00, 0x0e, 0x01, 0x00, 0x00, 0x09, 0xff, 0xfe, 0x00, 0x38, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xfc, 0x00, 0x0e, 0x02, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x98, 0x00, 0x00, 0x00,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x07, 0xff, 0xff, 0x00, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0xf0, 0x3f, 0x3f, 0x70, 0xbe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0x3c, 0x06, 0x00, 0x00, 0x10, 0xff, 0xff, 0x80, 0x0e, 0xff, 0x00, 0x37,
  0x00, 0x00, 0x00, 0x78, 0x33, 0xfe, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x80, 0x1f, 0xfe, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x3c, 0x07, 0x9f, 0xc0, 0x00, 0x14, 0x0c, 0x7f, 0x80, 0x0c, 0xfe, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x14, 0x2c, 0x07, 0xe0, 0x00, 0x01, 0xe0, 0x1f, 0x81, 0x04, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0e, 0x17, 0x9f, 0xe0, 0x00, 0x01, 0xff, 0x8f, 0xf3, 0xff, 0xfe, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0f, 0xbf, 0xff, 0xf0, 0x07, 0x03, 0xfc, 0xcf, 0xff, 0xfc, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xf0, 0x7c, 0x3e, 0xfc, 0x67, 0xf3, 0xf8, 0x00, 0x00, 0x60,
  0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xb8, 0xbf, 0x7d, 0xf0, 0x27, 0xe7, 0xfe, 0x00, 0x00, 0x60,
  0x00, 0x00, 0x00, 0x07, 0xfd, 0xff, 0xb8, 0x04, 0x3d, 0xf0, 0x37, 0x83, 0xf8, 0x00, 0x00, 0xc0,
  0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xfb, 0xff, 0x7c, 0x7c, 0x37, 0xb3, 0xfc, 0x00, 0x01, 0x80,
  0x00, 0x00, 0x00, 0x03, 0xff, 0xfe, 0x78, 0x78, 0x28, 0x7c, 0x37, 0xc1, 0xfe, 0x00, 0x01, 0x80,
  0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xf8, 0x70, 0x0f, 0xfe, 0x77, 0x81, 0x1f, 0x00, 0x01, 0x80,
  0x00, 0x00, 0x00, 0x09, 0xff, 0xff, 0xf8, 0x0f, 0xfc, 0xfc, 0xf7, 0x9e, 0x3e, 0x08, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x09, 0xff, 0xf7, 0xb3, 0xff, 0xfc, 0xfc, 0x77, 0xcf, 0xf8, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x09, 0xfb, 0xfc, 0x30, 0x1f, 0xef, 0xff, 0xff, 0xc6, 0xd0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0f, 0xf9, 0xf8, 0x70, 0x1f, 0xff, 0x38, 0x0f, 0xc0, 0xc8, 0x00, 0x40, 0x08,
  0x00, 0x00, 0x00, 0x07, 0xfb, 0xfc, 0x64, 0x1f, 0xff, 0x80, 0x1f, 0x80, 0x38, 0x00, 0x0c, 0x00,
  0x00, 0x00, 0x00, 0x31, 0xff, 0xfb, 0xce, 0x1f, 0xfe, 0xe8, 0xff, 0xc0, 0xf8, 0x00, 0x30, 0x00,
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0x94, 0x1f, 0xff, 0x7f, 0xff, 0xf1, 0xe0, 0x00, 0x60, 0x00,
  0x00, 0x00, 0x00, 0x05, 0xff, 0xff, 0x7d, 0x3f, 0xff, 0xff, 0xff, 0xf0, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x34, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x5a, 0x02,
  0x00, 0x00, 0x00, 0x06, 0x3e, 0x02, 0x5c, 0x1f, 0xfe, 0x0f, 0xff, 0xf3, 0xb8, 0x01, 0xa0, 0x04,
  0x00, 0x00, 0x20, 0x06, 0x0e, 0x04, 0x4d, 0x3f, 0xff, 0x07, 0xff, 0xf3, 0xd8, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x07, 0x0e, 0x04, 0x5f, 0x1f, 0xff, 0xc3, 0xff, 0xf1, 0x58, 0x20, 0x00, 0x08,
  0x00, 0x00, 0x08, 0x07, 0x0f, 0x0c, 0x3e, 0x1f, 0xff, 0xc7, 0xff, 0xfb, 0xfc, 0x10, 0x00, 0x00,
  0x00, 0x00, 0xdc, 0x07, 0x8d, 0x34, 0x00, 0x1f, 0xff, 0xf7, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x30,
  0x00, 0x00, 0x60, 0x27, 0x86, 0x34, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xfe, 0x7c, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x60, 0x37, 0x2a, 0x08, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x40, 0x37, 0x00, 0x08, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfb, 0xfc, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xc0, 0x1f, 0xc0, 0x08, 0x00, 0x00, 0x7c, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x81, 0xff, 0x00, 0x0c, 0x00, 0x00, 0x7e, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x40,
  0x00, 0x00, 0x1f, 0xff, 0xc4, 0x08, 0x10, 0x02, 0x7e, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x80,
  0x00, 0x00, 0x7f, 0xf7, 0x80, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0x7c, 0x00, 0x05, 0x80,
  0x00, 0x03, 0xff, 0xf1, 0xf1, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x01, 0x00,
  0x00, 0x1f, 0xf2, 0xfd, 0xc1, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xfe, 0xdc, 0x00, 0x02, 0x00,
  0x00, 0x1e, 0xc4, 0x3f, 0xc0, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xfe, 0xf8, 0x00, 0x24, 0x00,
  0x00, 0x0f, 0x98, 0x0f, 0xe2, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x28, 0x00,
  0x00, 0x07, 0x10, 0x01, 0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x10, 0x00,
  0x00, 0x02, 0x10, 0x07, 0xb0, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x06, 0x0f, 0xb2, 0x03, 0xf8, 0x00, 0x51, 0xff, 0xff, 0xf1, 0xf0, 0x01, 0x40, 0x00,
  0x01, 0xc0, 0x06, 0x21, 0x80, 0x01, 0xf8, 0x00, 0x40, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x80, 0x00,
  0x03, 0xe0, 0x00, 0x03, 0xc4, 0x01, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
  0x07, 0xe0, 0x00, 0x01, 0x8c, 0x03, 0xe0, 0x00, 0xa8, 0x3f, 0xff, 0xff, 0xe0, 0x01, 0x00, 0x00,
  0x07, 0xf0, 0x00, 0x00, 0x04, 0x03, 0xfc, 0x00, 0xfe, 0x3f, 0xff, 0xff, 0xc0, 0x10, 0x00, 0x00,
  0x07, 0xe0, 0x00, 0x00, 0x62, 0x01, 0x40, 0x00, 0x7e, 0x0f, 0xff, 0xff, 0x80, 0x24, 0x00, 0x00,
  0x03, 0xc0, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x7f, 0x0f, 0xff, 0xff, 0x80, 0x10, 0x00, 0x00,
  0x00, 0xc0, 0x00, 0x01, 0xf0, 0x40, 0x00, 0x00, 0x7f, 0x9f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x00, 0x00, 0xf0, 0x40, 0x00, 0x00, 0x7f, 0xcf, 0xff, 0xf0, 0x80, 0x60, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0xd8, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xf3, 0x03, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0xb8, 0x00, 0x00, 0x40, 0x1f, 0xff, 0xff, 0x10, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x98, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xde, 0x0c, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xde, 0x38, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x10, 0x03, 0xff, 0xff, 0xff, 0x8e, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0x8c, 0x60, 0x00, 0x00, 0x00
};
// 
// Website : https://www.oledanimationmaker.com/
// Author :- Ashish ( ashish_y_794 )
// -------------------------------------------------------------------------------------------------


// forward declarations
  void doEncoder();
  void demoMenu();
  void menuActions();
  void value1();
  void menuValues();
  void reUpdateButton();
  void serviceMenu();
  int serviceValue(bool _blocking);
  void createList(String _title, int _noOfElements, String *_list);
  void displayMessage(String _title, String _message);
  void resetMenu();


// menus
  // modes that the menu system can be in
  enum menuModes {
      off,                                  // display is off
      menu,                                 // a menu is active
      value,                                // 'enter a value' none blocking is active
      message,                              // displaying a message
      blocking                              // a blocking procedure is in progress (see enter value)
  };
  menuModes menuMode = off;                 // default mode at startup is off

  struct oledMenus {
    // menu
    String menuTitle = "";                    // the title of active mode
    int noOfmenuItems = 0;                    // number if menu items in the active menu
    int selectedMenuItem = 0;                 // when a menu item is selected it is flagged here until actioned and cleared
    int highlightedMenuItem = 0;              // which item is curently highlighted in the menu
    String menuItems[maxmenuItems+1];         // store for the menu item titles
    uint32_t lastMenuActivity = 0;            // time the menu last saw any activity (used for timeout)
    // 'enter a value'
    int mValueEntered = 0;                    // store for number entered by value entry menu
    int mValueLow = 0;                        // lowest allowed value
    int mValueHigh = 0;                       // highest allowed value
    int mValueStep = 0;                       // step size when encoder is turned
  };
  oledMenus oledMenu;

  struct rotaryEncoders {
    volatile int encoder0Pos = 0;             // current value selected with rotary encoder (updated by interrupt routine)
    volatile bool encoderPrevA;               // used to debounced rotary encoder
    volatile bool encoderPrevB;               // used to debounced rotary encoder
    uint32_t reLastButtonChange = 0;          // last time state of button changed (for debouncing)
    bool encoderPrevButton = 0;               // used to debounce button
    int reButtonDebounced = 0;                // debounced current button state (1 when pressed)
    const bool reButtonPressedState = BUTTONPRESSEDSTATE;  // the logic level when the button is pressed
    const uint32_t reDebounceDelay = DEBOUNCEDELAY;        // button debounce delay setting
    bool reButtonPressed = 0;                 // flag set when the button is pressed (it has to be manually reset)
  };
  rotaryEncoders rotaryEncoder;

// oled SSD1306 display connected to I2C
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// -------------------------------------------------------------------------------------------------
//                                 The custom menus go below here
// -------------------------------------------------------------------------------------------------


// Start the default menu
void defaultMenu() {
  demoMenu();
}

//                -----------------------------------------------

//    when an item is selected it is actioned in menuActions()
void demoMenu() {
  resetMenu();                          // clear any previous menu
  menuMode = menu;                      // enable menu mode
  oledMenu.noOfmenuItems = 2;           // set the number of items in this menu
  oledMenu.menuTitle = "Neuromancer 88";     // menus title (used to identify it)
  oledMenu.menuItems[1] = "PAX Decoder";
  oledMenu.menuItems[2] = "About";
} //demoMenu

void AMenu() {
  resetMenu();                          // clear any previous menu
  menuMode = menu;                      // enable menu mode
  oledMenu.noOfmenuItems = 9;           // set the number of items in this menu
  oledMenu.menuTitle = "A";     // menus title (used to identify it)
  oledMenu.menuItems[1] = "Night City";      // set the menu items
  oledMenu.menuItems[2] = "Chatsubo";
  oledMenu.menuItems[3] = "Cyberspace";
  oledMenu.menuItems[4] = "Gemeinschaft";
  oledMenu.menuItems[5] = "Crazy Edo";
  oledMenu.menuItems[6] = "Microsofts";
  oledMenu.menuItems[7] = "Cheap Hotel";
  oledMenu.menuItems[8] = "Julius Deane";
  oledMenu.menuItems[9] = "Next Page";
}  // AMenu

void AMenu2() {
  resetMenu();                          // clear any previous menu
  menuMode = menu;                      // enable menu mode
  oledMenu.noOfmenuItems = 9;           // set the number of items in this menu
  oledMenu.menuTitle = "A cont";     // menus title (used to identify it)
  oledMenu.menuItems[1] = "Donut World";
  oledMenu.menuItems[2] = "Lawbot";
  oledMenu.menuItems[3] = "Database";
  oledMenu.menuItems[4] = "Shiva";
  oledMenu.menuItems[5] = "Freeside";
  oledMenu.menuItems[6] = "Softwarez";
  oledMenu.menuItems[7] = "Matrix";
  oledMenu.menuItems[8] = "Matt Shaw";
  oledMenu.menuItems[9] = "Previous Page";
}  // AMenu2

void BMenu() {
  resetMenu();                          // clear any previous menu
  menuMode = menu;                      // enable menu mode
  oledMenu.noOfmenuItems = 9;           // set the number of items in this menu
  oledMenu.menuTitle = "B";     // menus title (used to identify it)
  oledMenu.menuItems[1] = "Emperor Norton";      // set the menu items
  oledMenu.menuItems[2] = "Cyberjack";
  oledMenu.menuItems[3] = "Ratz";
  oledMenu.menuItems[4] = "Holografix";
  oledMenu.menuItems[5] = "Larry Moe";
  oledMenu.menuItems[6] = "PAX";
  oledMenu.menuItems[7] = "Skill Chips";
  oledMenu.menuItems[8] = "Shin's Pawn";
  oledMenu.menuItems[9] = "Next Page";
}  // BMenu

void BMenu2() {
  resetMenu();                          // clear any previous menu
  menuMode = menu;                      // enable menu mode
  oledMenu.noOfmenuItems = 9;           // set the number of items in this menu
  oledMenu.menuTitle = "B cont";     // menus title (used to identify it)
  oledMenu.menuItems[1] = "Gentleman Loser";
  oledMenu.menuItems[2] = "Maas Biolabs";
  oledMenu.menuItems[3] = "Justice Booth";
  oledMenu.menuItems[4] = "Cyberdeck";
  oledMenu.menuItems[5] = "Comlink";
  oledMenu.menuItems[6] = "ICEbreaker";
  oledMenu.menuItems[7] = "ROM Construct";
  oledMenu.menuItems[8] = "Cowboy";
  oledMenu.menuItems[9] = "Previous Page";
}  // BMenu2

void CMenu() {
  resetMenu();                          // clear any previous menu
  menuMode = menu;                      // enable menu mode
  oledMenu.noOfmenuItems = 9;           // set the number of items in this menu
  oledMenu.menuTitle = "C";     // menus title (used to identify it)
  oledMenu.menuItems[1] = "Asano computing";      // set the menu items
  oledMenu.menuItems[2] = "Spacedock";
  oledMenu.menuItems[3] = "Space Colony";
  oledMenu.menuItems[4] = "Flatline";
  oledMenu.menuItems[5] = "Zion cluster";
  oledMenu.menuItems[6] = "Marcus Garvey";
  oledMenu.menuItems[7] = "Cryptology";
  oledMenu.menuItems[8] = "AI";
  oledMenu.menuItems[9] = "Next Page";
}  // CMenu

void CMenu2() {
  resetMenu();                          // clear any previous menu
  menuMode = menu;                      // enable menu mode
  oledMenu.noOfmenuItems = 16;           // set the number of items in this menu
  oledMenu.menuTitle = "C cont";     // menus title (used to identify it)
  oledMenu.menuItems[1] = "Bank of Berne";
  oledMenu.menuItems[2] = "Bank of Zurick";
  oledMenu.menuItems[3] = "Chiba City";
  oledMenu.menuItems[4] = "Fuji Electric";
  oledMenu.menuItems[5] = "Holy Joystick";
  oledMenu.menuItems[6] = "Compu-Judge";
  oledMenu.menuItems[7] = "Hitachi Biotech";
  oledMenu.menuItems[8] = "Ono-Sendai";
  oledMenu.menuItems[9] = "Previous Page";
}  // CMenu2


// Actions for menu selections are put in here
//    note: it would probably be better to put these in their own seperate procedures but I have kept
//          them all in this one place to make it easier to follow
void menuActions() {

  if (oledMenu.menuTitle == "Neuromancer 88") {    // actions when an item is selected in "demo_menu"
    // demonstrate quickly create a menu from a list
    if (oledMenu.menuTitle == "Neuromancer 88" && oledMenu.selectedMenuItem == 1) {
      AMenu();
    }
    if (oledMenu.menuTitle == "Neuromancer 88" && oledMenu.selectedMenuItem == 2) {
      displayMessage("","Evan Nordquist \n2016");
    }
  }

     
  if (oledMenu.menuTitle == "A") {    // actions when an item is selected in "A_menu"  
  // Dealing with choices in the A Menu
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 1) {choiceA = 1; BMenu();}
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 2) {choiceA = 2; BMenu();}
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 3) {choiceA = 3; BMenu();}
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 4) {choiceA = 4; BMenu();}
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 5) {choiceA = 5; BMenu();}
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 6) {choiceA = 6; BMenu();}
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 7) {choiceA = 7; BMenu();}
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 8) {choiceA = 8; BMenu();}
    if (oledMenu.menuTitle == "A" && oledMenu.selectedMenuItem == 9) {AMenu2();}
  }

  if (oledMenu.menuTitle == "A cont") {    // actions when an item is selected in "a_menu"  
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 1) {choiceA = 9; BMenu();}
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 2) {choiceA = 10; BMenu();}
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 3) {choiceA = 11; BMenu();}
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 4) {choiceA = 12; BMenu();}
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 5) {choiceA = 13; BMenu();}
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 6) {choiceA = 14; BMenu();}
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 7) {choiceA = 15; BMenu();}
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 8) {choiceA = 16; BMenu();}
    if (oledMenu.menuTitle == "A cont" && oledMenu.selectedMenuItem == 9) {AMenu();}
  }

  if (oledMenu.menuTitle == "B") {    // actions when an item is selected in "b_menu"    
  // Dealing with choices in the B Menu
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 1) {choiceB = 16; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 2) {choiceB = 15; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 3) {choiceB = 14; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 4) {choiceB = 13; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 5) {choiceB = 12; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 6) {choiceB = 11; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 7) {choiceB = 10; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 8) {choiceB = 9; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B" && oledMenu.selectedMenuItem == 9) {BMenu2();}
  }

  if (oledMenu.menuTitle == "B cont") {    // actions when an item is selected in "b_menu"    
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 1) {choiceB = 8; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 2) {choiceB = 7; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 3) {choiceB = 6; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 4) {choiceB = 5; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 5) {choiceB = 4; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 6) {choiceB = 3; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 7) {choiceB = 2; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 8) {choiceB = 1; a_b_sum = ((choiceA + choiceB) % 16); CMenu();}
    if (oledMenu.menuTitle == "B cont" && oledMenu.selectedMenuItem == 9) {BMenu();}
  }

  if (oledMenu.menuTitle == "C") {    // actions when an item is selected in "a_menu"  
  // Dealing with choices in the A Menu
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 1) {choiceC = 1; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 2) {choiceC = 2; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 3) {choiceC = 3; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 4) {choiceC = 4; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 5) {choiceC = 5; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 6) {choiceC = 6; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 7) {choiceC = 7; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 8) {choiceC = 8; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C" && oledMenu.selectedMenuItem == 9) {CMenu2();}
  }
  if (oledMenu.menuTitle == "C cont") {    // actions when an item is selected in "a_menu"  
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 1) {choiceC = 9; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 2) {choiceC = 10; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 3) {choiceC = 11; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 4) {choiceC = 12; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 5) {choiceC = 13; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 6) {choiceC = 14; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 7) {choiceC = 15; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 8) {choiceC = 16; displayMessage(String(arrayC[choiceC-1][a_b_sum-1]),"");}
    if (oledMenu.menuTitle == "C cont" && oledMenu.selectedMenuItem == 9) {CMenu();}
    //    if (oledMenu.menuTitle == "c_cont" && oledMenu.selectedMenuItem == 8) {choiceC = 16; Serial.println(arrayC[choiceC-1][a_b_sum-1]);}
  }
} //MenuActions

//                -----------------------------------------------


// demonstration of how to enter a value
void value1() {
  resetMenu();                           // clear any previous menu
  menuMode = value;                      // enable value entry
  oledMenu.menuTitle = "demo_value";     // title (used to identify which number was entered)
  oledMenu.mValueLow = 0;                // minimum value allowed
  oledMenu.mValueHigh = 100;             // maximum value allowed
  oledMenu.mValueStep = 1;               // step size
  oledMenu.mValueEntered = 50;           // starting value
}


// actions for value entered put in here
void menuValues() {

  // action for "demo_value"
  if (oledMenu.menuTitle == "demo_value") {
    String tString = String(oledMenu.mValueEntered);
    if (serialDebug) Serial.println("demo_value: The value entered was " + tString);
    displayMessage("ENTERED", "\nYou entered\nthe value\n    " + tString);
    // alternatively use 'resetMenu()' here to turn menus off after value entered - or use 'defaultMenu()' to re-start the default menu
  }

  // action for "on or off"
  if (oledMenu.menuTitle == "on or off") {
    if (serialDebug) Serial.println("demo_menu: on off selection was " + String(oledMenu.mValueEntered));
    defaultMenu();
  }
}


// ----------------------------------------------------------------
//                              -setup
// ----------------------------------------------------------------
// called from main setup

void setup() {

  Serial.begin(115200); while (!Serial); delay(50);       // start serial comms
  Serial.println("\n\n\nStarting menu demo\n");

  pinMode(iLED, OUTPUT);     // onboard indicator led

  // configure gpio pins for rotary encoder
    pinMode(encoder0Press, INPUT_PULLUP);
    pinMode(encoder0PinA, INPUT);
    pinMode(encoder0PinB, INPUT);

  // initialise the oled display
    // enable pin
      if (OLEDE != 0) {
        pinMode(OLEDE , OUTPUT);
        digitalWrite(OLEDE, HIGH);
      }
    if (0 == OLEDC) Wire.begin();
    //else Wire.begin(OLEDD, OLEDC);
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
      if (serialDebug) Serial.println(("\nError initialising the oled display"));
    }
    Wire.setClock(100000);

  // Interrupt for reading the rotary encoder position
    rotaryEncoder.encoder0Pos = 0;
    attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);

  defaultMenu();       // start the default menu

  // display greeting message - pressing button will start menu
  //displayMessage("STARTED", "BasicWebserver\nsketch");
  display.clearDisplay();
  display.drawBitmap(0, 0, frame0, 128, 64, SSD1306_WHITE);
  display.display(); 
  delay(400); 
} // setup


// ----------------------------------------------------------------
//                              -loop
// ----------------------------------------------------------------
// called from main loop

void loop() {

  reUpdateButton();      // update rotary encoder button status (if pressed activate default menu)
  menuUpdate();          // update or action the oled menu

  // flash onboard led
    static uint32_t ledTimer = millis();
    if ( (unsigned long)(millis() - ledTimer) > 500 ) {
      digitalWrite(iLED, !digitalRead(iLED));
      ledTimer = millis();
    }
}  // loop


// ----------------------------------------------------------------
//                   -button debounce (rotary encoder)
// ----------------------------------------------------------------
// update rotary encoder current button status

void reUpdateButton() {
    bool tReading = digitalRead(encoder0Press);        // read current button state
    if (tReading != rotaryEncoder.encoderPrevButton) rotaryEncoder.reLastButtonChange = millis();     // if it has changed reset timer
    if ( (unsigned long)(millis() - rotaryEncoder.reLastButtonChange) > rotaryEncoder.reDebounceDelay ) {  // if button state is stable
      if (rotaryEncoder.encoderPrevButton == rotaryEncoder.reButtonPressedState) {
        if (rotaryEncoder.reButtonDebounced == 0) {    // if the button has been pressed
          rotaryEncoder.reButtonPressed = 1;           // flag set when the button has been pressed
          if (menuMode == off) defaultMenu();          // if the display is off start the default menu
        }
        rotaryEncoder.reButtonDebounced = 1;           // debounced button status  (1 when pressed)
      } else {
        rotaryEncoder.reButtonDebounced = 0;
      }
    }
    rotaryEncoder.encoderPrevButton = tReading;            // update last state read
} //reUpdateButton


// ----------------------------------------------------------------
//                    -update the active menu
// ----------------------------------------------------------------

void menuUpdate() {

  if (menuMode == off) return;    // if menu system is turned off do nothing more

  // if no recent activity then turn oled off
    if ( (unsigned long)(millis() - oledMenu.lastMenuActivity) > (menuTimeout * 1000) ) {
      resetMenu();
      return;
    }

    switch (menuMode) {

      // if there is an active menu
      case menu:
        serviceMenu();
        menuActions();
        break;

      // if there is an active none blocking 'enter value'
      case value:
        serviceValue(0);
        if (rotaryEncoder.reButtonPressed) {                        // if the button has been pressed
          menuValues();                                             // a value has been entered so action it
          break;
        }

      // if a message is being displayed
      case message:
        if (rotaryEncoder.reButtonPressed == 1) defaultMenu();    // if button has been pressed return to default menu
        break;
    }
} //menuUpdate


// ----------------------------------------------------------------
//                       -service active menu
// ----------------------------------------------------------------

void serviceMenu() {

    // rotary encoder
      if (rotaryEncoder.encoder0Pos >= itemTrigger) {
        rotaryEncoder.encoder0Pos -= itemTrigger;
        oledMenu.highlightedMenuItem++;
        oledMenu.lastMenuActivity = millis();   // log time
      }
      if (rotaryEncoder.encoder0Pos <= -itemTrigger) {
        rotaryEncoder.encoder0Pos += itemTrigger;
        oledMenu.highlightedMenuItem--;
        oledMenu.lastMenuActivity = millis();   // log time
      }
      if (rotaryEncoder.reButtonPressed == 1) {
        oledMenu.selectedMenuItem = oledMenu.highlightedMenuItem;     // flag that the item has been selected
        oledMenu.lastMenuActivity = millis();   // log time
        if (serialDebug) Serial.println("menu '" + oledMenu.menuTitle + "' item '" + oledMenu.menuItems[oledMenu.highlightedMenuItem] + "' selected");
      }

    const int _centreLine = displayMaxLines / 2 + 1;    // mid list point
    display.clearDisplay();
    display.setTextColor(WHITE);

    // verify valid highlighted item
      if (oledMenu.highlightedMenuItem > oledMenu.noOfmenuItems) oledMenu.highlightedMenuItem = oledMenu.noOfmenuItems;
      if (oledMenu.highlightedMenuItem < 1) oledMenu.highlightedMenuItem = 1;

    // title
      display.setCursor(0, 0);
      if (menuLargeText) {
        display.setTextSize(2);
        display.println(oledMenu.menuItems[oledMenu.highlightedMenuItem].substring(0, MaxmenuTitleLength));
      } else {
        if (oledMenu.menuTitle.length() > MaxmenuTitleLength) display.setTextSize(1);
        else display.setTextSize(2);
        display.println(oledMenu.menuTitle);
      }
      display.drawLine(0, topLine-1, display.width(), topLine-1, WHITE);       // draw horizontal line under title

    // menu
      display.setTextSize(1);
      display.setCursor(0, topLine);
      for (int i=1; i <= displayMaxLines; i++) {
        int item = oledMenu.highlightedMenuItem - _centreLine + i;
        if (item == oledMenu.highlightedMenuItem) display.setTextColor(BLACK, WHITE);
        else display.setTextColor(WHITE);
        if (item > 0 && item <= oledMenu.noOfmenuItems) display.println(oledMenu.menuItems[item]);
        else display.println(" ");
      }

    //// how to display some updating info. on the menu screen
    // display.setCursor(80, 25);
    // display.println(millis());
 
    display.display();
} //serviceMenu


// ----------------------------------------------------------------
//                        -service value entry
// ----------------------------------------------------------------
// if _blocking set to 1 then all other tasks are stopped until a value is entered

int serviceValue(bool _blocking) {

  const int _valueSpacingX = 30;      // spacing for the displayed value y position
  const int _valueSpacingY = 5;       // spacing for the displayed value y position

  if (_blocking) {
    menuMode = blocking;
    oledMenu.lastMenuActivity = millis();   // log time of last activity (for timeout)
  }
  uint32_t tTime;

  do {

    // rotary encoder
      if (rotaryEncoder.encoder0Pos >= itemTrigger) {
        rotaryEncoder.encoder0Pos -= itemTrigger;
        oledMenu.mValueEntered-= oledMenu.mValueStep;
        oledMenu.lastMenuActivity = millis();   // log time
      }
      if (rotaryEncoder.encoder0Pos <= -itemTrigger) {
        rotaryEncoder.encoder0Pos += itemTrigger;
        oledMenu.mValueEntered+= oledMenu.mValueStep;
        oledMenu.lastMenuActivity = millis();   // log time
      }
      if (oledMenu.mValueEntered < oledMenu.mValueLow) {
        oledMenu.mValueEntered = oledMenu.mValueLow;
        oledMenu.lastMenuActivity = millis();   // log time
      }
      if (oledMenu.mValueEntered > oledMenu.mValueHigh) {
        oledMenu.mValueEntered = oledMenu.mValueHigh;
        oledMenu.lastMenuActivity = millis();   // log time
      }

      display.clearDisplay();
      display.setTextColor(WHITE);

      // title
        display.setCursor(0, 0);
        if (oledMenu.menuTitle.length() > MaxmenuTitleLength) display.setTextSize(1);
        else display.setTextSize(2);
        display.println(oledMenu.menuTitle);
        display.drawLine(0, topLine-1, display.width(), topLine-1, WHITE);       // draw horizontal line under title

      // value selected
        display.setCursor(_valueSpacingX, topLine + _valueSpacingY);
        display.setTextSize(3);
        display.println(oledMenu.mValueEntered);

      // range
        display.setCursor(0, display.height() - lineSpace1 - 1 );   // bottom of display
        display.setTextSize(1);
        display.println(String(oledMenu.mValueLow) + " to " + String(oledMenu.mValueHigh));

      // bar
        int Tlinelength = map(oledMenu.mValueEntered, oledMenu.mValueLow, oledMenu.mValueHigh, 0 , display.width());
        display.drawLine(0, display.height()-1, Tlinelength, display.height()-1, WHITE);

      display.display();

      reUpdateButton();        // check status of button
      tTime = (unsigned long)(millis() - oledMenu.lastMenuActivity);      // time since last activity

  } while (_blocking && rotaryEncoder.reButtonPressed == 0 && tTime < (menuTimeout * 1000));        // if in blocking mode repeat until button is pressed or timeout

  if (_blocking) menuMode = off;

  return oledMenu.mValueEntered;        // used when in blocking mode
}


// ----------------------------------------------------------------
//                           -list create
// ----------------------------------------------------------------
// create a menu from a list
// e.g.       String tList[]={"main menu", "2", "3", "4", "5", "6"};
//            createList("demo_list", 6, &tList[0]);

void createList(String _title, int _noOfElements, String *_list) {
  resetMenu();                      // clear any previous menu
  menuMode = menu;                  // enable menu mode
  oledMenu.noOfmenuItems = _noOfElements;    // set the number of items in this menu
  oledMenu.menuTitle = _title;               // menus title (used to identify it)

  for (int i=1; i <= _noOfElements; i++) {
    oledMenu.menuItems[i] = _list[i-1];        // set the menu items
  }
}


// ----------------------------------------------------------------
//                         -message display
// ----------------------------------------------------------------
// 21 characters per line, use "\n" for next line
// assistant:  <     line 1        ><     line 2        ><     line 3        ><     line 4         >

 void displayMessage(String _title, String _message) {
  resetMenu();
  menuMode = message;

  display.clearDisplay();
  display.setTextColor(WHITE);

  // title
    display.setCursor(0, 0);
    if (menuLargeText) {
      display.setTextSize(2);
      display.println(_title.substring(0, MaxmenuTitleLength));
    } else {
      if (_title.length() > MaxmenuTitleLength) display.setTextSize(1);
      else display.setTextSize(2);
      display.println(_title);
    }

  // message
    display.setCursor(0, topLine);
    display.setTextSize(1);
    display.println(_message);

  display.display();

 }


// ----------------------------------------------------------------
//                        -reset menu system
// ----------------------------------------------------------------

void resetMenu() {
  // reset all menu variables / flags
    menuMode = off;
    oledMenu.selectedMenuItem = 0;
    rotaryEncoder.encoder0Pos = 0;
    oledMenu.noOfmenuItems = 0;
    oledMenu.menuTitle = "";
    oledMenu.highlightedMenuItem = 0;
    oledMenu.mValueEntered = 0;
    rotaryEncoder.reButtonPressed = 0;

  oledMenu.lastMenuActivity = millis();   // log time

  // clear oled display
    display.clearDisplay();
    display.display();
}


// ----------------------------------------------------------------
//                     -interrupt for rotary encoder
// ----------------------------------------------------------------
// rotary encoder interrupt routine to update position counter when turned
//     interrupt info: https://www.gammon.com.au/forum/bbshowpost.php?id=11488

void doEncoder() {

  bool pinA = digitalRead(encoder0PinA);
  bool pinB = digitalRead(encoder0PinB);

  if ( (rotaryEncoder.encoderPrevA == pinA && rotaryEncoder.encoderPrevB == pinB) ) return;  // no change since last time (i.e. reject bounce)

  // same direction (alternating between 0,1 and 1,0 in one direction or 1,1 and 0,0 in the other direction)
         if (rotaryEncoder.encoderPrevA == 1 && rotaryEncoder.encoderPrevB == 0 && pinA == 0 && pinB == 1) rotaryEncoder.encoder0Pos -= 1;
    else if (rotaryEncoder.encoderPrevA == 0 && rotaryEncoder.encoderPrevB == 1 && pinA == 1 && pinB == 0) rotaryEncoder.encoder0Pos -= 1;
    else if (rotaryEncoder.encoderPrevA == 0 && rotaryEncoder.encoderPrevB == 0 && pinA == 1 && pinB == 1) rotaryEncoder.encoder0Pos += 1;
    else if (rotaryEncoder.encoderPrevA == 1 && rotaryEncoder.encoderPrevB == 1 && pinA == 0 && pinB == 0) rotaryEncoder.encoder0Pos += 1;

  // change of direction
    else if (rotaryEncoder.encoderPrevA == 1 && rotaryEncoder.encoderPrevB == 0 && pinA == 0 && pinB == 0) rotaryEncoder.encoder0Pos += 1;
    else if (rotaryEncoder.encoderPrevA == 0 && rotaryEncoder.encoderPrevB == 1 && pinA == 1 && pinB == 1) rotaryEncoder.encoder0Pos += 1;
    else if (rotaryEncoder.encoderPrevA == 0 && rotaryEncoder.encoderPrevB == 0 && pinA == 1 && pinB == 0) rotaryEncoder.encoder0Pos -= 1;
    else if (rotaryEncoder.encoderPrevA == 1 && rotaryEncoder.encoderPrevB == 1 && pinA == 0 && pinB == 1) rotaryEncoder.encoder0Pos -= 1;

  //else if (serialDebug) Serial.println("Error: invalid rotary encoder pin state - prev=" + String(rotaryEncoder.encoderPrevA) + ","
  //                                      + String(rotaryEncoder.encoderPrevB) + " new=" + String(pinA) + "," + String(pinB));

  // update previous readings
    rotaryEncoder.encoderPrevA = pinA;
    rotaryEncoder.encoderPrevB = pinB;
}


// ---------------------------------------------- end ----------------------------------------------
