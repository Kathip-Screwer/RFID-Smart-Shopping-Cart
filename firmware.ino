#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WebServer.h>

#define SS_PIN 5
#define RST_PIN 22
#define BUZZER 15

const char* ssid = "SmartCart";
const char* password = "12345678";

WebServer server(80);

MFRC522 rfid(SS_PIN, RST_PIN);

int total = 0;
String cartItems = "";   // 🔥 store items

// Item states
bool milkAdded = false;
bool breadAdded = false;
bool biscuitAdded = false;
bool juiceAdded = false;

// 🔊 Beep functions
void beepOnce() {
  tone(BUZZER, 1000);
  delay(150);
  noTone(BUZZER);
}

void beepTwice() {
  for (int i = 0; i < 2; i++) {
    tone(BUZZER, 1000);
    delay(150);
    noTone(BUZZER);
    delay(150);
  }
}

// 🌐 Web Page
void handleRoot() {
  String page = "<html><head>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += "<meta http-equiv='refresh' content='2'>";  // auto refresh
  page += "<style>";
  page += "body{font-family:Arial;text-align:center;background:#f2f2f2;}";
  page += "h1{color:#333;}";
  page += ".box{background:white;padding:20px;margin:20px;border-radius:10px;box-shadow:0 0 10px #ccc;}";
  page += "</style></head><body>";

  page += "<h1>🛒 Smart Cart</h1>";
  page += "<div class='box'>";
  page += "<h2>Items</h2>";
  page += cartItems;
  page += "<hr>";
  page += "<h2>Total: &#8377;" + String(total) + "</h2>";
  page += "</div>";

  page += "</body></html>";

  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);
  SPI.begin(18, 19, 23, 5);
  rfid.PCD_Init();

  pinMode(BUZZER, OUTPUT);

  Serial.println("Smart Cart Ready...");

  WiFi.softAP(ssid, password);
  Serial.println("WiFi Started");

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {

    String uid = "";

    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uid += "0";
      uid += String(rfid.uid.uidByte[i], HEX);
    }

    uid.toUpperCase();

    Serial.print("Scanned UID: ");
    Serial.println(uid);

    if (uid == "82D5F906") {
      if (!milkAdded) {
        total += 30;
        milkAdded = true;
        cartItems += "<p>Milk - &#8377;30</p>";
        Serial.println("Milk Added");
        beepOnce();
      } else {
        total -= 30;
        milkAdded = false;
        cartItems.replace("<p>Milk - &#8377;30</p>", "");
        Serial.println("Milk Removed");
        beepTwice();
      }
    }

    else if (uid == "99A9F506") {
      if (!breadAdded) {
        total += 25;
        breadAdded = true;
        cartItems += "<p>Bread - &#8377;25</p>";
        Serial.println("Bread Added");
        beepOnce();
      } else {
        total -= 25;
        breadAdded = false;
        cartItems.replace("<p>Bread - &#8377;25</p>", "");
        Serial.println("Bread Removed");
        beepTwice();
      }
    }

    else if (uid == "8694F804") {
      if (!biscuitAdded) {
        total += 20;
        biscuitAdded = true;
        cartItems += "<p>Biscuit - &#8377;20</p>";
        Serial.println("Biscuit Added");
        beepOnce();
      } else {
        total -= 20;
        biscuitAdded = false;
        cartItems.replace("<p>Biscuit - &#8377;20</p>", "");
        Serial.println("Biscuit Removed");
        beepTwice();
      }
    }

    else if (uid == "9EA90902") {
      if (!juiceAdded) {
        total += 40;
        juiceAdded = true;
        cartItems += "<p>Juice - &#8377;40</p>";
        Serial.println("Juice Added");
        beepOnce();
      } else {
        total -= 40;
        juiceAdded = false;
        cartItems.replace("<p>Juice - &#8377;40</p>", "");
        Serial.println("Juice Removed");
        beepTwice();
      }
    }

    else {
      Serial.println("Unknown Item");
    }

    Serial.print("Total: ₹");
    Serial.println(total);
    Serial.println("------------------");

    delay(500);
  }
}
