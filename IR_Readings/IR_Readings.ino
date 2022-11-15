/*SHARP GP2Y0A710K0F IR distance sensor with Arduino and SharpIR library example code. More info: https://www.makerguides.com */

// Include the library:
#include <SharpIR.h>

// Define model and input pin:
#define IRPin_Sentinel_Left A2
#define IRPin_Evaluator A1
#define IRPin_Sentinel_Right A0

#define model 100500

// Create variable to store the distance:
int leftDistance;
int evaluatorDistance;
int rightDistance;
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
SharpIR rightSentinel = SharpIR(IRPin_Sentinel_Right, model);

void setup() {
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
}

void loop() {
  // Get a distance measurement and store it as distance_cm:
  leftDistance = leftSentinel.distance();
  evaluatorDistance = evaluatorNode.distance();
  rightDistance = rightSentinel.distance();
  
  // Print the measured distance to the serial monitor:
  sprintf(buffer, "Mean distances: (%d,%d,%d)", leftDistance, evaluatorDistance, rightDistance);
  Serial.println(buffer);

  delay(1000);
}
