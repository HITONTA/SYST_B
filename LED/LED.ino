//www.elegoo.com
//2016.12.08

int ledPin = 6;



void setup() 
{
  pinMode(ledPin, OUTPUT);
}

void loop() 
{
    delay(300);
    digitalWrite(ledPin, HIGH);
    delay(300);
    digitalWrite(ledPin, LOW);
}
