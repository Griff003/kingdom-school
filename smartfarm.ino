#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <DHT.h>

// ==== WiFi AP Mode ====
const char* ssid = "SmartFarm";
const char* password = "";

ESP8266WebServer server(80);

// ==== Pin Definitions ====
#define MOISTURE_PIN A0
#define PUMP_PIN D4
#define LDR1_PIN D1
#define LDR2_PIN D2
#define SERVO_PIN D8
#define DHTPIN D5
#define DHTTYPE DHT11

Servo servo;
DHT dht(DHTPIN, DHTTYPE);

int ldr1Val, ldr2Val;
int moistureRaw;
int moisturePercent;
float temperature, humidity;

String language = "en"; // default language

String translate(String key) {
  if (language == "sw") {
    if (key == "title") return "Shamba ya Kisasa ya Kingdom";
    if (key == "sensor_readings") return "Soma za Vihisi";
    if (key == "soil_moisture") return "Unyevu wa Udongo";
    if (key == "temperature") return "Joto";
    if (key == "humidity") return "Unyevu hewani";
    if (key == "pump_status") return "Hali ya Pampu";
    if (key == "on") return "WASH";
    if (key == "off") return "ZIMWA";
    if (key == "tips") return "Vidokezo vya Mkoa";
    if (key == "food_security") return "Habari ya Usalama wa Chakula";
    if (key == "food_update") return "Kenya inakumbwa na upungufu wa chakula kutokana na mvua zisizoeleweka, uhifadhi duni na uharibifu wa ardhi.";
    if (key == "tip_text") return "🔹 Magharibi mwa Kenya: Panda mahindi mwezi wa Machi na mtama mwezi wa Oktoba.<br>🔹 Pwani ya Kenya: Inafaa kwa muhogo na nazi kati ya Aprili na Juni.<br>🔹 Kenya ya Kati: Wakati bora wa viazi na kahawa ni kati ya Machi na Julai.<br>🔹 Bonde la Ufa: Ngano na shayiri hustawi kuanzia Mei hadi Septemba.";
  } else if (language == "luo") {
    if (key == "title") return "Shamba Mariek Kingdom";
    if (key == "sensor_readings") return "Neno Mar Sensor";
    if (key == "soil_moisture") return "Unyiego mar Piny";
    if (key == "temperature") return "Temperatur";
    if (key == "humidity") return "Unyiego mar yien";
    if (key == "pump_status") return "Tije mar Pump";
    if (key == "on") return "OTING'O";
    if (key == "off") return "OTEL";
    if (key == "tips") return "Tije mag Region";
    if (key == "food_security") return "Neno mar Chakruok";
    if (key == "food_update") return "Kenya ni gi puonj mar chakruok nikech koth ma ok olo, ketho piny gi tich mar chal.";
    if (key == "tip_text") return "🔹 Kenya Maler: Pand maize e March kod sorghum e October.<br>🔹 Kenya Mar nam: Oketo cassava gi coconut e April okanyo June.<br>🔹 Central Kenya: Potatoes gi coffee gin maber e March okanyo July.<br>🔹 Rift Valley: Wheat gi barley mitie e May okanyo September.";
  } else if (language == "kikuyu") {
    if (key == "title") return "Shamba ria Kingdom ria Gûtukû";
    if (key == "sensor_readings") return "Maûndû ma Sensor";
    if (key == "soil_moisture") return "Maî mûgũnda-inî";
    if (key == "temperature") return "Ûhoro wa mûthenya";
    if (key == "humidity") return "Ûhoro wa mathagu";
    if (key == "pump_status") return "Mwena wa Pampo";
    if (key == "on") return "IRÎE";
    if (key == "off") return "GÛCOKIA";
    if (key == "tips") return "Mwarîria wa Gîthomo";
    if (key == "food_security") return "Ûhoro wa Mûno";
    if (key == "food_update") return "Kenya ĩkûrîa na ûhoro wa mûno nîkûgûria kûgûa kwa mvura na kûnyita gûtogoria wa gûtwara.";
    if (key == "tip_text") return "🔹 Western Kenya: Tumie mathayû mûno March na sorghum October.<br>🔹 Coastal Kenya: Itirikanî cassava na coconut thîna wa April na June.<br>🔹 Central Kenya: Gûtua mbembe na kahawa March na July.<br>🔹 Rift Valley: Witia mbembe ya ngano na barley May to September.";
  } else if (language == "kalenjin") {
    if (key == "title") return "Farmab Kingdom ne Kiptai";
    if (key == "sensor_readings") return "Mechyek che Sensor";
    if (key == "soil_moisture") return "Muget Ne Kaptich";
    if (key == "temperature") return "Kipkelat";
    if (key == "humidity") return "Muget ne leel";
    if (key == "pump_status") return "Ngotutik Pump";
    if (key == "on") return "TUN";
    if (key == "off") return "CHEK";
    if (key == "tips") return "Chitoitab Region";
    if (key == "food_security") return "Tililap Komek";
    if (key == "food_update") return "Kenya ko amwae ngotutik che kiboisio konget ak storotet nebo chi ne muto.";
    if (key == "tip_text") return "🔹 Western Kenya: Kaset maize March ak sorghum October.<br>🔹 Coastal Kenya: Kapse cassava ak coconut April to June.<br>🔹 Central Kenya: Potatoes ak coffee March to July.<br>🔹 Rift Valley: Wheat ak barley May to September.";
  }
  if (key == "tip_text") return "🔹 Western Kenya: Plant maize in March and sorghum in October.<br>🔹 Coastal Kenya: Ideal for cassava and coconut between April and June.<br>🔹 Central Kenya: Best time for potatoes and coffee is between March and July.<br>🔹 Rift Valley: Wheat and barley thrive from May to September.";
  if (key == "food_update") return "Kenya is facing food shortages due to erratic rains, poor storage, and land degradation.";
  return key;
}

