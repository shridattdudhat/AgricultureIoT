#include <ESP8266WiFi.h>

const char* ssid = "Oizom/2G";
const char* password = "9510810866";

char* toiletStatus;

const int redPin = 4;    // ~D2
const int greenPin = 12; // ~D6
const int bluePin = 14;  // ~D5

const int switchPin = 5; // D1
const int LedPin = 16; // D0

unsigned long duration;

const int FarmPin = 13; // D7

int WiFiStrength = 0;
bool presence = false;
bool toiletUse = false;

WiFiServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  
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
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Connect to WiFi Router

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Starting the Server
  server.begin();
  Serial.println("Server Started");

  // Print Local IP Address
  Serial.print("Use this URL to Connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}


double analogValue = 0.0;
double analogVolts = 0.0;
int moistureLevel;

void loop() {
  // put your main code here, to run repeatedly:
  
  WiFiStrength = WiFi.RSSI(); // Get dBm from ESP8266
  analogValue = analogRead(A0); // Read Analog Signal From Moiture Sensor
  analogVolts = (analogValue*3) / 1024; //Convert Analog Signal to Voltage
  

  analogValue = constrain(analogValue, 485, 1023);
  moistureLevel = map(analogValue, 485,1023,100,0);
  

  //Looping for RGB and LED Controll

  if (moistureLevel <= 25) // 0-25 is red "dry"
    {  
      analogWrite(redPin, 1000);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
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
      digitalWrite(FarmPin, LOW);
    }

    delay(1000); // this is the duration the LED will stay ON

    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);

  motorCheck();
  getToietStatus();

  // Serial data
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
  Serial.print("WiFi Strength: ");
  Serial.print(WiFiStrength); Serial.println("dBm");
  Serial.println(" ");
  delay(1000); // slows amount of data sent via serial

  // Check Client Request from Browser
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");

  // WebPage
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<script>setTimeout(function() {location.reload();}, 1000*60);</script>");
  client.println("<head></head>");
  client.println("<body style=background-color:powderblue;>");
  client.println("<div style=text-align:center: center>");
  client.print("<h1 style=\"size:12px;\">Soil Moisture</h1>");
  client.print("WiFi Signal Strength: ");
  client.println(WiFiStrength);
  client.println("dBm<br>");
  client.print("Analog Raw: ");
  client.println(analogValue);
  client.print("<br>Analog Volts: ");
  client.println(analogVolts);
  client.print("<br>Moiture Level: ");
  client.print(moistureLevel);
  client.println("</div>");
  client.println("</body>");
  client.println("</html>");
  // End of WebPage
  
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

void motorCheck()
{
  int value = digitalRead(switchPin);

  if(value == 1 && presence == false)
  {
    presence = true;
    toiletUse = true;
  }

 if(presence == true && value == 0)
 {
   digitalWrite(LedPin, HIGH);
   delay(5000);
   digitalWrite(LedPin, LOW);
   presence = false;
   toiletUse = false;
  }

}

void getToietStatus()
{
  if(toiletUse == true)
  {
   toiletStatus = "in Use"; 
  }
  else
  {
    toiletStatus = "under no Use";
  }
}

