//pin definitions
#define TURN_PIN 2
#define THROTTLE_PIN 3
#define WEAPON_PIN 7

#define LEFT_OUT_PIN 4
#define RIGHT_OUT_PIN 5
#define WEAPON_OUT_PIN 6

//slew rate limit, used as an acceleration limit on the wheels
//might be a good idea to do this for weapon too, but not implemented
#define SLEW_LIMIT 10.0

//function declarations for interrupt functions
void updateTurn();
void updateWeapon();
void updateThrottle();

//variables for storing the PWM values from ISRs
volatile int curTurn=0;
volatile int curWeapon=0;
volatile int curThrottle=0;

//used for timing PWM pulses
volatile unsigned long lastTurnTime=0;
volatile unsigned long lastThrottleTime=0;
volatile unsigned long lastWeaponTime=0;

//set up PWM delays
int leftTemp=1500;
int rightTemp=1500;
int weaponTemp=900;

//used for timing the PWM loop
unsigned long lastT = 0l;

void setup(){
  //pinmodes
	pinMode(TURN_PIN, INPUT);
	pinMode(THROTTLE_PIN, INPUT);
	pinMode(WEAPON_PIN, INPUT);

  pinMode(LEFT_OUT_PIN, OUTPUT);
  pinMode(RIGHT_OUT_PIN, OUTPUT);
  pinMode(WEAPON_OUT_PIN, OUTPUT);
	
	attachInterrupt(digitalPinToInterrupt(TURN_PIN),updateTurn,CHANGE);
	attachInterrupt(digitalPinToInterrupt(THROTTLE_PIN),updateThrottle,CHANGE);
	attachInterrupt(digitalPinToInterrupt(WEAPON_PIN),updateWeapon,CHANGE);
	Serial.begin(115200);
}


//slew limiting and capping values
void updateVals(){
  //store the previous output value for slew limiting
  int rightTempOld = rightTemp;
  int leftTempOld = leftTemp;

  //get the base left and right values
  rightTemp = (curThrottle+curTurn);
  leftTemp = -(curThrottle-curTurn);

  //limit to +/- 400
  if(leftTemp>400l || rightTemp>400l){
    leftTemp =(int)(leftTemp*(400.0l/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
    rightTemp =(int)(rightTemp*(400.0l/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
  }

  if(leftTemp<-400l || rightTemp<-400l){
    leftTemp =(int)(leftTemp*(400.0l/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
    rightTemp =(int)(rightTemp*(400.0l/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
  }

  //set value for weapon output
  weaponTemp = curWeapon<1400?curWeapon:1700;

  //center sides on the PWM range
  leftTemp += 1500l;
  rightTemp += 1500l;

  //find the slew
  int leftSlew = leftTempOld-leftTemp;
  leftSlew = abs(leftSlew);
  int rightSlew = rightTempOld-rightTemp;
  rightSlew = abs(rightSlew);

  //if it's excessive, limit it to SLEW_LIMIT
  if((leftSlew>rightSlew?leftSlew:rightSlew)>SLEW_LIMIT){
    leftTemp = (int)(leftTempOld+(SLEW_LIMIT*(leftTemp-leftTempOld)/(leftSlew>rightSlew?leftSlew:rightSlew)));
    rightTemp = (int)(rightTempOld+(SLEW_LIMIT*(rightTemp-rightTempOld)/(leftSlew>rightSlew?leftSlew:rightSlew)));
  }
  
}

void loop(){
  //values used on every loop
  unsigned long curMicros = micros();
  bool disabled = true;
  long tstamp = curMicros-lastT;

  //failsafe, checks to make sure that we have received a pulse from the radio in the last 40000 uS
  if(curMicros-lastTurnTime > 40000 || 
    curMicros-lastThrottleTime > 40000 ||
    curMicros-lastWeaponTime > 40000){
    disabled = true;
  }
  else{
    disabled = false;
  }

  //make the PWM pulses, but don't if the robot is disabled by the failsafe
  if(disabled){
    digitalWrite(LEFT_OUT_PIN,LOW);
    digitalWrite(RIGHT_OUT_PIN,LOW);
    digitalWrite(WEAPON_OUT_PIN,LOW);
  }
  else if(tstamp>=20000){
    //reset the timer
    lastT = curMicros;
    
    //updateVals needs to be here for slew limiting to work because it needs to be timed
    updateVals();

    //Write the PWM pulses.
    //note: This must be done with delays to preserve accuracy. The loop is not fast enough to do them any other way.
    digitalWrite(LEFT_OUT_PIN,HIGH);
    delayMicroseconds(leftTemp);
    digitalWrite(LEFT_OUT_PIN, LOW);
    
    digitalWrite(RIGHT_OUT_PIN,HIGH);
    delayMicroseconds(rightTemp);
    digitalWrite(RIGHT_OUT_PIN, LOW);
    
    digitalWrite(WEAPON_OUT_PIN,HIGH);
    delayMicroseconds(weaponTemp);
    digitalWrite(WEAPON_OUT_PIN, LOW);
    
  }
}

//ISRs
void updateTurn(){
	if(digitalRead(TURN_PIN)){
		lastTurnTime = micros();
	}
	else{
		curTurn = micros()-lastTurnTime-1500;
	}
}
		
void updateThrottle(){
	if(digitalRead(THROTTLE_PIN)){
		lastThrottleTime = micros();
	}
	else{
		curThrottle = micros()-lastThrottleTime-1500;
	}
}

void updateWeapon(){
	if(digitalRead(WEAPON_PIN)){
		lastWeaponTime = micros();
	}
	else{
		curWeapon = micros()-lastWeaponTime;
	}
}
