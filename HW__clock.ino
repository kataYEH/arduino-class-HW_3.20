// 定義腳位
#define PIN_0 2
#define PIN_g 12
#define PIN_c 8
#define PIN_h 13
#define PIN_d 9
#define PIN_e 10
#define PIN_b 7
#define PIN_1 3
#define PIN_2 4
#define PIN_f 11
#define PIN_a 6
#define PIN_3 5

// 共有4個七段顯示器，分別由針腳PIN_0、PIN_1、PIN_2、PIN_3控制
// 七段顯示器裡有8個LED（包含小數點）
#define POS_NUM 4
#define SEG_NUM 8
const byte pos_pins[POS_NUM] = {PIN_0, PIN_1, PIN_2, PIN_3};
const byte seg_pins[SEG_NUM] = {PIN_a, PIN_b, PIN_c, PIN_d, PIN_e, PIN_f, PIN_g, PIN_h};
const byte SW = A0;
// 底下定義由七段顯示器顯示數字時所需要的資料
#define t true
#define f false
const boolean data[10][SEG_NUM] = {
  {t, t, t, t, t, t, f, f}, // 0
  {f, t, t, f, f, f, f, f}, // 1
  {t, t, f, t, t, f, t, f}, // 2
  {t, t, t, t, f, f, t, f}, // 3
  {f, t, t, f, f, t, t, f}, // 4
  {t, f, t, t, f, t, t, f}, // 5
  {t, f, t, t, t, t, t, f}, // 6
  {t, t, t, f, f, f, f, f}, // 7
  {t, t, t, t, t, t, t, f}, // 8
  {t, t, t, t, f, t, t, f}, // 9
};

// 一支方便的函式，以格式字串輸出到序列埠
void pf(const char *fmt, ... ){
    char tmp[128]; // max is 128 chars
    va_list args;
    va_start (args, fmt );
    vsnprintf(tmp, 128, fmt, args);
    va_end (args);
    Serial.print(tmp);
}

// 設定某個七段顯示器所顯示的數字，
// 參數pos為0~3，指出想要更新哪一個七段顯示器，
// 參數n為0~9，顯示數字
void setDigit(int pos, int n){
  if(pos < 0 || 3 < pos){
    pf("error pos=%d\n", pos);
    return;
  }

  // 控制想要更新哪一個七段顯示器，將其腳位設為LOW
  // 其他腳位則設為HIGH，代表不更新。 
  for(int p = 0; p < POS_NUM; p++){
    if(p == pos)
      digitalWrite(pos_pins[p], LOW);
    else
      digitalWrite(pos_pins[p], HIGH);
  }
  
  // 寫入數字 
  if(0 <= n && n <= 9){
    for(int i = 0; i < SEG_NUM; i++){
      digitalWrite(seg_pins[i], data[n][i] == t ? HIGH : LOW);
    }
  }
  else{
    for(int i = 0; i < SEG_NUM; i++){
      digitalWrite(seg_pins[i], LOW);
    }
    digitalWrite(PIN_h, HIGH);
    pf("error pos=%d, n=%d\n", pos, n);
  }
}

// 設定整個四合一型七段顯示器想要顯示的數字
// 參數number的範圍應是0~9999
void setNumber(int number,int hour,boolean val)
{
  int n0, n1, n2, n3;

  if(val)
    number = 0;
  n3 = hour / 10;
  
  n2 = hour % 10;
  
  n1 = number / 10;
  n0 = number % 10;

  // 求出每個位數的值後，分別更新
  // 注意，此處以delay(5)隔開每個位數的更新 
  setDigit(0, n0); delay(5);
  setDigit(1, n1); delay(5);
  setDigit(2, n2); delay(5);
  setDigit(3, n3); delay(5);
}

unsigned long time_previous;
void setup() {
  Serial.begin(115200);
   pinMode(SW,INPUT);
  for(int i = 0; i < POS_NUM; i++){
    pinMode(pos_pins[i], OUTPUT);
    digitalWrite(pos_pins[i], HIGH);
  }
  for(int i = 0; i < SEG_NUM; i++){
    pinMode(seg_pins[i], OUTPUT);
    digitalWrite(seg_pins[i], LOW);
  }
  
  time_previous = millis();
}

int number = 0;
void loop() {
  // 經過一秒後就讓number加1 
  boolean val = digitalRead(A0);
  unsigned long time_now = millis();
 static  int hour = 0; 
  
   if(time_now - time_previous > 100){
     {
          if(val)
            {
              number = 0;
              hour = 0;
             }


         if(number <60)
            number++;
         if(number == 60)
            {
              number = 0;
              if(hour <23)
                hour++;
              else
                hour = 0;
              }
         
     }
    time_previous += 100;
    pf("number=%d\n", number);
  }
 
  // 不斷地寫入數字
  setNumber(number,hour,val);
}
