/*
  ------------------------------------------------------
  Project Title   : ESP8266 OLED screen based Robotic Arm
  Version         : 1.2
  Author          : Mohd Amir
  Contributor     : Sunil Kumar
  Project Type    : Major Project
  ------------------------------------------------------

  Description:
  This project implements an interactive menu system using an ESP8266
  microcontroller with a 0.96" I2C OLED display. It features a smooth
  UI experience with bitmap icons, animated startup screen, and
  navigation using active-low push buttons.

  Designed as a versatile interface console, this system can be easily
  adapted for various applications like motion presets, testing routines,
  and device linking. Clean aesthetics and logical flow ensure both
  usability and expandability for embedded solutions.

  Developed with passion and precision to serve as a solid foundation
  for real-world IoT and embedded menu-driven applications.
*/


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

#define D2 5  //Bhai D2 ki pin SCL se connect ker.
#define D1 4  //Bhai D1 ki pin SDL se connect ker.
//<---------------------------------buttons------------------------------------->
#define BUTTON_UP 14    // D5
#define BUTTON_DOWN 12  // D6
#define BUTTON_A 13     // D7
//<-----------------------------Servo Pins-------------------------------->
const int pinX = 0;  //D3
const int pinY = 1;  //D10
const int pinZ = 2;  //D4
const int pinG = 3;  //D9

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//<-----------------------Wifi credential and port-------------------------->
const char* ssid = "Robotic_hand";
const char* password = "12345678";
ESP8266WebServer server(80);

bool awaitingPing = false;
unsigned long pingStartTime = 0;
bool linkConfirmed = false;
bool stopreverse180 = false;

Servo servoX, servoY, servoZ, servoG;

// 'Outline-select', 128x21px
const unsigned char epd_bitmap_Outline_select[] PROGMEM = {
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
  0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00,
  0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00
};
// 'RoboticHandicon', 16x16px
const unsigned char epd_bitmap_RoboticHandicon[] PROGMEM = {
  0x00, 0x3e, 0x00, 0x41, 0x01, 0x80, 0x03, 0x80, 0x07, 0x41, 0x1e, 0x3e, 0x3c, 0x00, 0x3c, 0x00,
  0x3e, 0x00, 0x1f, 0x00, 0x03, 0x80, 0x01, 0xc0, 0x00, 0xe0, 0x00, 0x70, 0x07, 0xfe, 0x0f, 0xff
};
// 'TestIcon', 16x16px
const unsigned char epd_bitmap_TestIcon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x07, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x38,
  0x40, 0x70, 0xe0, 0xe0, 0x71, 0xc0, 0x3b, 0x80, 0x1f, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'Checklink', 16x16px
const unsigned char epd_bitmap_Checklink[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x10, 0x08, 0x20, 0x04, 0x4f, 0xf2, 0x10, 0x08,
  0x23, 0xc4, 0x04, 0x20, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_About[] PROGMEM = {
  0x00, 0x00, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0,
  0x01, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 496)
const int epd_bitmap_allArray_LEN = 5;
const unsigned char* epd_bitmap_allArray[5] = {
  epd_bitmap_RoboticHandicon,
  epd_bitmap_Checklink,
  epd_bitmap_TestIcon,
  epd_bitmap_About,
  epd_bitmap_Outline_select
};


void startAnimation() {
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2);
  display.println("Starting...");
  display.display();
  delay(1000);
  display.clearDisplay();
  for (int i = 0; i < 3; i++) {
    display.clearDisplay();
    display.drawBitmap(
      (SCREEN_WIDTH - 16) / 2,  // center 16x16 bitmap
      (SCREEN_HEIGHT - 16) / 2,
      epd_bitmap_allArray[i],
      16, 16,
      SSD1306_WHITE);
    display.display();
    delay(800);
  }

  delay(300);
  display.clearDisplay();
}

void LoadingBar(int timeDelay) {
  int barWidth = 80;
  int barHeight = 10;
  int barX = (SCREEN_WIDTH - barWidth) / 2;
  int barY = 35;

  display.drawRect(barX, barY, barWidth, barHeight, WHITE);
  display.display();


  for (int i = 0; i <= 100; i += 2) {
    int fillWidth = (i * (barWidth - 2)) / 100;
    display.fillRect(barX + 1, barY + 1, fillWidth, barHeight - 2, WHITE);


    //presentage number bhai
    display.fillRect((SCREEN_WIDTH - 20) / 2, barY + 15, 30, 10, BLACK);
    display.setCursor((SCREEN_WIDTH - 20) / 2, barY + 15);
    display.setTextColor(WHITE);
    display.printf("%d%%", i);


    display.display();
    delay(timeDelay);
  }
}

