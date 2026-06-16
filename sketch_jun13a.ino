#define ENA 18  // 右輪速度控制
#define ENB 19  // 左輪速度控制

#define IN1 4
#define IN2 5
#define IN3 21
#define IN4 22

#define S1 13
#define S2 14
#define S3 27
#define S4 26
#define S5 25

// ===== RGB 狀態指示燈腳位定義 ===== 
#define LED_RED 23   // 🔴 紅燈接 Pin 23 
#define LED_GREEN 16 // 🟢 綠燈接 Pin 16 
#define LED_BLUE 15  // 🔵 藍燈接 Pin 15 

//================================================
// 🎯 黃金參數調校區
//================================================
#define SPEED_FORWARD_L 255  // 直行左輪 
#define SPEED_FORWARD_R 225  // 直行右輪 

#define SPEED_TURN_SLOW 140  // 正常弧度彎：慢輪速度 (維持雙輪向前，直線才穩)
#define SPEED_TURN_FAST 255  // 正常弧度彎：快輪速度

int lastDirection = 0; // -1 = 左, 0 = 中, +1 = 右
int turnLock = 0;      // 💡 轉彎鎖定：-1 = 鎖定大左轉, 0 = 無鎖定, 1 = 鎖定大右轉

//================================================
// 💡 RGB 燈光切換控制函式
//================================================
void setLED(bool red, bool green, bool blue) {
  digitalWrite(LED_RED, red);
  digitalWrite(LED_GREEN, green);
  digitalWrite(LED_BLUE, blue);
}

//================================================
// PWM速度控制
//================================================
void setMotor(int leftSpeed, int rightSpeed)
{
  leftSpeed  = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  ledcWrite(ENB, leftSpeed);   
  ledcWrite(ENA, rightSpeed);  
}

//================================================
// 動作控制函式 (小彎恢復為雙輪向前弧度轉，大彎維持原地旋轉)
//================================================
void forward()
{
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotor(SPEED_FORWARD_L, SPEED_FORWARD_R); 
}

void left()
{
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); // 🟢 雙輪向前弧度彎，保證直線穩定度
  setMotor(SPEED_TURN_SLOW, 250); 
}

void right()
{
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); // 🟢 雙輪向前弧度彎，保證直線穩定度
  setMotor(SPEED_FORWARD_L, SPEED_TURN_SLOW); 
}

void sharpLeft()
{
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  // 右輪前進
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); // 左輪後退
  setMotor(255, 255); 
}

void sharpRight()
{
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); // 右輪後退
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  // 左輪前進
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

  // ─── 💡 核心修正：大彎智慧鎖定檢查 ───
  if (turnLock == -1) {
    if (s3 == 0 || s2 == 0) {
      turnLock = 0; 
    } else {
      setLED(HIGH, LOW, LOW); // 🔴 繼續保持紅燈
      sharpLeft();
      delay(5);
      return; 
    }
  }
  else if (turnLock == 1) {
    if (s3 == 0 || s4 == 0) {
      turnLock = 0; 
    } else {
      setLED(HIGH, LOW, LOW); // 🔴 繼續保持紅燈
      sharpRight();
      delay(5);
      return; 
    }
  }

  // ─── 正常循跡邏輯判斷 ───

  if (s1 == 0 && s2 == 0 && s3 == 0 && s4 == 0 && s5 == 0)
  {
    setLED(LOW, HIGH, LOW); // 🟢 安全狀態（衝線停車亮綠燈）
    stopCar();
    lastDirection = 0;      
    turnLock = 0;
  }
  else if (s1 == 0)
  {
    setLED(HIGH, LOW, LOW); // 🔴 大左轉亮紅燈
    sharpLeft();
    delay(20);              // ⏱️ 這裡的 20ms 會完美維持紅燈
    lastDirection = -1;
    turnLock = -1;          
  }
  else if (s5 == 0)
  {
    setLED(HIGH, LOW, LOW); // 🔴 大右轉亮紅燈
    sharpRight();
    delay(20);
    lastDirection = 1;
    turnLock = 1;           
  }
  else if (s2 == 0)
  {
    setLED(HIGH, LOW, LOW); // 🔴 小左轉亮紅燈
    left();
    lastDirection = -1;
  }
  else if (s4 == 0)
  {
    setLED(HIGH, LOW, LOW); // 🔴 小右轉亮紅燈
    right();
    lastDirection = 1;
  }
  else if (s3 == 0)
  {
    setLED(HIGH, LOW, LOW); // 🔴 直行亮紅燈
    forward();
    lastDirection = 0;
  }
  // ─── 🛠️ 關鍵修正：找不到線的救援盲區 ───
  else
  {
    if (lastDirection == -1) {
      setLED(HIGH, LOW, LOW); // 🔥 修正：左轉未完成掉入盲區時，拒絕亮藍燈！繼續亮紅燈 🔴
      sharpLeft();  
    }
    else if (lastDirection == 1) {
      setLED(HIGH, LOW, LOW); // 🔥 修正：右轉未完成掉入盲區時，拒絕亮藍燈！繼續亮紅燈 🔴
      sharpRight(); 
    }
    else {
      setLED(LOW, HIGH, LOW); // 🟢 真正完全迷路停下時才亮綠燈
      stopCar();    
    }
  }

  delay(5);
}