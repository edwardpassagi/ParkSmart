/*SHARP GP2Y0A710K0F IR distance sensor with Arduino and SharpIR library example code. More info: https://www.makerguides.com */

// Include the library:
#include <SharpIR.h>
#include <Servo.h>

// Define model and input pin:
#define IRPin_Sentinel_Left A0
#define IRPin_Evaluator A1
#define SERVO_PIN 8
#define LED_PIN 7

#define model 100500

/*
  If an IR reading is less than this threshold, we count this as
  a detected object.
*/
const int SERVO_TURN_SPEED = 10;
const int DETECTION_THRESHOLD = 120;
const int MIDDLE_SERVO_ANGLE = 90;
const int IR_ANGLE_OFFSET = 60;
const int EVALUATOR_START_ANGLE = MIDDLE_SERVO_ANGLE - IR_ANGLE_OFFSET;
const int EVALUATOR_END_ANGLE = MIDDLE_SERVO_ANGLE + IR_ANGLE_OFFSET;
const int MAX_OSCILLATIONS = 4;
const int PADDING = 15;

const int DORMANT_STATE = 0;
const int AWAKE_STATE = 1;

const int CCW = 1;
const int CW = -1;
const int DEFAULT_TURN_DIRECTION = CCW;

// Create variable to store the distance:
int leftDistance;
int evaluatorDistance;

// Stores the state of the system
int system_state;

// Determines whether the evaluator node is locked or scanning
bool is_evaluator_locked;

// Indicates whether the alarm is on
bool is_alarm_on;

// keeps track of the servo angle
int servo_angle;
int turn_direction;

// the current number of oscillations done by the evaluator
int oscillations;

// the reading last step
int previous_reading;

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
  Serial.println("DORMANT");
  system_state = DORMANT_STATE;
  set_servo_angle(EVALUATOR_START_ANGLE);
  turn_direction = DEFAULT_TURN_DIRECTION;
  is_alarm_on = false;
  oscillations = 0;
  previous_reading = INFINITY;
  turn_LED_off();
}

void enter_awake_state() {
  Serial.println("awake state activated");
  system_state = AWAKE_STATE;
  is_alarm_on = false;
  turn_LED_on();
}

void do_turn_servo() {
  servo_angle = servo_angle + (SERVO_TURN_SPEED*turn_direction);
  set_servo_angle(servo_angle);
}

void enable_servo() {
  return;
}

void disable_servo() {
  return;
}

void turn_LED_on() {
  digitalWrite(LED_PIN, HIGH);
}

void turn_LED_off() {
  digitalWrite(LED_PIN, LOW);
}

void switch_servo_turn_direction() {
  turn_direction = (turn_direction == CCW)? CW : CCW;
}

bool is_detecting_obstructor(SharpIR* sensor_ir) {
  int detected_distance = sensor_ir->distance();
  return detected_distance <= DETECTION_THRESHOLD;
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
  
  // Print the measured distance to the serial monitor:
  sprintf(buffer, "EVAL: %d\tLEFT: %d\to: %d\tangle: %d\tdir: %d", evaluatorDistance, leftDistance, oscillations, servo_angle, turn_direction);
  Serial.println(buffer);

  if (system_state == DORMANT_STATE){
    if (is_detecting_obstructor(&leftSentinel) || is_detecting_obstructor(&evaluatorNode)) {
      enter_awake_state();
    }
  }
  if (system_state == AWAKE_STATE){
    do_awake_state();
  }

  delay(20);
}

void do_awake_state() {
  if (!is_evaluator_locked) {
    do_turn_servo();
  }

  if ((servo_angle <= EVALUATOR_START_ANGLE && turn_direction == CW) || (servo_angle >= EVALUATOR_END_ANGLE && turn_direction == CCW)) {
    oscillations += 1;
    if (servo_angle <= EVALUATOR_START_ANGLE) {
      turn_direction = CCW;
    } else if (servo_angle >= EVALUATOR_END_ANGLE){
      turn_direction = CW;
    }
  }

  if (get_evaluator_reading() > previous_reading + PADDING) {
    if (!is_evaluator_locked) {
      is_evaluator_locked = true;
    } else {
      is_evaluator_locked = false;
    }
  }

  if (is_evaluator_locked && !is_detecting_obstructor(&evaluatorNode)) {
    is_evaluator_locked = false;
  }

  if (is_detecting_obstructor(&evaluatorNode)) {
    oscillations = 0;
  }

  previous_reading = get_evaluator_reading();
  if (oscillations == MAX_OSCILLATIONS) {
    enter_dormant_state();
  }
}
