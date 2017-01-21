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

#define BLACK_THRESHOLD -160
#define WHITE_THRESHOLD -175
#define MOVE_TIME 80
#define TURN_TIME 16
#define BACK_TIME 35

#define MOVE_FORWARD 0
#define MOVE_BACKWARD 1
#define TURN_LEFT 2
#define TURN_RIGHT 3
#define WAIT 4

static int8 programStarted = 1;

static uint8 tick = 0;
static uint8 seconds = 0;
static uint8 minutes = 0;
static uint8 hours = 0;

static int16 sensor1, sensor2, sensor3;
static int8 leftFlag = MOTOR_STOP, rightFlag = MOTOR_STOP;

static int16 searchTime = 0, obstacleTime = 0;
static int8 state = SEARCH, previousState;
static int8 searchState = 12, obstacleState = 0;
static int8 searchType = WAIT, obstacleType;

static int8 startFollow = 0, hitBlack = 0;

static int8 followCount = 0;




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

C_task void search(void * ignore){
  int16 leftSensor = sensor1;
  int16 optSensor = sensor3;
  int16 rightSensor = sensor2;
  int16 dt = 0;
  
  if(leftSensor < -1) {                            //obstacle hit on left or both sensors
   searchTime = 0;
   previousState = state;
   state = OBSTACLE_LEFT;
  }
  else if(rightSensor < -1) {                     //obstacle hit on right sensor
   searchTime = 0;
   previousState = state;
   state = OBSTACLE_RIGHT;
  }
  else if(searchType != WAIT && optSensor > -165){                       //Over black. End search
   searchState = 12;
   searchTime = 0;
   searchType = WAIT;
   leftFlag = MOTOR_STOP;
   rightFlag = MOTOR_STOP;
   raiseSignal(leftSignal);
   raiseSignal(rightSignal);
   state = FOLLOW;
   startFollow = 1;
  }
  else {                                          //Search
   dt = tick + seconds * 100 - searchTime;
   if (dt < 0) dt += 6000;
   switch(searchState){
    case 12:
      searchType = WAIT;
      searchState = 0;
      searchTime = tick + seconds * 100;
      break;
    case 0:
      if (dt > 750) {                                        //Init
        searchType = MOVE_FORWARD;
        searchState++;
        searchTime = tick + seconds * 100;
      }
      break;
    case 1: case 3:
      if (dt > MOVE_TIME) {                      //Turn
        searchType = TURN_RIGHT;
        searchState++;
        searchTime = tick + seconds * 100;
      }
      break;
    case 5: case 7: case 9:
      if (dt > MOVE_TIME*2) {                    //Turn
        searchType = TURN_RIGHT;
        searchState++;
        searchTime = tick + seconds * 100;
      }
      break;
    case 2: case 4: case 6: case 8: case 10:
      if (dt > 16) {                       //Forward
        searchType = MOVE_FORWARD;
        searchState++;
        searchTime = tick + seconds * 100;
      }
      break;
    case 11:
      if (dt > MOVE_TIME) {                     //Failed search
        leftFlag = MOTOR_STOP;
        rightFlag = MOTOR_STOP;
        raiseSignal(leftSignal);
        raiseSignal(rightSignal);
        searchState++;
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
    case TURN_RIGHT:
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


C_task void follow(void * ignore) {
  int16 leftSensor = sensor1;
  int16 optSensor = sensor3;
  int16 rightSensor = sensor2;
  int16 dt = 0;
  
  if (startFollow) {
   startFollow = 0;
   rightFlag = MOTOR_FORWARD;
   leftFlag = MOTOR_FLOAT;
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

  if(leftSensor < -1){          //obstacle hit on left or both sensors
   previousState = state;
   state = OBSTACLE_LEFT;
  }
  else if(rightSensor < -1) {   //obstacle hit on right sensor
   previousState = state;
   state = OBSTACLE_RIGHT;
  }
  else if(optSensor < WHITE_THRESHOLD){         //on white
   if (hitBlack) {
     hitBlack = 0;

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
  else if(optSensor > -165){         //on black
   hitBlack = 1;
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
  
C_task void obstacle(void * ignore){
  dt = tick + seconds * 100 - obstacleTime;
  if (dt < 0) dt += 6000;

  switch(obstacleState) {
   case 0:                                     //Backward
      obstacleType = MOVE_BACKWARD;
      obstacleState++;
      obstacleTime = tick + seconds * 100;
     break;
   case 1:
      if (dt > 21) {                         //Turn left
        obstacleType = (state == OBSTACLE_LEFT ? TURN_RIGHT : TURN_LEFT);
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;
   case 2:
      if (dt > 16) {                        //Forward
        obstacleType = MOVE_FORWARD;
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;
   case 3:
      if (dt > 41) {                      //Turn right
        obstacleType = (state == OBSTACLE_LEFT ? TURN_LEFT : TURN_RIGHT);
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;
   case 4:
      if (dt > 16) {                        //Forward
        obstacleType = MOVE_FORWARD;
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;
   case 5:
      if (dt > 66) {                      //Turn right
        obstacleType = (state == OBSTACLE_LEFT ? TURN_LEFT : TURN_RIGHT);
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;
   case 6:
      if (dt > 21) {                        //Forward
        obstacleType = MOVE_FORWARD;
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;
 /*case 7:
      if (dt > 50) {                      //Turn right
        obstacleType = (OBSTACLE_LEFT ? TURN_LEFT : TURN_RIGHT);
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;
   case 8:
      if (dt > TURN_TIME) {                      //Forward
        obstacleType = MOVE_FORWARD;
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;*/
   case 7:                                         //End sequence
      if(optSensor > -165) {
        obstacleType = MOVE_FORWARD;
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
      break;
   case 8:
      if (dt > 6) {                        //Forward
        obstacleType = (state == OBSTACLE_LEFT ? TURN_RIGHT : TURN_LEFT);
        obstacleState++;
        obstacleTime = tick + seconds * 100;
      }
   case 9:
      if (dt > 11) {
        obstacleState = 0;
        obstacleTime = 0;
        obstacleType = WAIT;
        startFollow = 1;

        state = previousState;
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
    case TURN_LEFT:
      leftFlag = MOTOR_BACKWARD;
      rightFlag = MOTOR_FORWARD;
      raiseSignal(leftSignal);
      raiseSignal(rightSignal);
      break;
    case TURN_RIGHT:
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
         dt = tick + seconds * 100 - obstacleTime;
         if (dt < 0) dt += 6000;

         switch(obstacleState) {
           case 0:                                     //Backward
              obstacleType = MOVE_BACKWARD;
              obstacleState++;
              obstacleTime = tick + seconds * 100;
             break;
           case 1:
              if (dt > 21) {                         //Turn left
                obstacleType = (state == OBSTACLE_LEFT ? TURN_RIGHT : TURN_LEFT);
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;
           case 2:
              if (dt > 16) {                        //Forward
                obstacleType = MOVE_FORWARD;
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;
           case 3:
              if (dt > 41) {                      //Turn right
                obstacleType = (state == OBSTACLE_LEFT ? TURN_LEFT : TURN_RIGHT);
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;
           case 4:
              if (dt > 16) {                        //Forward
                obstacleType = MOVE_FORWARD;
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;
           case 5:
              if (dt > 66) {                      //Turn right
                obstacleType = (state == OBSTACLE_LEFT ? TURN_LEFT : TURN_RIGHT);
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;
           case 6:
              if (dt > 21) {                        //Forward
                obstacleType = MOVE_FORWARD;
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;
           /*case 7:
              if (dt > 50) {                      //Turn right
                obstacleType = (OBSTACLE_LEFT ? TURN_LEFT : TURN_RIGHT);
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;
           case 8:
              if (dt > TURN_TIME) {                      //Forward
                obstacleType = MOVE_FORWARD;
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;*/
           case 7:                                         //End sequence
              if(optSensor > -165) {
                obstacleType = MOVE_FORWARD;
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
              break;
           case 8:
              if (dt > 6) {                        //Forward
                obstacleType = (state == OBSTACLE_LEFT ? TURN_RIGHT : TURN_LEFT);
                obstacleState++;
                obstacleTime = tick + seconds * 100;
              }
           case 9:
              if (dt > 11) {
                obstacleState = 0;
                obstacleTime = 0;
                obstacleType = WAIT;
                startFollow = 1;

                state = previousState;
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
            case TURN_LEFT:
              leftFlag = MOTOR_BACKWARD;
              rightFlag = MOTOR_FORWARD;
              raiseSignal(leftSignal);
              raiseSignal(rightSignal);
              break;
            case TURN_RIGHT:
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
    default:
         state = SEARCH;
         break;
         }
  }
  setLcdNumber(LCD_SIGNED, leftSensor, 0);
  //setLcdNumber(LCD_SIGNED, searchTime, 0);
  refreshDisplay();

}

C_task void motorLeft(void * ignore){
  setMotor(MOTOR_A, leftFlag);
}

C_task void motorRight(void * ignore){
  setMotor(MOTOR_C, rightFlag);
}







