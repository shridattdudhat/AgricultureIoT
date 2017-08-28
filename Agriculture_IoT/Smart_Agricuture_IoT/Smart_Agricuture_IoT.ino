#include <ESP8266WiFi.h>

const char* ssid = "Oizom/2G";
const char* password = "9510810866";

const int redPin = 4;    // ~D2
const int greenPin = 12; // ~D6
const int bluePin = 14;  // ~D5

const int IR_SENSOR = 9; //sd2
const int toiletPin = 10; //sd3

unsigned long duration;

const int FarmPin = 13; // D7

int WiFiStrength = 0;
bool presence = false;

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

  pinMode(toiletPin, OUTPUT);
  pinMode(IR_SENSOR, INPUT_PULLUP);

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

  // Serial data
  Serial.print("Toilet Motor :");
  Serial.println(toiletPin);
  Serial.print("IR :");
  Serial.println(IR_SENSOR);
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
  client.println("<script>setTimeout(function() {location.reload();}, 5000);</script>");
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
  int value = digitalRead(IR_SENSOR);

  if(value == 1 && presence == false)
  {
    presence = true;
  }

 if(presence == true && value == 0)
 {
   digitalWrite(toiletPin, HIGH);
   delay(5000);
   digitalWrite(toiletPin, LOW);
   presence = false;
  }

}

