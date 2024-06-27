#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1                  // Wemos TTGO ESP8266 with 0.91 Inch OLED version A type
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include  "My_Arduino_GPIO_Lib_V1_6.h"



const int motor_pin1 = 6;
const int motor_pin2 = 7;
const int motor_speed_pin = 5;
const int trig_pin = 9;           // 초음파 센서 트리거 핀
const int echo_pin = 8;           // 초음파 센서 에코 핀
const int button_pin = 4;         // 버튼 핀
const int mode_button_pin = 2;    // 모드 전환 버튼 핀
const int ir_recv_pin = 14;      // IR 리시버 핀

// 모터 속도 변수
int motor_speed = 0;
int speed_levels[3] = {100, 150, 255}; // 각 단계별 모터 속도 (1단계, 2단계, 3단계)

// 버튼 상태 변수
int button1_state = 0;
int mode_button2_state = 0;
int button_press_count = 0;

// 초음파 센서 관련 변수
long duration;
int distance;
int distance_levels[3] = {10, 30, 50}; 

// 모드 상태 변수
bool is_auto_mode = true;  // 기본값은 자동 모드

// IR 리모콘 관련 변수
IRrecv irrecv(ir_recv_pin);
decode_results results;

void setup() {
  // put your setup code here, to run once:
  pinMode(motor_pin1, OUTPUT);        // 모터 제어 핀을 출력으로 설정
  pinMode(motor_pin2, OUTPUT);       
  pinMode(motor_speed_pin, OUTPUT);      
  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, LOW); 
  pinMode(button_pin, INPUT_PULLUP);        // 버튼 핀을 입력으로 설정      _PULLUP
  pinMode(mode_button_pin, INPUT_PULLUP);   // 모드 전환 버튼 핀을 입력으로 설정
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  attachInterrupt(0, chk_mode_irq, RISING);
  Serial.begin(115200);
  
  irrecv.enableIRIn();   //시작함수lib

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
   {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
   } 
  delay(2000); // Pause for 2 seconds

}

void chk_mode_irq()
{
  //Serial.println("mode change?");
  mode_button2_state = d_in(mode_button_pin);
  // 모드 전환 버튼이 눌리면
  if (mode_button2_state == LOW) {
    delay(50);  // 디바운스를 위한 짧은 딜레이
    is_auto_mode = !is_auto_mode;  // 모드 전환
    displayMode(); // OLED에 현재 모드 표시
    delay(300);  // 버튼이 눌린 후의 딜레이
    Serial.println("mode change");
  }
}

void chk_mode()
{
  //Serial.println(F("mode chk start"));
  // 모드 전환 버튼 상태 확인
  chk_mode_irq();

}

// 초음파 센서를 사용하여 거리를 측정하는 함수
int measureDistance() {
  // 초음파 신호를 보냄
  d_out(trig_pin, LOW); 
  delayMicroseconds(2);
  d_out(trig_pin, HIGH);   //digital out.   digitalWrite()매크로함수.
  delayMicroseconds(10);  //delay(ms) micro -> 10^-6, m -> 10^-2
  d_out(trig_pin, LOW);           //high(1)로 올렸다가 10Ms 지난 후 다시 low(0)로 떨굼. start 신호. 단발

  long distance = pulseIn(echo_pin, HIGH) / 58;  // pulseIn() : 아두이노 제공 타이머 함수
  Serial.println(F("measure distance"));
  return distance;
}

//자동모드 실행 함수
void mode_ultra()
{
  distance = measureDistance(); // 초음파 센서 값 읽기
    if (distance <= distance_levels[0]) {
      motor_speed = speed_levels[0]; // 1단계 속도 설정
    } else if (distance >= distance_levels[0] && distance < distance_levels[1]) {
      motor_speed = speed_levels[1]; // 2단계 속도 설정
    } else {
      motor_speed = speed_levels[2]; // 3단계 속도 설정
    }
    
    analogWrite(motor_speed_pin, motor_speed);
    Serial.println(distance);
    Serial.println(motor_speed);
}

void mode_hand()
{
    button1_state = d_in(button_pin);
    if (button1_state == 0) {
      Serial.println(F("push button"));
      delay(50);  // 디바운스를 위한 짧은 딜레이
      // 버튼 누름 카운트 증가
      button_press_count++;
      
      Serial.println(button_press_count);
      
      // 버튼 누름 카운트에 따라 속도 설정 및 모터 제어
      if (button_press_count <= 3) {
        motor_speed = speed_levels[button_press_count - 1];
        
        analogWrite(motor_speed_pin, motor_speed);
        //Serial.println(motor_speed);
        
      } else if (button_press_count == 4) {
        // 전원 꺼짐
        
        analogWrite(motor_speed_pin, 0);
        button_press_count = 0; // 버튼 누름 카운트 초기화
        Serial.println(F("turn off"));
      }
      
      // 버튼 누름 후 잠시 대기
      delay(300); // 버튼이 눌린 후의 딜레이
    }
}

void mode_remote()
{
   if (irrecv.decode(&results)) {
      unsigned long int decCode = results.value;

      switch (decCode) {
        case 0xFF6897: // 0번 버튼: 종료
          
          analogWrite(motor_speed_pin, motor_speed);
          button_press_count = 0; // 버튼 누름 카운트 초기화
          displaySpeedLevel(0); // OLED에 종료 메시지 출력
          Serial.println(F("remote button 0 off"));
          break;

        case 0xFF30CF: // 1번 버튼: 1단계 속도 설정
          motor_speed = speed_levels[0];
          
          analogWrite(motor_speed_pin, motor_speed);
          displaySpeedLevel(1); // OLED에 1단계 출력
          Serial.println(F("remote mode 1"));
          break;

        case 0xFF18E7: // 2번 버튼: 2단계 속도 설정
          motor_speed = speed_levels[1];
          
          analogWrite(motor_speed_pin, motor_speed);
          displaySpeedLevel(2); // OLED에 2단계 출력
          Serial.println(F("remote mode 2"));
          break;

        case 0xFF7A85: // 3번 버튼: 3단계 속도 설정
          motor_speed = speed_levels[2];
          
          analogWrite(motor_speed_pin, motor_speed);
          displaySpeedLevel(3); // OLED에 3단계 출력
          Serial.println(F("remote mode 3"));
          break;

        default:
          break;
      }

      irrecv.resume(); // 다음 신호 수신을 위해 리시버 재시작
    }
}

// OLED에 속도 단계 출력 함수
void displaySpeedLevel(int level) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print(level);
  display.display();
}

// OLED에 현재 모드 출력 함수
void displayMode() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(is_auto_mode ? "Auto Mode" : "Manual Mode");
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  chk_mode(); //자동 or 수동

  if (is_auto_mode) mode_ultra();
  else //수동모드에서는 버튼과 리모콘을 통한 제어
  {
    mode_hand();
    mode_remote();
  }
}














