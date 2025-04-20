#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define D2 5  //Bhai D2 ki pin SCL se connect ker.
#define D1 4  //Bhai D1 ki pin SDL se connect ker.
//<---------------------------------buttons------------------------------------->
#define BUTTON_UP 14    // D5
#define BUTTON_DOWN 12  // D6
#define BUTTON_A 13     // D7
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define ROBOT_IP "192.168.43.69"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient client;

const char* ssid = "Robotic_hand";
const char* password = "12345678";

//<-----------------Change this to your Network IP------------------------>
IPAddress local_IP(192, 168, 43, 70);
IPAddress gateway(192, 168, 43, 1);       
IPAddress subnet(255, 255, 255, 0); 

//<---------------Bit MAP image------------------>

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
      (SCREEN_WIDTH - 16) / 2,
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

void test() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("Testing...");
  LoadingBar(60);
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
void sendCommand(const char* funcName) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" ROBOT_IP "/Preset?func=" + String(funcName);
    http.begin(client,url);

    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.println("GET request failed. Code: " + String(httpCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void highfive() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("Dancing...");
  LoadingBar(20);
  display.display();
  sendCommand("HighFive");
  delay(500);
}

void Do180() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("Doing 180");
  LoadingBar(20);
  display.display();
  sendCommand("Do180");
  delay(500);
}

void pickup() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("Picking up");
  LoadingBar(20);
  display.display();
  sendCommand("PickUp");
  delay(500);
}

void Dance() {
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - 60) / 2, (SCREEN_HEIGHT + 22) / 2 - 20);
  display.println("Dancing...");
  LoadingBar(20);
  display.display();
  sendCommand("Dance");
  delay(500);
}

//Checks connection to the Robotic Hand
void Link() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" ROBOT_IP "/ping";
    http.begin(client,url);

    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response: " + payload);

      if (payload == "PONG") {
        // Show "Connected" on the OLED
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(5, 20);
        display.println("PONG");
        display.display();
      }
    } else {
      Serial.println("Ping request failed. Code: " + String(httpCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}


int num_item = 4;  //<-----------Also update this variable if you updatethe manu_name

char manu_name[4][20] = {
  { "Preset Motions" },
  { "Ping" },
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

  Serial.begin(115200);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  // Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }


  display.clearDisplay();
  display.setCursor(20, 32);
  display.println(WiFi.localIP());
  display.display();
  delay(3000);
}

void loop() {
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
          if (preset_item_selected == 4) delay(300);
          break;
          preset_lastButtonPress = millis();
        }
      }
    }

    if (item_selected == 1) {
      Link();
    }
    if (item_selected == 2) {
      testServoandConnection();
    }
    if (item_selected == 3) {
      showAbout();
    }
  }
}
