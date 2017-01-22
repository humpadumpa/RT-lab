/*
##################################################
##  ASTERIX PROJECT                             ##
##                                              ##
## Subproject   : Asterix, realtime kernel      ##
##                                              ##
## COPYRIGHT (c) 2000                           ##
##################################################
*/
/*
**************************************************
**  File        : user.c                        **
**  Date        : 2005-08-10                    **
**  Author(s)   : Anders Pettersson             **
**                                              **
**************************************************
*/
/*
--------------------------------------------------
-- Description  :                               --
--      User defined tasks can be placed here.  --
--      The idle task must always be present.   --
--                                              --
--------------------------------------------------
*/

/* System specific includes do not remove */
#include <os_kernel.h>
#include <sys_spec.h>

/* User includes can be placed here */
#include <rcx_display.h>
#include <rcx_motor.h>
#include <rcx_sensor.h>

#define SEARCH 0
#define FOLLOW 1
#define OBSTACLE_LEFT 2
#define OBSTACLE_RIGHT 3
#define FAILURE 4

#define MOVE_TIME 80
#define TURN_TIME 16
#define BACK_TIME 35

#define MOVE_FORWARD 0
#define MOVE_BACKWARD 1
#define SPIN_LEFT 2
#define SPIN_RIGHT 3
#define TURN_LEFT 4
#define TURN_RIGHT 5
#define TURN_LEFT_SLOW 6
#define TURN_RIGHT_SLOW 7
#define SLOW_DOWN 8
#define WAIT 9

static int8 programStarted = 1;

static uint8 tick = 0;
static uint8 seconds = 0;
static uint8 minutes = 0;
static uint8 hours = 0;

static int16 sensor1, sensor2, sensor3;
static int8 leftFlag = MOTOR_STOP, rightFlag = MOTOR_STOP;

static int16 timestamp = 0, stateTime = 0;
static int8 state = SEARCH;
static int8 searchState = 12, obstacleState = 0, followState = 0;
static int8 searchType = WAIT, obstacleType, moveType;

static int8 startFollow = 0, hitTrack = 0;

/*
----------------------------------------
-- Idle task, do not remove or put in --
-- non-terminating loops.             --
----------------------------------------
*/
C_task void idletask( void *ignore )
{

}


/*
----------------------------------------
-- Increasing a counter               --
-- and write it out on the LCD-display--
----------------------------------------
*/

C_task void tock(void * ignore){
  uint16 whatever;
  if(tick >= 100){
    seconds += tick / 100;
    tick = tick % 100;

    if(seconds >= 60){
      minutes += seconds / 60;
      seconds = seconds % 60;

      if(minutes >= 60){
        hours += minutes / 60;
        minutes = minutes % 60;
      }
    }
  }
  tick += 1;
  whatever = minutes*100+seconds;
  //setLcdNumber(LCD_UNSIGNED, whatever, 0);
  //refreshDisplay();
}

/*
----------------------------------------
--
----------------------------------------
*/

C_task void getOptSensor(void * ignore){
  initSensors();
  setSensorPassive(SENSOR_2);
  readSensor(SENSOR_2, &sensor2);
  //setLcdNumber(LCD_SIGNED, sensor2, 0);
  //refreshDisplay();
  setSensorActive(SENSOR_2);
}

C_task void getLeftSensor(void * ignore){
  initSensors();
  setSensorPassive(SENSOR_1);
  readSensor(SENSOR_1, &sensor1);
  setSensorActive(SENSOR_1);
}

C_task void getRightSensor(void * ignore){
  initSensors();
  setSensorPassive(SENSOR_3);
  readSensor(SENSOR_3, &sensor3);
  setSensorActive(SENSOR_3);
}
  
