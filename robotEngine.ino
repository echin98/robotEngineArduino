#define TURN_PIN 2
#define THROTTLE_PIN 3
#define WEAPON_PIN 7

#define LEFT_OUT_PIN 4
#define RIGHT_OUT_PIN 5
#define WEAPON_OUT_PIN 6

#define SLEW_LIMIT 10.0

void updateTurn();
void updateWeapon();
void updateThrottle();

volatile int curTurn=0;
volatile int curWeapon=0;
volatile int curThrottle=0;

void setup(){
	pinMode(TURN_PIN, INPUT);
	pinMode(THROTTLE_PIN, INPUT);
	pinMode(WEAPON_PIN, INPUT);

  pinMode(LEFT_OUT_PIN, OUTPUT);
  pinMode(RIGHT_OUT_PIN, OUTPUT);
  pinMode(WEAPON_OUT_PIN, OUTPUT);

  pinMode(10, OUTPUT);
	
	attachInterrupt(digitalPinToInterrupt(TURN_PIN),updateTurn,CHANGE);
	attachInterrupt(digitalPinToInterrupt(THROTTLE_PIN),updateThrottle,CHANGE);
	attachInterrupt(digitalPinToInterrupt(WEAPON_PIN),updateWeapon,CHANGE);
	Serial.begin(115200);
}

int leftTemp=1500;
int rightTemp=1500;
int weaponTemp=900;
unsigned long lastT = 0l;

void updateVals(){
  int rightTempOld = rightTemp;
  int leftTempOld = leftTemp;
  rightTemp = (curThrottle+curTurn);
  leftTemp = -(curThrottle-curTurn);
  
  if(leftTemp>400l || rightTemp>400l){
    leftTemp =(int)(leftTemp*(400.0l/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
    rightTemp =(int)(rightTemp*(400.0l/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
  }

  if(leftTemp<-400l || rightTemp<-400l){
    leftTemp =(int)(leftTemp*(400.0l/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
    rightTemp =(int)(rightTemp*(400.0l/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
  }

  weaponTemp = curWeapon<1400?curWeapon:1200;

  leftTemp += 1500l;
  rightTemp += 1500l;
  int leftSlew = leftTempOld-leftTemp;
  leftSlew = abs(leftSlew);
  int rightSlew = rightTempOld-rightTemp;
  rightSlew = abs(rightSlew);
  
  if((leftSlew>rightSlew?leftSlew:rightSlew)>SLEW_LIMIT){
    Serial.print((leftSlew>rightSlew?leftSlew:rightSlew));
    Serial.print("   ");
    Serial.print(rightTempOld);
    Serial.print(" slew limit exceeded ");
    leftTemp = (int)(leftTempOld+(SLEW_LIMIT*(leftTemp-leftTempOld)/(leftSlew>rightSlew?leftSlew:rightSlew)));
    rightTemp = (int)(rightTempOld+(SLEW_LIMIT*(rightTemp-rightTempOld)/(leftSlew>rightSlew?leftSlew:rightSlew)));
    Serial.println((leftTemp-leftTempOld));
  }
  
}

void loop(){
  

  unsigned long curMicros = micros();
  long tstamp = curMicros-lastT;

  static int test = LOW;
  digitalWrite(10,test);
  test = test==LOW?HIGH:LOW;
  
  if(tstamp>=20000){
    updateVals();
    lastT = curMicros;
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
  /*
  Serial.print(leftTemp);
  Serial.print("   ");
  Serial.print(rightTemp);
  Serial.print("   ");  
  Serial.println(curWeapon);
  */
}

void updateTurn(){
	static unsigned long start=0;
	if(digitalRead(TURN_PIN)){
		start = micros();
	}
	else{
		curTurn = micros()-start-1500;
	}
}
		
void updateThrottle(){
	static unsigned long start=0;
	if(digitalRead(THROTTLE_PIN)){
		start = micros();
	}
	else{
		curThrottle = micros()-start-1500;
	}
}

void updateWeapon(){
	static unsigned long start=0;
	if(digitalRead(WEAPON_PIN)){
		start = micros();
	}
	else{
		curWeapon = micros()-start;
	}
}
