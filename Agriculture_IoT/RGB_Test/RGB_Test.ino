#define greenLed 9    //GREEN
#define redLed 10   //RED
#define blueLed 11   //BLUE

void setup() {
  // put your setup code here, to run once:
  pinMode(greenLed, OUTPUT);    //GREEN
  pinMode(redLed, OUTPUT);    //RED
  pinMode(blueLed, OUTPUT);    //BLUE
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);  

    
  analogWrite(greenLed, 255); delay(1000); analogWrite(greenLed, 0);   
  // Red Color for 1 second
  analogWrite(redLed, 255); delay(1000); analogWrite(redLed, 0);    
  // Blue Color for 1 second
  analogWrite(blueLed, 255); delay(1000); analogWrite(blueLed, 0);
  delay(2000);
  
}

void loop() {
  // put your main code here, to run repeatedly:


  ledglow(255,128,30);
}



void ledglow(int green, int red, int blue)
{
  if (green <= 255 && red <= 255 && blue <= 255) {

    analogWrite(greenLed, green);
    analogWrite(redLed, red);
    analogWrite(blueLed, blue);
  }
}