C_task void motorFunction(void * ignore){

  int16 leftSensor;
  int16 optSensor;
  int16 rightSensor;
  int16 dt = 0;
  int8 temp1, temp2;

  if (programStarted) {
    programStarted = 0;
    return;
  }

  leftSensor = sensor1;
  rightSensor = sensor3;
  optSensor = sensor2;
  
  switch(state) {
    case FAILURE:
         setLcdNumber(LCD_SIGNED, 8008, 0);
         refreshDisplay();
         break;
    case SEARCH:
         raiseSignal(searchSignal);
         break;
    case FOLLOW:
         raiseSignal(followSignal);
         break;
    case OBSTACLE_LEFT: case OBSTACLE_RIGHT:
         raiseSignal(obstacleSignal);
         break;
    default:
         state = SEARCH;
         break;
         }
  }

  
  switch(moveType) {
    case MOVE_FORWARD:
      leftFlag = MOTOR_FORWARD;
      rightFlag = MOTOR_FORWARD;
      break;
    case MOVE_BACKWARD:
      leftFlag = MOTOR_BACKWARD;
      rightFlag = MOTOR_BACKWARD;
      break;
    case SPIN_LEFT:
      leftFlag = MOTOR_BACKWARD;
      rightFlag = MOTOR_FORWARD;
      break;
    case TURN_LEFT:
      leftFlag = MOTOR_STOP;
      rightFlag = MOTOR_FORWARD;
      break;
    case TURN_LEFT_SLOW:
      leftFlag = MOTOR_FLOAT;
      rightFlag = MOTOR_FORWARD;
      break;
    case SPIN_RIGHT:
      leftFlag = MOTOR_FORWARD;
      rightFlag = MOTOR_BACKWARD;
      break;
    case TURN_RIGHT:
      leftFlag = MOTOR_FORWARD;
      rightFlag = MOTOR_STOP;
      break;
    case TURN_RIGHT_SLOW:
      leftFlag = MOTOR_FORWARD;
      rightFlag = MOTOR_FLOAT;
      break;
    case SLOW_DOWN:
      leftFlag = MOTOR_FLOAT;
      rightFlag = MOTOR_FLOAT;
      break;
    case WAIT:
      leftFlag = MOTOR_STOP;
      rightFlag = MOTOR_STOP;
      break;
  }
  raiseSignal(leftSignal);
  raiseSignal(rightSignal);

  setLcdNumber(LCD_SIGNED, timestamp, 0);
  refreshDisplay();

}

C_task void follow(void * ignore) {
  int16 leftSensor = sensor1;
  int16 optSensor = sensor3;
  int16 rightSensor = sensor2;
  int16 dt = 0;
  
  if (startFollow) {
   startFollow = 0;
   followState = 0;
   rightFlag = MOTOR_FORWARD;
   leftFlag = MOTOR_FLOAT;
   break;
  }

  if (leftSensor < -1) {              //Obstacle hit on left or both sensors
   state = OBSTACLE_LEFT;
  }
  else if (rightSensor < -1) {        //Obstacle hit on right sensor
   state = OBSTACLE_RIGHT;
  }
  else if(optSensor < -175){          //On white
   if (hitTrack) {
     hitTrack = 0;

     temp1 = rightFlag;
     rightFlag = leftFlag;
     leftFlag = temp1;

     temp1 = leftFlag;
     temp2 = rightFlag;
     leftFlag = MOTOR_STOP;
     rightFlag = MOTOR_STOP;
     raiseSignal(leftSignal);
     raiseSignal(rightSignal);
     leftFlag = temp1;
     rightFlag = temp2;
     break;
   }
  }
  else if(optSensor > -165){          //On track
   hitTrack = 1;
  }
  
  switch(followState) {
    case 0:
      stateTime = 100;
      break;
    case 1:
      
      break;
    case 2:
      
      break;
  }
  if (followCount == 5){
   if (rightFlag == MOTOR_FLOAT) rightFlag = MOTOR_STOP;
   else leftFlag = MOTOR_STOP;
  }
  if (followCount == 7) {
   if (rightFlag == MOTOR_STOP) rightFlag = MOTOR_FLOAT;
   else leftFlag = MOTOR_FLOAT;
   followCount = 0;
  }
  if (followCount == 2 || followCount == 10) {
   temp1 = leftFlag;
   temp2 = rightFlag;
   leftFlag = MOTOR_FLOAT;
   rightFlag = MOTOR_FLOAT;
   raiseSignal(leftSignal);
   raiseSignal(rightSignal);
   leftFlag = temp1;
   rightFlag = temp2;
  } else {
   raiseSignal(leftSignal);
   raiseSignal(rightSignal);
  }
}

