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
	
	attachInterrupt(digitalPinToInterrupt(TURN_PIN),updateTurn,CHANGE);
	attachInterrupt(digitalPinToInterrupt(THROTTLE_PIN),updateThrottle,CHANGE);
	attachInterrupt(digitalPinToInterrupt(WEAPON_PIN),updateWeapon,CHANGE);
	Serial.begin(9600);
}

void loop(){

  static unsigned long int lastT = 0;

  static int leftTemp = 0;
  static int rightTemp = 0;
  
  if(millis()>lastT+20){
    Serial.println(millis()-lastT);
    lastT = millis();
    rightTemp = (curThrottle+curTurn);
    leftTemp = -(curThrottle-curTurn);
    
    if(leftTemp>400 || rightTemp>400){
      leftTemp =(int)(leftTemp*(400.0/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
      rightTemp =(int)(rightTemp*(400.0/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
    }

    if(leftTemp<-400 || rightTemp<-400){
      leftTemp =(int)(leftTemp*(400.0/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
      rightTemp =(int)(rightTemp*(400.0/ (abs(leftTemp)>abs(rightTemp)?abs(leftTemp):abs(rightTemp))));
    }
     
    digitalWrite(LEFT_OUT_PIN, HIGH);
    delayMicroseconds(1400+leftTemp);
    digitalWrite(LEFT_OUT_PIN,LOW);
    
    digitalWrite(RIGHT_OUT_PIN, HIGH);
    delayMicroseconds(1400+rightTemp);
    digitalWrite(RIGHT_OUT_PIN,LOW);
    
    digitalWrite(WEAPON_OUT_PIN, HIGH);
    delayMicroseconds(curWeapon);
    digitalWrite(WEAPON_OUT_PIN,LOW);

    Serial.print(leftTemp);
    Serial.print("   ");
    Serial.print(rightTemp);
    Serial.print("   ");  
    Serial.println(curWeapon);
  }
  
}

void updateTurn(){
	static unsigned long int start=0;
	if(digitalRead(TURN_PIN)){
		start = micros();
	}
	else{
		curTurn = micros()-start-1448;
    if(curTurn>400) curTurn = 400;
    else if(curTurn<-400) curTurn = -400;
	}
}
		
void updateThrottle(){
	static unsigned long int start=0;
	if(digitalRead(THROTTLE_PIN)){
		start = micros();
	}
	else{
		curThrottle = micros()-start-1480;
    if(curTurn>400) curTurn = 400;
    else if(curTurn<-400) curTurn = -400;
	}
}

void updateWeapon(){
	static unsigned long int start=0;
	if(digitalRead(WEAPON_PIN)){
		start = micros();
	}
	else{
		curWeapon = micros()-start;
	}
}
