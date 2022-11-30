/*SHARP GP2Y0A710K0F IR distance sensor with Arduino and SharpIR library example code. More info: https://www.makerguides.com */

// Include the library:
#include <SharpIR.h>
#include <Servo.h>

// Define model and input pin:
#define IRPin_Sentinel_Left A0
#define IRPin_Evaluator A1
#define Servo_pin 8
#define LED_pin 7

#define model 100500

// Create variable to store the distance:
int leftDistance;
int evaluatorDistance;
char buffer[256];

/* Model :
  GP2Y0A02YK0F --> 20150
  GP2Y0A21YK0F --> 1080
  GP2Y0A710K0F --> 100500
  GP2YA41SK0F --> 430
*/

// Create a new instance of the SharpIR class:
SharpIR leftSentinel = SharpIR(IRPin_Sentinel_Left, model);
SharpIR evaluatorNode = SharpIR(IRPin_Evaluator, model);

// Servo object
Servo servo;

void setup() {
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  servo.attach(Servo_pin);
  servo.write(90);
  pinMode(LED_pin, OUTPUT);
}

void loop() {
  // Get a distance measurement and store it as distance_cm:
  for (int angle = 30; angle <= 120; angle += 1) {
    digitalWrite(LED_pin, LOW);
    servo.write(angle);
    leftDistance = leftSentinel.distance();
    evaluatorDistance = evaluatorNode.distance();
    
    // Print the measured distance to the serial monitor:
    sprintf(buffer, "Mean distances: (%d,%d)", leftDistance, evaluatorDistance);
    Serial.println(buffer);
    if (evaluatorDistance == 0) {
      digitalWrite(LED_pin, HIGH);
      delay(1000);
    }
    delay(25);
  }
  
  delay(500);
}

// Scraps
   // scan from 0 to 180 degrees
//  for(int angle = 30; angle < 150; angle+=10)  
//  {                                  
//    servo.write(angle);
//    evaluatorDistance = evaluatorNode.distance();
//    sprintf(buffer, "angle: %d, dist %d", angle, evaluatorDistance);
//    Serial.println(buffer);              
//    delay(125);                   
//  } 
