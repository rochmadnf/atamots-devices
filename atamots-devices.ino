#define echoR 35
#define trigR 19

#define echoC 34
#define trigC 18

#define echoL 39
#define trigL 5

float jarC, jarL, jarR;

void setup(){
  Serial.begin(115200);
  
  pinMode(echoR,INPUT);
  pinMode(trigR,OUTPUT);
  
  pinMode(echoC,INPUT);
  pinMode(trigC,OUTPUT);
  
  pinMode(echoL,INPUT);
  pinMode(trigL,OUTPUT);
}

void loop(){
  jarL = readDistance(trigL, echoL);
  jarC = readDistance(trigC, echoC);
  jarR = readDistance(trigR, echoR);
  
  Serial.print("Jarak Kiri  : ");Serial.println(jarL);
  Serial.print("Jarak Atas  : ");Serial.println(jarC);
  Serial.print("Jarak Kanan : ");Serial.println(jarR);
  
  Serial.println("");
  fuzzy();
  Serial.println("=================");
  delay(1000);
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

  float batFull, batTidFull;
  float zDef;

void fuzzy(){
  //Fuzzyfikasi
    float atsDekat = bahuKiri(10,20,jarC);
    float atsSedang = trapesium(10,20,30,40,jarC);
    float atsJauh = bahuKanan(30,40,jarC);
  
    float saKanDekat = bahuKiri(10,20,jarR);
    float saKanJauh = bahuKanan(10,20,jarR);
  
    float saKirDekat = bahuKiri(10,20,jarL);
    float saKirJauh = bahuKanan(10,20,jarL);

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
        
    zDef = fuzzyErrorHandling(jarC, jarL, jarR, zDef);
 
    batFull = ((zDef-60)/(80-60))*100;
    batTidFull = ((80-zDef)/(80-60))*100;

    Serial.print("Fuzzy     : ");Serial.println(zDef);
    Serial.print("Full      : ");Serial.println(batFull);
    Serial.print("Tid Full  : ");Serial.println(batTidFull);
}

float fuzzyErrorHandling(float atas, float kanan, float kiri, float init){
  if(atas >= 52 && kanan >= 26 && kiri >= 26){
      return 60;
  }

  if(atas >= 43 && atas <= 46) {
    return 63;
  }
  
  if(atas >= 35 && atas <= 40){
      return random(65, 68);
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
