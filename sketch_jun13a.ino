#define ENA 18
#define ENB 19

#define IN1 4
#define IN2 5
#define IN3 21
#define IN4 22

#define S1 13
#define S2 14
#define S3 27
#define S4 26
#define S5 25

#define LED_RED 23
#define LED_GREEN 16
#define LED_BLUE 15

#define SPEED_FORWARD_L 255
#define SPEED_FORWARD_R 225

#define SPEED_TURN_SLOW 140
#define SPEED_TURN_FAST 255

int lastDirection = 0;
int turnLock = 0;

void setLED(bool red, bool green, bool blue) {
  digitalWrite(LED_RED, red);
  digitalWrite(LED_GREEN, green);
  digitalWrite(LED_BLUE, blue);
}

void setMotor(int leftSpeed, int rightSpeed)
{
  leftSpeed  = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  ledcWrite(ENB, leftSpeed);   
  ledcWrite(ENA, rightSpeed);  
}

void forward()
{
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotor(SPEED_FORWARD_L, SPEED_FORWARD_R); 
}

void left()
{
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotor(SPEED_TURN_SLOW, 250); 
}

void right()
{
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotor(SPEED_FORWARD_L, SPEED_TURN_SLOW); 
}

void sharpLeft()
{
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  setMotor(255, 255); 
}

void sharpRight()
{
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotor(255, 255); 
}

void stopCar()
{
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  setMotor(0, 0);
}

void setup()
{
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);
  pinMode(S4, INPUT_PULLUP);
  pinMode(S5, INPUT_PULLUP);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  
  setLED(LOW, LOW, HIGH); 

  Serial.begin(115200);

  ledcAttach(ENA, 1000, 8);
  ledcAttach(ENB, 1000, 8);

  stopCar();
  delay(1000);
}

void loop()
{
  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);
  int s4 = digitalRead(S4);
  int s5 = digitalRead(S5);

  Serial.print(s1); Serial.print(" ");
  Serial.print(s2); Serial.print(" ");
  Serial.print(s3); Serial.print(" ");
  Serial.print(s4); Serial.print(" ");
  Serial.println(s5);

  if (turnLock == -1) {
    if (s3 == 0 || s2 == 0) {
      turnLock = 0; 
    } else {
      setLED(HIGH, LOW, LOW);
      sharpLeft();
      delay(5);
      return; 
    }
  }
  else if (turnLock == 1) {
    if (s3 == 0 || s4 == 0) {
      turnLock = 0; 
    } else {
      setLED(HIGH, LOW, LOW);
      sharpRight();
      delay(5);
      return; 
    }
  }

  if (s1 == 0 && s2 == 0 && s3 == 0 && s4 == 0 && s5 == 0)
  {
    setLED(LOW, HIGH, LOW);
    stopCar();
    lastDirection = 0;      
    turnLock = 0;
  }
  else if (s1 == 0)
  {
    setLED(HIGH, LOW, LOW);
    sharpLeft();
    delay(20);
    lastDirection = -1;
    turnLock = -1;          
  }
  else if (s5 == 0)
  {
    setLED(HIGH, LOW, LOW);
    sharpRight();
    delay(20);
    lastDirection = 1;
    turnLock = 1;           
  }
  else if (s2 == 0)
  {
    setLED(HIGH, LOW, LOW);
    left();
    lastDirection = -1;
  }
  else if (s4 == 0)
  {
    setLED(HIGH, LOW, LOW);
    right();
    lastDirection = 1;
  }
  else if (s3 == 0)
  {
    setLED(HIGH, LOW, LOW);
    forward();
    lastDirection = 0;
  }
  else
  {
    if (lastDirection == -1) {
      setLED(HIGH, LOW, LOW);
      sharpLeft();  
    }
    else if (lastDirection == 1) {
      setLED(HIGH, LOW, LOW);
      sharpRight(); 
    }
    else {
      setLED(LOW, HIGH, LOW);
      stopCar();    
    }
  }

  delay(5);
}
