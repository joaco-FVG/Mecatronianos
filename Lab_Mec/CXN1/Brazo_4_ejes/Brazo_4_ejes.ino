//      ******************************************************************
//      *                                                                *
//      *     Example of running two or more motors at the same time     *
//      *                                                                *
//      *            S. Reifel & Co.                6/24/2018            *
//      *                                                                *
//      ******************************************************************
// For faster step rates, see:
//    https://github.com/Stan-Reifel/SpeedyStepper
//
//
// Documentation for this library is at:
//    https://github.com/Stan-Reifel/FlexyStepper
//
//
// The motor must be connected to the Arduino with a driver board having a 
// "Step and Direction" interface.  It's VERY important that you set the 
// motor current first!  Read the driver board's documentation to learn how.
// ***********************************************************************
//Reduccion de las articulaciones del CXN
//cintura 30:1
//hombro 30:1
//codo 35:1
//muñeca 55:1

#include <FlexyStepper.h>
                                  //Pasos*Reduccion
#define K_1               0.5*400    // 12000   //400*30
#define K_2               0.5*400   // -12000  //400*30
#define K_3               0.5*400  // -22000  //400*55
#define K_4               0.5*400 // -14000  //400*35

#define M1_ENABLE_PIN       38
#define M2_ENABLE_PIN       56
#define M3_ENABLE_PIN       62
#define M4_ENABLE_PIN       24


int a_0 = 0;
int b_0 = 0;
int c_0 = 0;
int d_0 = 0;
int a_1 = 0;
int b_1 = 0;
int c_1 = 0;
int d_1 = 0;

const int LED_PIN = 13;
const int MOTOR_E1_STEP_PIN = 54;
const int MOTOR_E1_DIRECTION_PIN = 55;
const int MOTOR_E2_STEP_PIN = 60;
const int MOTOR_E2_DIRECTION_PIN = 61;
const int MOTOR_E3_STEP_PIN = 46;
const int MOTOR_E3_DIRECTION_PIN = 48;
const int MOTOR_E4_STEP_PIN = 26;
const int MOTOR_E4_DIRECTION_PIN = 28;
/*
#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define E_STEP_PIN         26
#define E_DIR_PIN          28
#define E_ENABLE_PIN       24

#define Q_STEP_PIN         36
#define Q_DIR_PIN          34
#define Q_ENABLE_PIN       30
*/
//
// create two stepper motor objects, one for each motor
//
FlexyStepper stepperE1;
FlexyStepper stepperE2;
FlexyStepper stepperE3;
FlexyStepper stepperE4;

int giro=1;

void setup() 
{
  pinMode(LED_PIN, OUTPUT);   
  Serial.begin(9600);

  stepperE1.connectToPins(MOTOR_E1_STEP_PIN, MOTOR_E1_DIRECTION_PIN);
  stepperE2.connectToPins(MOTOR_E2_STEP_PIN, MOTOR_E2_DIRECTION_PIN);
  stepperE3.connectToPins(MOTOR_E3_STEP_PIN, MOTOR_E3_DIRECTION_PIN);
  stepperE4.connectToPins(MOTOR_E4_STEP_PIN, MOTOR_E4_DIRECTION_PIN);

  pinMode( M1_ENABLE_PIN, OUTPUT);
  pinMode( M2_ENABLE_PIN, OUTPUT);
  pinMode( M3_ENABLE_PIN, OUTPUT);
  pinMode( M4_ENABLE_PIN, OUTPUT);
  digitalWrite(M1_ENABLE_PIN, LOW);
  digitalWrite(M2_ENABLE_PIN, LOW);
  digitalWrite(M3_ENABLE_PIN, LOW);
  digitalWrite(M4_ENABLE_PIN, LOW);
}


