/*SHARP GP2Y0A710K0F IR distance sensor with Arduino and SharpIR library example code. More info: https://www.makerguides.com */
// Include the library:
#include <SharpIR.h>
#include <SharpIR.h>
#include <Servo.h>

// Define model and input pin:
#define IRPin_Sentinel_Left A0
#define IRPin_Evaluator A1
#define SERVO_PIN 8
#define LED_PIN 7
#define model 100500 //do not change

// ------------------ VARIABLES TO ADJUST -----------------
const int DETECTION_THRESHOLD = 80; //distance threshold in cm
const int VEHICLE_LENGTH = 4; //how many datapoints 
const int SERVO_TURN_SPEED = 2; //angle for each move 

//FIXED variables
const int num_datapoint = (120/SERVO_TURN_SPEED)+1;
int arr[num_datapoint];
const int EVALUATOR_START_ANGLE = 30;
const int EVALUATOR_END_ANGLE = 150;
const int DORMANT_STATE = 0;
const int AWAKE_STATE = 1;
const int ALERT_STATE = 2;

int servo_angle;
int NUM_DETECTION = 0;
int alert_counter = 0;
int awake_counter = 0;
int index = 0;

// Create variable to store the distance:
int leftDistance;
int evaluatorDistance;

// Stores the state of the system
int system_state;

char buffer[256];

// Create a new instance of the SharpIR class:
SharpIR leftSentinel = SharpIR(IRPin_Sentinel_Left, model);
SharpIR evaluatorNode = SharpIR(IRPin_Evaluator, model);
Servo servo;

int get_evaluator_reading() {
  return evaluatorNode.distance();
}

void set_servo_angle(int angle) {
  servo_angle = angle;
  // TODO: HARDWARE STUFF
  servo.write(servo_angle);
}

void enter_dormant_state() {
  Serial.println("DORMANT state activated");
  system_state = DORMANT_STATE;
  set_servo_angle(EVALUATOR_START_ANGLE);
  turn_LED_off();
}

void enter_awake_state() {
  Serial.println("AWAKE state activated");
  system_state = AWAKE_STATE;
  turn_LED_on();
}

void enter_alert_state() {
  Serial.println("ALERT state activated");
  system_state = ALERT_STATE;
  turn_LED_blink();
}

void turn_LED_on() {
  digitalWrite(LED_PIN, HIGH);
}

void turn_LED_off() {
  digitalWrite(LED_PIN, LOW);
}

void turn_LED_blink() {
  for (int i = 0; i < 15; i++) {
    digitalWrite(LED_PIN, HIGH);  
    delay(300);                 
    digitalWrite(LED_PIN, LOW);   
    delay(300);
  }
}

bool is_detecting_obstructor(SharpIR* sensor_ir) {
  int detected_distance = sensor_ir->distance();
  return detected_distance <= DETECTION_THRESHOLD && detected_distance >= 1;
}

void setup() {
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
  pinMode(LED_PIN, OUTPUT);
  enter_dormant_state();
}

void loop() { 
  // Get a distance measurement and store it as distance_cm:
  leftDistance = leftSentinel.distance();
  evaluatorDistance = evaluatorNode.distance();

  if (system_state == DORMANT_STATE){
    if (is_detecting_obstructor(&leftSentinel) || is_detecting_obstructor(&evaluatorNode)) {
      enter_awake_state();
    }
  }
  if (system_state == AWAKE_STATE){
    do_detection();

    if (NUM_DETECTION >= 10){
      enter_alert_state();//change state and turn light blink
      NUM_DETECTION = 0;
    }
    //if detect no car
    if (NUM_DETECTION == 0){
      awake_counter += 1;
      if (awake_counter > 6){
        awake_counter = 0;
        enter_dormant_state();
      }
    }
  }
  if (system_state == ALERT_STATE){ 
    do_detection();
    if (NUM_DETECTION >= 3){
        enter_alert_state();
        NUM_DETECTION = 0;
    }
    if (NUM_DETECTION == 0){
        alert_counter += 1 ;
        if (alert_counter >= 3){
          alert_counter = 0;
          enter_awake_state();
        }
    }
}
  delay(10);
}

//make a clockwise and counter-clockwise turn, and load data to array
void do_detection() {
  int angle;
  for(angle = EVALUATOR_START_ANGLE; angle <= EVALUATOR_END_ANGLE; angle = angle + SERVO_TURN_SPEED)  
  {                                  
    servo.write(angle);     
    arr[index] = get_evaluator_reading(); 
    index += 1;   
    delay(5); 
  } 
  index = 0;  
  analyze();

  for(angle = EVALUATOR_END_ANGLE; angle >= EVALUATOR_START_ANGLE; angle= angle - SERVO_TURN_SPEED)    
  {                                
    servo.write(angle);   
    arr[index] = get_evaluator_reading(); 
    index += 1; 
    delay(5);             
  } 
  index = 0;
  analyze();
}

//Analyze array of data point
void analyze(){
  int max = 0;
  int counter = 0;
  //loop array to find the max segment of continuous value that is below threshold
  for (int i = 0; i < num_datapoint; i++ ){
    if (arr[i] <= DETECTION_THRESHOLD && arr[i] >= 1){ 
      counter += 1 ;
    } 
    else { //break point
      if (counter > max){ 
        max = counter;
      }
      counter = 0;
    }
  }
  if (max >= VEHICLE_LENGTH){
    NUM_DETECTION += 1;
    sprintf(buffer, "Max segment length: %d\t, detection+1, current NUM_DETECTION: %d", max, NUM_DETECTION);
    Serial.println(buffer);
  }
  else{
    NUM_DETECTION = 0;
  }
}