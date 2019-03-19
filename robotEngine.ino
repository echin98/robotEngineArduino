#define TURN_PIN 2
#define THROTTLE_PIN 3
#define WEAPON_PIN 7

#define LEFT_OUT_PIN 1
#define RIGHT_OUT_PIN 2
#define WEAPON_OUT_PIN 3

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
	
	attachInterrupt(digitalPinToInterrupt(TURN_PIN),updateTurn,CHANGE);
	attachInterrupt(digitalPinToInterrupt(THROTTLE_PIN),updateThrottle,CHANGE);
	attachInterrupt(digitalPinToInterrupt(WEAPON_PIN),updateWeapon,CHANGE);
	Serial.begin(9600);
}

void loop(){

	Serial.print(curTurn);
	Serial.print("   ");
	Serial.print(curThrottle);
	Serial.print("   ");	
	Serial.println(curWeapon);
}

void updateTurn(){
	static unsigned long int start=0;
	if(digitalRead(TURN_PIN)){
		start = micros();
	}
	else{
		curTurn = micros()-start;
	}
}
		
void updateThrottle(){
	static unsigned long int start=0;
	if(digitalRead(THROTTLE_PIN)){
		start = micros();
	}
	else{
		curThrottle = micros()-start;
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