void loop() 
{
  float a=0,b=0,c=0,d=0,e=0,f=0;
  
  if(Serial.available()>0){
    
    a = Serial.parseInt();
    b = Serial.parseInt();
    c = Serial.parseInt();
    d = Serial.parseInt();
    e = Serial.parseInt();
    f = Serial.parseInt();
    a_1=a;
    b_1=b;
    c_1=c;
    d_1=d;
    a = a - a_0;
    b = b - b_0;
    c = c - c_0;
    d = d - d_0; 
    a_0 = a_1;
    b_0 = b_1;
    c_0 = c_1;
    d_0 = d_1;
    Serial.print("nuevo movimiento: ");
    Serial.print(a);
    Serial.print(",  ");
    Serial.print(b);
    Serial.print(",  ");
    Serial.print(c);
    Serial.print(",  ");
    Serial.print(d);
    Serial.print(",  ");
    Serial.print(e);
    Serial.println();
    Serial.print(f);
    Serial.println();
    while(Serial.available()) Serial.read();
  }
  if(a!=0 || b!=0 || c!=0 || d!=0) {
    movePolar(a, b, c, d, e, f);
    delay(3000);
  }
  a=0;  
}
// move both X & Y motors together in a coordinated way, such that they each 
// start and stop at the same time, even if one motor moves a greater distance
//
int moveWithCoordination(long stepsE1, long stepsE2, long stepsE3, long stepsE4, float speedInStepsPerSecond, float accelerationInStepsPerSecondPerSecond)
{
  float speed_E1;
  float acceleration_E1;
  float speed_E2;
  float acceleration_E2;
  float speed_E3;
  float acceleration_E3;
  float speed_E4;
  float acceleration_E4;
  float scalerA;
  float scalerB;
  float scalerC;
  long absStepsE1;
  long absStepsE2;
  long absStepsE3;
  long absStepsE4;

  ///////////////////////////////////////////////////////
  // Seteamos velocidades y aceleraciones iniciales

  speed_E1 = speedInStepsPerSecond;
  acceleration_E1 = accelerationInStepsPerSecondPerSecond;
  
  speed_E2 = speedInStepsPerSecond;
  acceleration_E2 = accelerationInStepsPerSecondPerSecond;

  speed_E3 = speedInStepsPerSecond;
  acceleration_E3 = accelerationInStepsPerSecondPerSecond;

  speed_E4 = speedInStepsPerSecond;
  acceleration_E4 = accelerationInStepsPerSecondPerSecond;

  ////////////////////////////////////////////////////////////
  // Determinamos el valor absoluto de las incursiones
 
  if (stepsE1 >= 0)
    absStepsE1 = stepsE1;
  else
    absStepsE1 = -stepsE1;
 
  if (stepsE2 >= 0)
    absStepsE2 = stepsE2;
  else
    absStepsE2 = -stepsE2;

  if (stepsE3 >= 0)
    absStepsE3 = stepsE3;
  else
    absStepsE3 = -stepsE3;

  if (stepsE4 >= 0)
    absStepsE4 = stepsE4;
  else
    absStepsE4 = -stepsE4;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Si la incursion de E1 es mayor que E2, E3 y E4, la articulacion E1 tendra la velocidad y aceleracion maxima
  //y los demas velocidades y aceleraciones menores determinadas por la multiplicacion por un factor (scaler).
  
  if ((absStepsE1 > absStepsE2) && (absStepsE1 > absStepsE3) && (absStepsE1 > absStepsE4) && (stepsE1 != 0))
  {
    scalerA = (float) absStepsE2 / (float) absStepsE1;
    speed_E2 = speed_E2 * scalerA;
    acceleration_E2 = acceleration_E2 * scalerA;
    
    scalerB = (float) absStepsE3 / (float) absStepsE1;
    speed_E3 = speed_E3 * scalerB;
    acceleration_E3 = acceleration_E3 * scalerB;

    scalerC = (float) absStepsE4 / (float) absStepsE1;
    speed_E4 = speed_E4 * scalerC;
    acceleration_E4 = acceleration_E4 * scalerC;
  }

  //Si la incursion E2 es mayor que la de E1, E3 y E4 determino velocidades y aceleraciones
  
  if ((absStepsE2 > absStepsE1) && (absStepsE2 > absStepsE3) && (absStepsE2 > absStepsE4) && (stepsE2 != 0))
  {
    scalerA = (float) absStepsE1 / (float) absStepsE2;
    speed_E1 = speed_E1 * scalerA;
    acceleration_E1 = acceleration_E1 * scalerA;
    
    scalerB = (float) absStepsE3 / (float) absStepsE2;
    speed_E3 = speed_E3 * scalerB;
    acceleration_E3 = acceleration_E3 * scalerB;

    scalerC = (float) absStepsE4 / (float) absStepsE2;
    speed_E4 = speed_E4 * scalerC;
    acceleration_E4 = acceleration_E4 * scalerC;
  }
  
  //Si la incursion E3 es mayor que la de E1, E2 y E4 determino velocidades y aceleraciones
  
  if ((absStepsE3 > absStepsE1) && (absStepsE3 > absStepsE2) && (absStepsE3 > absStepsE4) && (stepsE3 != 0))
  {
    scalerA = (float) absStepsE1 / (float) absStepsE3;
    speed_E1 = speed_E1 * scalerA;
    acceleration_E1 = acceleration_E1 * scalerA;
    
    scalerB = (float) absStepsE2 / (float) absStepsE3;
    speed_E2 = speed_E2 * scalerB;
    acceleration_E2 = acceleration_E2 * scalerB;

    scalerC = (float) absStepsE4 / (float) absStepsE3;
    speed_E4 = speed_E4 * scalerC;
    acceleration_E4 = acceleration_E4 * scalerC;
  }
 
  //Si la incursion E4 es mayor que la de E1, E2 y E3 determino velocidades y aceleraciones
  
  if ((absStepsE4 > absStepsE1) && (absStepsE4 > absStepsE2) && (absStepsE4 > absStepsE3) && (stepsE4 != 0))
  {
    scalerA = (float) absStepsE1 / (float) absStepsE4;
    speed_E1 = speed_E1 * scalerA;
    acceleration_E1 = acceleration_E1 * scalerA;
    
    scalerB = (float) absStepsE2 / (float) absStepsE4;
    speed_E2 = speed_E2 * scalerB;
    acceleration_E2 = acceleration_E2 * scalerB;

    scalerC = (float) absStepsE3 / (float) absStepsE4;
    speed_E3 = speed_E3 * scalerC;
    acceleration_E3 = acceleration_E3 * scalerC;
  } 
  
  //////////////////////////////////////////////////////////////////////////////////////////////
  // Seteamos en los motores las incursiones, velocidades y aceleraciones
  
  stepperE1.setSpeedInStepsPerSecond(speed_E1);
  stepperE1.setAccelerationInStepsPerSecondPerSecond(acceleration_E1);
  stepperE1.setTargetPositionRelativeInSteps(stepsE1);


  stepperE2.setSpeedInStepsPerSecond(speed_E2);
  stepperE2.setAccelerationInStepsPerSecondPerSecond(acceleration_E2);
  stepperE2.setTargetPositionRelativeInSteps(stepsE2);

  stepperE3.setSpeedInStepsPerSecond(speed_E3);
  stepperE3.setAccelerationInStepsPerSecondPerSecond(acceleration_E3);
  stepperE3.setTargetPositionRelativeInSteps(stepsE3);

  stepperE4.setSpeedInStepsPerSecond(speed_E4);
  stepperE4.setAccelerationInStepsPerSecondPerSecond(acceleration_E4);
  stepperE4.setTargetPositionRelativeInSteps(stepsE4);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Ejecutamos el movimiento
  
  while((!stepperE1.motionComplete()) || (!stepperE2.motionComplete()) || (!stepperE3.motionComplete() || (!stepperE4.motionComplete())))
  {
    stepperE1.processMovement();
    stepperE2.processMovement();
    stepperE3.processMovement();
    stepperE4.processMovement();
  }

  return 1;
}
int movePolar(float a_p, float  b_p, float c_p, float d_p, float e_p, float f_p)
{
  int a_s = a_p * K_1 /360;
  int b_s = b_p * K_2 /360;
  int c_s = c_p * K_3 /360;
  int d_s = d_p * K_4 /360;
  float e_s = e_p;
  float f_s = f_p;
  moveWithCoordination(a_s, b_s, c_s, d_s, e_s, f_s);  
}
