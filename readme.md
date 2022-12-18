# ParkSmart

A smart solution to enforce illegal parking from prohibited spaces. Current version focuses on fire hydrant prohibited parking zones.

1. Hardward Setup: connect the Arduino with correct input pin, import the SharpIR-master.zip file to Arduino IDE.

2. To Run and Tune version 2:
Only modify parameters under the "variables to adjust" comment 
- DETECTION_THRESHOLD: distance between sensor and target object. num_datapoint = (120/SERVO_TURN_SPEED)+1.  If an IR reading is less than this threshold, we count this as a detected object (vehicle). 
- SERVO_TURN_SPEED: angle for each move of servo motor.
- VEHICLE_LENGTH: this length is defined by the number of data points. For example when VEHICLE_LENGTH = 20 and SERVO_TURN_SPEED = 2, for each full scan, if there are 20 continous data points that are lower than the DETECTION_THRESHOLD, it means there's a vehicle longer than 3.4m parked within the zone. 
