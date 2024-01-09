#include <M5StickC.h>

void setup() {
  M5.begin();
  M5.Lcd.setRotation(2);
  M5.Lcd.setTextSize(2);
  // シリアル出力の初期化
  Serial.begin(115200);
  delay(50);

  // アナログ入力モードに設定
  pinMode(26, ANALOG);
}

void loop() {
  M5.Lcd.fillScreen(BLACK);  // 画面をクリア
  // 状態を出力して1秒ウエイト
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(analogRead(26));
  Serial.println(analogRead(26));
  delay(1000);
}