void showAbout() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);


  display.setCursor((128 - 96) / 2, 2);
  display.println("Robotic Arm ");


  display.setCursor((128 - 96) / 2, 20);
  display.println("By Amir & Sunil");


  display.setCursor((128 - 94) / 2, 38);
  display.println("Guided by Anuj sir");

  display.display();

  delay(3000);
}




void highfive() {
  display.clearDisplay();

  // Show text message
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("High Five...");


  LoadingBar(90);
}


void test() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("Testing...");
  LoadingBar(60);
  display.display();
  delay(1000);
}

void pickup() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("picking up...");
  LoadingBar(60);
  display.display();
  delay(1000);
}

void Dance() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("Danceing...");
  display.display();
  delay(1000);
}

void testServoandConnection() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("Please wait..");
  LoadingBar(60);
  display.display();
  delay(1000);
}

void checkWebUILink() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println("Checking Web UI...");
  display.println("Please click TEST on UI.");
  display.display();

  awaitingPing = true;
  linkConfirmed = false;
  delay(4000);
  pingStartTime = millis();
}

// ek function jo 180 degree motion kerwayga base ka
void Do180() {
  display.clearDisplay();
  LoadingBar(20);
  display.display();
  if (stopreverse180) {
    for (int i = 0; i >= 0; i--) {
      servoG.write(i);
      delay(15);
    }
  }
  for (int i = 0; i < 180; i++) {
    servoG.write(i);
    delay(15);
  }

  stopreverse180 = true;
}


//<----------web interface Get request handler---------------->

//Cors policy ko ignor kerne ke liye
void sendWithCORS(int code, String contentType, String message) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.send(code, contentType, message);
}


void handleServoPos() {
  if (server.hasArg("x") && server.hasArg("y") && server.hasArg("z") && server.hasArg("g")) {
    int x = server.arg("x").toInt();
    int y = server.arg("y").toInt();
    int z = server.arg("z").toInt();
    int g = server.arg("g").toInt();

    // Clamp values to 0–180 (servo safe range)
    x = constrain(x, 0, 180);
    y = constrain(y, 0, 180);
    z = constrain(z, 0, 180);
    g = constrain(g, 0, 180);

    // Move servos
    servoX.write(x);
    servoY.write(y);
    servoZ.write(z);
    servoG.write(g);

    // Serial.println(x);
    // Serial.println(y);
    // Serial.println(z);
    // Serial.println(g);

    sendWithCORS(200, "text/plain", "ok");
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}



//<-------------Handle Preset motions------------------>
void Preset() {
  if (server.hasArg("func")) {
    String functionToDo = server.arg("func");
    functionToDo.trim();
    // Serial.print("func received: ");
    // Serial.println(server.arg("func"));

    if (functionToDo.equals("Do180")) {
      Do180();
      sendWithCORS(200, "text/plain", "Function Do180 executed successfully");
    } else if (functionToDo.equals("Dance")) {
      Dance();
      sendWithCORS(200, "text/plain", "Function Dance executed successfully");

    } else {
      sendWithCORS(503, "text/plain", "Not Defined !");
    }
  } else {
    sendWithCORS(400, "text/plain", "Missing Perameters");
  }
}


int num_item = 4;  //<-----------Also update this variable if you updatethe manu_name

char manu_name[4][20] = {
  { "Preset Motions" },
  { "Link" },
  { "Test" },
  { "About" }
};

char preset_motions_manu[5][20] = {
  { "High Five" },
  { "Do 180" },
  { "Pick up" },
  { "Dance" },
  { "Main Manu" }
};

int item_selected = 0;
int next_item;
int prev_item;

//<-----------Preventing Fast scrolling change it if the manu jumps multiple times ----------------->
unsigned long lastButtonPress = 0;
const int debounceDelay = 200;

void setup() {
  //<---Priortiy for servo-------->
  servoX.attach(pinX, 500, 2400);
  servoY.attach(pinY, 500, 2400);
  servoZ.attach(pinZ, 500, 2400);
  servoG.attach(pinG, 500, 2400);

  Wire.begin(D2, D1);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  delay(500);
  startAnimation();
  //<------------ Text size and Color---------------------->
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);  //Pull Uped by default so you have to connect the Pins to GND in order to activate it
  pinMode(BUTTON_A, INPUT_PULLUP);

  //<---------------Servo Axis--------------->


  //<-------------------Server setup------------------------->
  // Serial.begin(115200);



  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  // Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }

  // Serial.println("");
  // Serial.println("Connected to WiFi");
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
  
  display.clearDisplay();
  display.setCursor(20, 32);
  display.println(WiFi.localIP());
  display.display();
  delay(3000);

  server.on("/servoPos", HTTP_GET, handleServoPos);
  server.on("/Preset", HTTP_GET, Preset);

  server.on("/ping", []() {
    if (awaitingPing) {
      linkConfirmed = true;
      awaitingPing = false;
    }
    sendWithCORS(200, "text/plain", "PONG");
  });


  server.begin();
  // Serial.println("Server started");
}

