#include <Servo.h>
 
Servo myservo;

int sRtrig = 2; //sonar Right trig pin, 초음파 발생
int sRecho = 3; //echo 반사된 초음파 인식
int sLtrig = 4; //Left
int sLecho = 5; 
int PIR = 11; //PIR sensor
int pirState = LOW;
int pir_val = 0;
int Relay = 13;
int servo = 6;

int count = 0;

void setup() {
  Serial.begin(9600);
  //Sonar
  pinMode(sRtrig, OUTPUT);
  pinMode(sRecho, INPUT);
  pinMode(sLtrig, OUTPUT);
  pinMode(sLecho, INPUT);
  //Servo
  myservo.attach(servo);
  delay(15);
  myservo.write(0);
  delay(5000);
  //PIR
  pinMode(PIR, INPUT);
  //Relay
  pinMode(Relay, OUTPUT);
  delay(1000); 
}

  long microsecondsToCentimeters(long microseconds)
  {
    //음파의 속도는 340/m/s이므로 1cm이동에 약 29us가 들어감
    //왕복이므로 /2를 한번 더 한다
    return microseconds/29/2;
  }

  ///모터 동작 함수들
  int angle = 0;
  
  void left()
  {
    myservo.write(angle++);
    delay(15);
  }
  void right()
  {
    myservo.write(angle--);
    delay(15);
  }


  
void loop() {
  long durationL, durationR, cmL, cmR;
    
  digitalWrite(sRtrig, LOW);
  delayMicroseconds(sRtrig);
  digitalWrite(sRtrig, HIGH);
  delayMicroseconds(5);
  digitalWrite(sRtrig, LOW);

  durationR = pulseIn(sRecho, HIGH);
  if(sRecho == LOW){
    durationR = (long)pulseIn(sRecho, HIGH)/29/2;
  }
  else{
    pinMode(sRecho, OUTPUT);
    digitalWrite(sRecho, LOW);
    pinMode(sRecho, INPUT);
  }
 
 
  digitalWrite(sLtrig, LOW);
  delayMicroseconds(sLtrig);
  digitalWrite(sLtrig, HIGH);
  delayMicroseconds(5);
  digitalWrite(sLtrig, LOW);
 
  durationL = pulseIn(sLecho, HIGH);

//180416 PIR(인체감지센서) 추가
  pir_val = digitalRead(PIR); // 센서값 읽기
  if (pir_val == HIGH) { // 인체감지시
//      digitalWrite(ledPin, HIGH); // LED ON
      if (pirState == LOW) {
      Serial.println("Motion detected!");// 시리얼모니터에 메시지 출력
      pirState = HIGH;
      digitalWrite(Relay, LOW);
      }
  } else {
      if (pirState == HIGH){        
          // 시리얼모니터에 메시지 출력            
          Serial.println("Motion ended!");
          digitalWrite(Relay, HIGH);
          pirState = LOW;
        }
  }

/*171205
초음파센서가 저렴한 제품이라 값이 종종 튀어서 오동작을 할 때가 있음 => 하드웨어적 한계를 소프트웨어로 보완
일정 시간 간격으로 측정값을 평균내서, 그 값을 사용(값이 다소 바뀔 수 있지만, impulse로 인한 오동작을 잡을 수 있음
durationL_M
durationR_M
*/
    cmR = microsecondsToCentimeters(durationR);
    if(cmR >= 2000) cmR = 100;  //노이즈 제거
    else if (cmR >= 100) cmR -= 100;
    cmL = microsecondsToCentimeters(durationL);//측정된 시간 -> 거리
    if(cmL >= 2000) cmL = 100;       
    else if (cmL >= 100) cmL -= 100;        
            Serial.print("DistanceL = ");
            Serial.print(cmL);
            Serial.println("cm"); 
            delay(100);
            Serial.print("DistanceR = ");
            Serial.print(cmR);
            Serial.println("cm"); 
            delay(100);
  
    int boundary = 30;

    if((cmR <= boundary && cmL <= boundary) )//양쪽동시에 감지되면 가운데로, 180416 : PIR센서 추가
    {
      
      //왼쪽으로 놓여있으면 오른쪽으로, 오른쪽으로 놓여있으면 왼쪽으로
      do right();
      while(angle >= 90);
      do left();
      while(angle <= 90);
     }
    else if(cmR <= boundary)//오른쪽만 감지되면 45도
    {
      do right();
      while(angle >= 45);
    }
    else if(cmL <= boundary)//왼쪽만 감지되면 135도
    {
      do left();
      while(angle <= 135);
     }
    else////모두거짓이면 다시 90도
    {
      do right();
      while(angle >= 90);
      do left();
      while(angle <= 90);
    }

    /*
    if(cmR <= boundary && cmL <= boundary)//양쪽 센서 사이에 있으면
    {
      if(cmR == 100 || cmL == 100) myservo.write(90);
      else myservo.write(90 + 5*(cmR - cmL));
    }
    else if(cmR <= boundary && cmL >= boundary)//오른쪽에 있으면
    {
     myservo.write(45);
     delay(1000);
    }
    else if(cmR >= boundary && cmL <= boundary)//왼쪽에 있으면
    {
      myservo.write(135);
      delay(1000);
    }
    else//그 외
    {
      myservo.write(90);
      delay(1000);
    }
    */
    
  }