void handleRoot() {
  if (server.hasArg("lang")) language = server.arg("lang");

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<title>" + translate("title") + "</title>";
  html += "<style>body{font-family:Arial;text-align:center;background:#eef;padding:20px;}";
  html += "h1{color:#2E8B57;} .box{background:white;padding:20px;margin:10px auto;border-radius:10px;max-width:800px;box-shadow:0 0 10px #ccc;}";
  html += ".btn{padding:10px 20px;background:#2E8B57;color:white;border:none;border-radius:5px;cursor:pointer;}";
  html += ".chart{height:200px;} select{margin-top:10px;}";
  html += "</style><script src='https://cdn.jsdelivr.net/npm/chart.js'></script></head><body>";
  html += "<h1>🌱 " + translate("title") + "</h1>";

  html += "<form><label>Language:</label> <select name='lang' onchange='this.form.submit()'>";
  html += String("<option value='en'") + (language=="en"?" selected":"") + ">English</option>";
  html += String("<option value='sw'") + (language=="sw"?" selected":"") + ">Kiswahili</option>";
  html += String("<option value='luo'") + (language=="luo"?" selected":"") + ">Luo</option>";
  html += String("<option value='kikuyu'") + (language=="kikuyu"?" selected":"") + ">Kikuyu</option>";
  html += String("<option value='kalenjin'") + (language=="kalenjin"?" selected":"") + ">Kalenjin</option></select></form>";

  html += "<div class='box'><h2>" + translate("sensor_readings") + "</h2>";
  html += "<p><b>" + translate("soil_moisture") + ":</b> " + String(moisturePercent) + " %</p>";
  html += "<p><b>" + translate("temperature") + ":</b> " + String(temperature) + " °C</p>";
  html += "<p><b>" + translate("humidity") + ":</b> " + String(humidity) + " %</p>";
  html += "<p><b>" + translate("pump_status") + ":</b> " + String(digitalRead(PUMP_PIN)==LOW ? translate("off") : translate("on")) + "</p>";
  html += "<button onclick='location.reload()' class='btn'>🔄 Refresh</button></div>";

  html += "<div class='box'><h2>🌍 " + translate("tips") + "</h2><p style='text-align:left;'>" + translate("tip_text") + "</p></div>";

  html += "<div class='box'><h2>⚠️ " + translate("food_security") + "</h2><p>" + translate("food_update") + "</p></div></body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT); digitalWrite(PUMP_PIN, HIGH);
  pinMode(LDR1_PIN, INPUT); pinMode(LDR2_PIN, INPUT);
  servo.attach(SERVO_PIN); servo.write(90);
  dht.begin();

  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("SmartFarm running at 192.168.4.1");
}

void loop() {
  moistureRaw = analogRead(MOISTURE_PIN);
  moisturePercent = map(moistureRaw, 1024, 300, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  digitalWrite(PUMP_PIN, moisturePercent < 20 ? HIGH : (moisturePercent > 60 ? LOW : digitalRead(PUMP_PIN)));

  ldr1Val = digitalRead(LDR1_PIN);
  ldr2Val = digitalRead(LDR2_PIN);
  servo.write(ldr1Val==HIGH && ldr2Val==LOW ? 60 : (ldr2Val==HIGH && ldr1Val==LOW ? 120 : 90));

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) { temperature = 0; humidity = 0; }

  server.handleClient();
  delay(2000);
}
