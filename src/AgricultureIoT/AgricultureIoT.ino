#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "abcdefghijklmnopqrstywxz";

char ssid[] = "Test";
char pass[] = "TestPassword";

char* toiletStatus;

int debugFlag=0;

const int redPin = 4;    // ~D2
const int greenPin = 12; // ~D6
const int bluePin = 14;  // ~D5

const int switchPin = 5; // D1
const int LedPin = 16; // D0

unsigned long duration;

const int FarmPin = 13; // D7

int WiFiStrength_ = 0;
int WiFiStrength = 0;
bool presence = false;
int toiletUse = 0;
int waterPump = 0;

double analogValue = 0.0;
double analogVolts = 0.0;
int moistureLevel;

WidgetLCD lcd(V3);

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  Blynk.begin(auth, ssid, pass);
  // Difining RGB pin as Output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(FarmPin, OUTPUT);

  pinMode(LedPin, OUTPUT);
  pinMode(switchPin, INPUT);

  // Waiting to Connect LED Color

  analogWrite(redPin, 280);
  analogWrite(greenPin, 300);
  analogWrite(bluePin, 300);
  
  // Connecting to WiFi Network
}

void loop() {

  Blynk.run();

  WiFiStrength_ = WiFi.RSSI(); // Get dBm from ESP8266
  WiFiStrength = -WiFiStrength_;
  
  
  analogValue = analogRead(A0); // Read Analog Signal From Moiture Sensor
  analogVolts = (analogValue*3) / 1024; //Convert Analog Signal to Voltage
  

  analogValue = constrain(analogValue, 485, 1023);
  moistureLevel = map(analogValue, 485,1023,100,0);

  RGBindication();

  Blynk.virtualWrite(V1, moistureLevel);
  Blynk.virtualWrite(V2, WiFiStrength);

  //Looping for RGB and LED Controll    
  //Check Toilet Status

  motorCheck();
  getToietStatus();

  Blynk.virtualWrite(V0, toiletUse);
  Blynk.virtualWrite(V4, waterPump);

  debug();
}

void motorCheck()
{
  int value = digitalRead(switchPin);

  if(value == 1 && presence == false)
  {
    presence = true;
    toiletUse = 255;
    lcd.clear();
    lcd.print(0, 0, " TOILET STATUS ");
    lcd.print(0, 1, "---[ IN USE ]---");
  }

 if(presence == true && value == 0)
 {
   digitalWrite(LedPin, HIGH);
   delay(3000);
   digitalWrite(LedPin, LOW);

   presence = false;
   toiletUse = 0;
   lcd.clear();
   lcd.print(0, 0, " TOILET STATUS ");
   lcd.print(0, 1, "-[Under No USE]-");
  }

}

void getToietStatus()
{
  if(toiletUse == 255)
  {
   toiletStatus = "in Use"; 
  }
  else
  {
    toiletStatus = "under no Use";
  }
}

void debug()
{
   if(debugFlag==1)
   {
    
    Serial.print("Toilet Usage :");
    Serial.println(toiletUse);
    Serial.print("Toilet ");
    Serial.println(toiletStatus);
    Serial.print("Analog raw: ");
    Serial.println(analogValue);
    Serial.print("Analog V: ");
    Serial.println(analogVolts);
    Serial.print("Moisture Level: ");
    Serial.println(moistureLevel);
    Serial.println(" ");
    Serial.print("WiFi Strength: ");
    Serial.print(WiFiStrength); Serial.println("dBm");
   }
}

void RGBindication()
{

  
  if (moistureLevel <= 25) // 0-25 is red "dry"
    {  
      analogWrite(redPin, 1000);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
      waterPump = 255;
      digitalWrite(FarmPin, HIGH);
    }
    
    else if (moistureLevel > 25 && moistureLevel <= 75) // 26-75 is green
    {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 1000);
      analogWrite(bluePin, 0);
     
    }
    
    else if (moistureLevel > 75 ) // 76-100 is blue
    {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 1000);
      waterPump = 0;
      digitalWrite(FarmPin, LOW);
    }
  
}
