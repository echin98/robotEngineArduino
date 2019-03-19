#define TURN_PIN 2
#define THROTTLE_PIN 3
#define WEAPON_PIN 7

#define LEFT_OUT_PIN 4
#define RIGHT_OUT_PIN 5
#define WEAPON_OUT_PIN 6

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

int leftTemp;
int rightTemp;
unsigned long lastT = 0l;

void updateVals(){
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

  leftTemp += 1400l;
  rightTemp += 1400l;
}

void loop(){

  leftTemp = 0l;
  rightTemp = 0l;
  
  int leftState = LOW;
  int rightState = LOW;
  int weaponState = LOW;
  
  updateVals();

  unsigned long curMicros = micros();
  lastT = curMicros;
  long tstamp = curMicros-lastT;

  static int test = LOW;
  digitalWrite(10,test);
  test = test==LOW?HIGH:LOW;
  
  while(tstamp<20000){
    curMicros = micros();
    tstamp = curMicros-lastT;
    Serial.println(tstamp);

    if(tstamp<leftTemp)
      leftState = HIGH;
    else
      leftState = LOW;
      
    if(tstamp<rightTemp)
      rightState = HIGH;
    else
      rightState = LOW;
    
    if(tstamp<curWeapon)
      weaponState = HIGH;
    else
      weaponState = LOW;
    
    digitalWrite(LEFT_OUT_PIN, leftState);
    digitalWrite(RIGHT_OUT_PIN, rightState);
    digitalWrite(WEAPON_OUT_PIN, weaponState);
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
		curTurn = micros()-start-1448;
	}
}
		
void updateThrottle(){
	static unsigned long start=0;
	if(digitalRead(THROTTLE_PIN)){
		start = micros();
	}
	else{
		curThrottle = micros()-start-1480;
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
