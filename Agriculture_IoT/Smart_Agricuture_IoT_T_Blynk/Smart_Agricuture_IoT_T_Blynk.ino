#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "af65b1232da84cd88fbfb4efb64ea19c";

char ssid[] = "Imagination";
char pass[] = "Shr1datt";

const int redPin = 4;    // ~D2
const int greenPin = 12; // ~D6
const int bluePin = 14;  // ~D5

// Farm Variable Declaration
const int farmMotor = 13; // D7
int waterPumpLED = 0; // Farm Watering Indication

double analogValue = 0.0;
double analogVolts = 0.0;
int moistureLevel;

//Toilet Variable Declaration
const int toiletDoorPin = 5; // D1
const int toiletMotor = 16; // D0
bool presence = false;
int toiletUse = 0;

//WiFi Strenght Variable
int WiFiStrength_ = 0;
int WiFiStrength = 0;

WidgetLCD lcd(V3);

BlynkTimer timer;

void setup() {
  // put your setup code here, to run once:
  
  //Serial Monitor for debug
  Serial.begin(115200);

  //Blynk Begin
  Blynk.begin(auth, ssid, pass);

  // Difining RGB pin as Output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(farmMotor, OUTPUT); // Farm Water Pump

  //Toilet pins
  pinMode(toiletMotor, OUTPUT);
  pinMode(toiletDoorPin, INPUT);

  // Waiting to Connect LED Color
  analogWrite(redPin, 280);
  analogWrite(greenPin, 300);
  analogWrite(bluePin, 300);
  
  // Connecting to WiFi Network
  Serial.println("WiFi Connected !");
}

void loop() {

  Blynk.run();

  WiFiStrength_ = WiFi.RSSI();   // Get dBm from ESP8266
  WiFiStrength = -WiFiStrength_; // Converting to Positive value
  
  analogValue = analogRead(A0);         // Read Analog Signal From Moiture Sensor
  analogVolts = (analogValue*3) / 1024; //Convert Analog Signal to Voltage

  analogValue = constrain(analogValue, 485, 1023);
  moistureLevel = map(analogValue, 485,1023,100,0);
  
  //Taking MoistureLevel & WiFiStrength to Blynk Virtual Pin
  Blynk.virtualWrite(V1, moistureLevel);
  Blynk.virtualWrite(V2, WiFiStrength);

  //Looping for RGB and LED Controll
  if (moistureLevel <= 25) // 0-25 is red "dry"
    {  
      analogWrite(redPin, 1000);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
      waterPumpLED = 255;
      digitalWrite(farmMotor, HIGH);
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
      waterPumpLED = 0;
      digitalWrite(farmMotor, LOW);
    }

  getToietStatus();  //Check Toilet Status

  Blynk.virtualWrite(V0, toiletUse);
  Blynk.virtualWrite(V4, waterPumpLED);
  
  // Serial data for Debug
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

void getToietStatus()
{
  int value = digitalRead(toiletDoorPin); // Reading Toilet Door Status

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
   digitalWrite(toiletMotor, HIGH);
   delay(5000);
   digitalWrite(toiletMotor, LOW);

   presence = false;
   toiletUse = 0;
   lcd.clear();
   lcd.print(0, 0, " TOILET STATUS ");
   lcd.print(0, 1, "-[Under No USE]-");
  }
}