C_task void search(void * ignore){
  int16 leftSensor = sensor1;
  int16 optSensor = sensor3;
  int16 rightSensor = sensor2;
  int16 dt = 0;
  
  if(searchType != WAIT && optSensor > -165){           //On track. End search
   searchState = 0;
   timestamp = 0;
   searchType = WAIT;
   leftFlag = MOTOR_STOP;
   rightFlag = MOTOR_STOP;
   raiseSignal(leftSignal);
   raiseSignal(rightSignal);
   state = FOLLOW;
   startFollow = 1;
  }
  else {                                                //Search
   dt = tick + seconds * 100 - timestamp;
   if (dt < 0) dt += 6000;
   switch(searchState) {
    case 0:                                             //Init
      searchType = WAIT;
      searchState = 0;
      timestamp = tick + seconds * 100;
      stateTime = 700;
      break;
     case 1: case 3: case 11:                           //Short forward
      if (dt > stateTime) {
        searchType = MOVE_FORWARD;
        searchState++;
        timestamp = tick + seconds * 100;
        stateTime = MOVE_TIME;
      }
      break;
     case 5: case 7: case 9:                            //Long forward
      if (dt > stateTime) {
        searchType = MOVE_FORWARD;
        searchState++;
        timestamp = tick + seconds * 100;
        stateTime = MOVE_TIME*2;
      }
      break;
    case 2: case 4: case 6: case 8: case 10:
      if (dt > stateTime) {                             //Turn
        searchType = SPIN_RIGHT;
        searchState++;
        timestamp = tick + seconds * 100;
        stateTime = 16;
      }
      break;
    case 12:
      if (dt > stateTime) {                             //Failed search
        leftFlag = MOTOR_STOP;
        rightFlag = MOTOR_STOP;
        raiseSignal(leftSignal);
        raiseSignal(rightSignal);
        searchState = 0;
      }
      break;
   }
   switch(searchType) {
    case MOVE_FORWARD:
      leftFlag = MOTOR_FORWARD;
      rightFlag = MOTOR_FORWARD;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
    case SPIN_RIGHT:
      leftFlag = MOTOR_FORWARD;
      rightFlag = MOTOR_BACKWARD;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
    case WAIT:
      leftFlag = MOTOR_STOP;
      rightFlag = MOTOR_STOP;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
   }
}
  
C_task void obstacle(void * ignore){
  dt = tick + seconds * 100 - timestamp;
  if (dt < 0) dt += 6000;

  switch(obstacleState) {
   case 0:                                          //Backward
      obstacleType = MOVE_BACKWARD;
      obstacleState++;
      timestamp = tick + seconds * 100;
      stateTime = 21;
     break;
   case 1:
      if (dt > stateTime) {                         //Turn left
        obstacleType = (state == OBSTACLE_LEFT ? SPIN_RIGHT : SPIN_LEFT);
        obstacleState++;
        timestamp = tick + seconds * 100;
        stateTime = 16;
      }
      break;
   case 2:
      if (dt > stateTime) {                        //Forward
        obstacleType = MOVE_FORWARD;
        obstacleState++;
        timestamp = tick + seconds * 100;
        stateTime = 41;
      }
      break;
   case 3:
      if (dt > stateTime) {                        //Turn right
        obstacleType = (state == OBSTACLE_LEFT ? SPIN_LEFT : SPIN_RIGHT);
        obstacleState++;
        timestamp = tick + seconds * 100;
        stateTime = 16;
      }
      break;
   case 4:
      if (dt > stateTime) {                        //Forward
        obstacleType = MOVE_FORWARD;
        obstacleState++;
        timestamp = tick + seconds * 100;
        stateTime = 66;
      }
      break;
   case 5:
      if (dt > stateTime) {                        //Turn right
        obstacleType = (state == OBSTACLE_LEFT ? SPIN_LEFT : SPIN_RIGHT);
        obstacleState++;
        timestamp = tick + seconds * 100;
        stateTime = 21;
      }
      break;
   case 6:
      if (dt > stateTime) {                        //Forward
        obstacleType = MOVE_FORWARD;
        obstacleState++;
      }
      break;
   case 7:                                         //Finding track
      if(optSensor > -165) {
        obstacleType = MOVE_FORWARD;
        obstacleState++;
        timestamp = tick + seconds * 100;
        stateTime = 6;
      }
      break;
   case 8:
      if (dt > stateTime) {                        //Forward
        obstacleType = (state == OBSTACLE_LEFT ? SPIN_RIGHT : SPIN_LEFT);
        obstacleState++;
        timestamp = tick + seconds * 100;
        stateTime = 11;
      }
   case 9:
      if (dt > stateTime) {
        obstacleState = 0;
        timestamp = 0;
        obstacleType = WAIT;
        startFollow = 1;

        state = FOLLOW;
      }
      break;
  }

  switch(obstacleType) {
    case MOVE_BACKWARD:
      leftFlag = MOTOR_BACKWARD;
      rightFlag = MOTOR_BACKWARD;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
    case MOVE_FORWARD:
      leftFlag = MOTOR_FORWARD;
      rightFlag = MOTOR_FORWARD;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
    case SPIN_LEFT:
      leftFlag = MOTOR_BACKWARD;
      rightFlag = MOTOR_FORWARD;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
    case SPIN_RIGHT:
      leftFlag = MOTOR_FORWARD;
      rightFlag = MOTOR_BACKWARD;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
    case WAIT:
      leftFlag = MOTOR_STOP;
      rightFlag = MOTOR_STOP;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
  }
  break;
}

C_task void motorLeft(void * ignore){
  setMotor(MOTOR_A, leftFlag);
}

C_task void motorRight(void * ignore){
  setMotor(MOTOR_C, rightFlag);
}







