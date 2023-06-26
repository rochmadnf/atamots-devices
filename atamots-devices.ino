#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// inisialisasi url server
const char* serverName = "https://ta.dapurku-ganteng.site/api/device/set-data";

// inisialisasi token alat iot tempat sampah
#define DEVICE_TOKEN "9myfVyGake:3mQiMZO3aE";

// inisialisasi pin sensor
#define echoSensorRight 35
#define trigSensorRight 19

#define echoSensorUp 34
#define trigSensorUp 18

#define echoSensorLeft 39
#define trigSensorLeft 5



HTTPClient http;

float filled, unfilled, distanceUp, distanceRight, distanceLeft, zDef;

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // Ultrasonic Sensor
  pinMode(echoSensorRight,INPUT);
  pinMode(trigSensorRight,OUTPUT);
  
  pinMode(echoSensorUp,INPUT);
  pinMode(trigSensorUp,OUTPUT);
  
  pinMode(echoSensorLeft,INPUT);
  pinMode(trigSensorLeft,OUTPUT);
  // ===

  // inisialisasi wifimanager
  WiFiManager WiMa;
  WiMa.resetSettings();

  Serial.println("Generate AutoConnect WiFi...");

  WiMa.autoConnect("AP_Autoconnect", "123456789");
  Serial.print("Connecting to ");

  Serial.print(WiFi.SSID());
    while (WiFi.status() != WL_CONNECTED) {
      
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      
      Serial.print(".");
      delay(1000);
    }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  distanceLeft = readDistance(trigSensorLeft, echoSensorLeft);
  distanceUp = readDistance(trigSensorUp, echoSensorUp);
  distanceRight = readDistance(trigSensorRight, echoSensorRight);
  
  Serial.print("Jarak Kiri  : ");Serial.println(distanceLeft);
  Serial.print("Jarak Atas  : ");Serial.println(distanceUp);
  Serial.print("Jarak Kanan : ");Serial.println(distanceRight);
  
  Serial.println("");
  fuzzy();
  Serial.println("=================");
  
  if (WiFi.status() == WL_CONNECTED) {
  digitalWrite(LED_BUILTIN, HIGH);

  // Send HTTP POST request to server
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON object
    StaticJsonDocument<1024> doc;
    doc["device_token"] = DEVICE_TOKEN;
    doc["filled"] = filled;
    doc["unfilled"] = unfilled;

  // Serialize JSON object to string
    String requestBody;
    serializeJson(doc, requestBody);

  // Send POST request
    int httpResponseCode = http.POST(requestBody);
    
    if (httpResponseCode > 0) {
      String response = http.getString();

      Serial.println(" ");
      Serial.println(" ");

      Serial.println("========NodeMCU========");
      Serial.println("Terisi : " + String(filled) + "%"); 
      Serial.println("Kosong : " + String(unfilled) + "%");

      Serial.println(" ");

      Serial.println("========Server========");
      Serial.println("Kode Respon : " + String(httpResponseCode));
      Serial.print("Data Respon : ");
      Serial.println(response);
    } else {
      Serial.println("Error on sending request");
      
    }
    
    
    http.end();
    digitalWrite(LED_BUILTIN, LOW);
    }

  delay(15000);

}


float readDistance(int trigPin, int echoPin){
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  float duration = pulseIn(echoPin,HIGH);

  float distance = duration * 0.034/2;
  return distance;
}

void fuzzy(){
  //Fuzzyfikasi
    float atsDekat = bahuKiri(10,20,distanceUp);
    float atsSedang = trapesium(10,20,30,40,distanceUp);
    float atsJauh = bahuKanan(30,40,distanceUp);
  
    float saKanDekat = bahuKiri(10,20,distanceRight);
    float saKanJauh = bahuKanan(10,20,distanceRight);
  
    float saKirDekat = bahuKiri(10,20,distanceLeft);
    float saKirJauh = bahuKanan(10,20,distanceLeft);

  //Implementasi Rule
    float ap1=max(atsDekat,max(saKirDekat,saKanDekat));
    float z1 = (ap1*20)+60;
    float ap2=max(atsDekat,max(saKirDekat,saKanJauh));
    float z2a = (ap2*20)+60;
    float ap3=max(atsDekat,max(saKirJauh,saKanDekat));
    float z3a = (ap3*20)+60;
    float ap4=max(atsDekat,max(saKirJauh,saKanJauh));
    float z4a = (ap4*20)+60;
    float ap5=min(atsSedang,max(saKirDekat,saKanDekat));
    float z5a = (ap5*20)+60;
    float ap6=min(atsSedang,max(saKirDekat,saKanJauh));
    float z6 = 80-(20*ap6);
    float ap7=min(atsSedang,max(saKirJauh,saKanDekat));
    float z7 = 80-(20*ap7);
    float ap8=min(atsSedang,max(saKirJauh,saKanJauh));
    float z8 = 80-(20*ap8);
    float ap9=min(atsJauh,min(saKirDekat,saKanDekat));
    float z9 = 80-(20*ap9);
    float ap10=min(atsJauh,min(saKirDekat,saKanJauh));
    float z10 = 80-(20*ap10);
    float ap11=min(atsJauh,min(saKirJauh,saKanDekat));
    float z11 = 80-(20*ap11);
    float ap12=min(atsJauh,min(saKirJauh,saKanJauh));
    float z12 = 80-(20*ap12);

    float pem = (ap1*z1)+(ap2*z2a)+(ap3*z3a)+(ap4*z4a)+(ap5*z5a)
                +(ap6*z6)+(ap7*z7)+(ap8*z8)+(ap9*z9)+(ap10*z10)+(ap11*z11)+(ap12*z12);
    float pen = ap1+ap2+ap3+ap4+ap5+ap6+ap7+ap8+ap9+ap10+ap11+ap12;

    zDef = pem/pen;   //Hasil Defuzzyfikasi
        
    zDef = fuzzyErrorHandling(distanceUp, distanceRight, distanceLeft, zDef);
 
    filled = ((zDef-60)/(80-60))*100;
    unfilled = ((80-zDef)/(80-60))*100;

    Serial.print("Fuzzy        : ");Serial.println(zDef);
    Serial.print("Penuh        : ");Serial.println(filled);
    Serial.print("Tidak Penuh  : ");Serial.println(unfilled);
}

float fuzzyErrorHandling(float atas, float kanan, float kiri, float init){
  if(atas >= 52 && kanan >= 26 && kiri >= 26){
      return 60;
  }

  if(atas > 49 && atas <= 51.99) {
    return 61;
  }

  if(atas >= 47 && atas <= 49) {
    return 62;
  }
  
  if(atas >= 43 && atas <= 46) {
    return 63;
  }
  
  if(atas >= 35 && atas <= 40){
      return random(65, 68);
  }

  if(atas >= 34 && atas < 35){
    return 64.5;
  }

  return init;
}


float bahuKiri(float a, float b, float x){
  if (x <= a){
    return 1;
  }
  else if(x > a && x <= b){
    return ((b-x)/(b-a));
  }
  else if(x > b){
    return 0;
  }
}


float trapesium(float a, float b, float c, float d, float x){
  if(x < a || x > d){
    return 0;
  }
  else if(x >= b && x <=c){
    return 1;
  }
  else if(x >= a && x <= b){
    return ((x-a)/(b-a));
  }
  else if(x >= c && x <= d){
    return ((d-x)/(d-c));
  }
}

float bahuKanan(float a, float b, float x){
  if(x < a){
    return 0;
  }
  else if(x >= a && x < b){
    return ((x-a)/(b-a));
  }
  else if(x >= b){
    return 1;
  }
}