void loop() {
  server.handleClient();

  //<---------To test the Link between the ESP8266 and the WEB UI---------------->

  if (linkConfirmed) {
    display.clearDisplay();
    display.setCursor(30, 25);
    display.println("Connected!");
    display.display();
    delay(4000);
    linkConfirmed = false;
  }



  if (digitalRead(BUTTON_UP) == LOW && millis() - lastButtonPress > debounceDelay) {
    item_selected -= 1;
    if (item_selected < 0) item_selected = num_item - 1;
    lastButtonPress = millis();
  }

  if (digitalRead(BUTTON_DOWN) == LOW && millis() - lastButtonPress > debounceDelay) {
    item_selected += 1;
    if (item_selected >= num_item) item_selected = 0;
    lastButtonPress = millis();
  }

  prev_item = item_selected - 1;
  //preventing overflow of list items

  if (prev_item < 0) {
    prev_item = num_item - 1;
  }

  next_item = item_selected + 1;

  if (next_item >= num_item) {
    next_item = 0;
  }


  display.clearDisplay();
  display.setCursor(26, 8);
  display.println(manu_name[prev_item]);
  display.setCursor(26, 29);
  display.println(manu_name[item_selected]);
  display.setCursor(26, 50);
  display.println(manu_name[next_item]);

  display.drawBitmap(4, 2, epd_bitmap_allArray[prev_item], 16, 16, SSD1306_WHITE);

  display.drawBitmap(4, 24, epd_bitmap_allArray[item_selected], 16, 16, SSD1306_WHITE);

  display.drawBitmap(4, 46, epd_bitmap_allArray[next_item], 16, 16, SSD1306_WHITE);

  display.drawBitmap(0, 22, epd_bitmap_allArray[4], 128, 21, SSD1306_WHITE);
  display.display();


  if (digitalRead(BUTTON_A) == LOW && millis() - lastButtonPress > debounceDelay) {
    while (digitalRead(BUTTON_A) == LOW)
      ;
    delay(200);
    lastButtonPress = millis();

    if (item_selected == 0) {
      int preset_num_item = 5;
      int preset_item_selected = 0;
      int preset_prev_item;
      int preset_next_item;
      unsigned long preset_lastButtonPress = 0;
      const int preset_debounceDelay = 200;
      while (true) {
        if (digitalRead(BUTTON_DOWN) == LOW && millis() - preset_lastButtonPress > preset_debounceDelay) {
          preset_item_selected++;
          if (preset_item_selected >= preset_num_item) preset_item_selected = 0;
          preset_lastButtonPress = millis();
        }
        if (digitalRead(BUTTON_UP) == LOW && millis() - preset_lastButtonPress > preset_debounceDelay) {
          preset_item_selected--;
          if (preset_item_selected < 0) preset_item_selected = preset_num_item - 1;
          preset_lastButtonPress = millis();
        }

        preset_prev_item = preset_item_selected - 1;

        if (preset_prev_item < 0) preset_prev_item = preset_num_item - 1;

        preset_next_item = preset_item_selected + 1;

        if (preset_next_item >= preset_num_item) preset_next_item = 0;

        display.clearDisplay();
        display.setCursor(26, 8);
        display.println(preset_motions_manu[preset_prev_item]);
        display.setCursor(26, 29);
        display.println(preset_motions_manu[preset_item_selected]);
        display.setCursor(26, 50);
        display.println(preset_motions_manu[preset_next_item]);

        display.drawBitmap(4, 2, epd_bitmap_allArray[0], 16, 16, SSD1306_WHITE);

        display.drawBitmap(4, 24, epd_bitmap_allArray[0], 16, 16, SSD1306_WHITE);

        display.drawBitmap(4, 46, epd_bitmap_allArray[0], 16, 16, SSD1306_WHITE);

        display.drawBitmap(0, 22, epd_bitmap_allArray[4], 128, 21, SSD1306_WHITE);

        display.display();

        //<-------------Calling Each function according to the Loock up table ----------------->

        if (digitalRead(BUTTON_A) == LOW && millis() - preset_lastButtonPress > preset_debounceDelay) {
          if (preset_item_selected == 0) highfive();
          if (preset_item_selected == 1) Do180();
          if (preset_item_selected == 2) pickup();
          if (preset_item_selected == 3) Dance();
          if (preset_item_selected == 4) delay(300); break;
          preset_lastButtonPress = millis();
        }
      }
    }

    if (item_selected == 1) {
      checkWebUILink();
    }
    if (item_selected == 2) {
      testServoandConnection();
    }
    if (item_selected == 3) {
      showAbout();
    }
  }
}
