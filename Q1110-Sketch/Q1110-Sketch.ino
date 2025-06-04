  #include <LiquidCrystal_I2C.h>

  #define pinSensorDC A1
  #define maxSensor 25       //volts
  #define maxAnalogica 1023  //valor máximo de leitura da porta analógica (Arduino Uno=1023  Esp32=4095)

  #define tensaoDeCorte 3.00  //tensão mínima para considerar que a bateria esta sendo carregada

  LiquidCrystal_I2C lcd(0x3F, 16, 2);


  int leituraSensorDC;
  unsigned long delayDisplay;

  void setup() {
    lcd.init();
    lcd.init();
    lcd.backlight();
  }


  void loop() {
    leituraSensorDC = analogRead(pinSensorDC);
    float tensaoSensor = (float(leituraSensorDC) * float(maxSensor)) / float(maxAnalogica);

    if ((millis() - delayDisplay) > 1000) {
      delayDisplay = millis();
      lcd.setCursor(0, 0);
      lcd.print(leituraSensorDC);
      lcd.setCursor(0, 1);
      lcd.print(tensaoSensor);

      lcd.setCursor(6, 0);
      if (tensaoSensor > tensaoDeCorte) {
        lcd.print("        ok");
      } else {
        lcd.print("BATERIA!!!");
      }
    }

    delay(20);
  }
