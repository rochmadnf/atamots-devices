const int echoPins[3] = { 34, 35, 36 };
const int trigPins[3] = { 18, 19, 5 };

float distances[3];

void setup() {
  Serial.begin(115200);
  for(byte pinNumber = 0; pinNumber < 3; pinNumber++){
    pinMode(echoPins[pinNumber], INPUT);
    pinMode(trigPins[pinNumber], OUTPUT);
  }  

}

void loop() {
  
  for(byte i = 0; i < 3; i++){
    distances[i] = getDistance(trigPins[i], echoPins[i]);
    delay(500);
  }

  Serial.println(String(distances[0]) + " | " + String(distances[1]) + " | " + String(distances[2]) + " | ");
  delay(10000);
  

}

float getDistance(byte trig, byte echo){
  //  local variable
  long duration;
  float distance;
  
  digitalWrite(trig, LOW);
  delayMicroseconds(20);
  digitalWrite(trig, HIGH);
  delayMicroseconds(100);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);

  distance = duration * (0.034/2);
  Serial.println("Pin Trig : " + String(trig) + ", D : " + String(distance));
  return distance;
}
