
#include "Free_Fonts.h"
#include <TFT_eSPI.h>
#include <SPI.h>

byte HEIGHT                   = 0;                                //Объявляем переменные
byte WIDGHT                   = 0;
int DATA[161];
int MAX = 0, SAVE_MAX         = 0;
byte NUMBER                   = 0;
unsigned long TIME            = 0;
unsigned long TIME_PULSE      = 0;

unsigned long SUMM            = 0;
unsigned long SUMM_COUNT      = 0;


TFT_eSPI TFT = TFT_eSPI();

void setup(void) {
  pinMode(16, INPUT_PULLUP);
  
  TFT.begin();
  TFT.setRotation(3);                                               //Положение экрана определяется цифрами 1 или 3, но после переворачивания нужно сделать калибровку сенсора
  TFT.fillScreen(TFT_BLACK);                                        //Закрашиваем экран чёрным цветом
  
  HEIGHT = TFT.height();
  WIDGHT = TFT.width();
  pinMode(2, OUTPUT);

}

void loop() {
  if (millis() - TIME_PULSE >= 4)                                   //Добавляем задержку в 4 миллисекунды
  {
    digitalWrite(2,HIGH);                                           //Подаём высокий уровень сигнала на пин D4 
    if (millis() - TIME_PULSE >= 5)                                 //Добавляем задержку в 5 миллисекунды
    {
      digitalWrite(2,LOW);                                          //Подаём низкий уровень сигнала на пин D4
      if (millis() - TIME_PULSE >= 40)                              //Добавляем задержку в 40 миллисекунды
        TIME_PULSE = millis();                                      //Обнуляем таймер
      }
    }
    
    DATA[NUMBER]        = pulseIn(4, HIGH);                         //Добавляем в массив значение функции подсчёта днительности импульса

    if (NUMBER == 160){                                             //Если произошло превышение длины массива, то сдвигаем все значения влево.
        for (byte i = 0; i < NUMBER; ++i) {
            DATA[i]      = DATA[i + 1];
          }
        --NUMBER;
      }
     ++NUMBER;
    MAX = 0;
    for (byte i = 0; i < NUMBER; ++i)
      MAX                 = max(MAX, DATA[i]);                      //Получаем максимальное значение массива
    if (MAX != SAVE_MAX)
    {
      TFT.fillScreen(TFT_BLACK);                                    //Закрашиваем экран чёрным цветом
      TFT.setTextColor(TFT_GREEN, TFT_BLACK);                       //Устанавливаем цвет текста
      TFT.drawRightString("Signal:", 50, 0, 2);                     //Выводим статистические данные
      TFT.drawRightString("us", 160, 0, 2);
    }
    SAVE_MAX    = MAX;                                              //Сохраняем максимальное значение массива

    ++SUMM_COUNT;
    SUMM += DATA[NUMBER];
    analogWrite(5, MAPFLOAT(DATA[NUMBER], 0, MAX, 0, 255));         //Подаём сигнал на спикер
    if (millis() - TIME >= 100)                                     //Добавляем задержку в 100 миллисекунд
    {
      TFT.setTextColor(TFT_GREEN, TFT_BLACK);                       //Устанавливаем цвет текста
      TFT.drawRightString(DISPLAY_TEXT(SUMM / SUMM_COUNT, 9, 0), 142, 0, 2);  //"Текст", положение по оси Х, положение по оси Y, размер шрифта
      TIME = millis();
      SUMM_COUNT = 0;
      SUMM = 0;
    }
    BACKGROUND();
    for (byte i = 9; i < NUMBER; ++i) {                             
      TFT.drawLine(i, MAPFLOAT(DATA[i-2], 0, MAX, HEIGHT, 18), i, MAPFLOAT(DATA[i-1], 0, MAX, HEIGHT, 18), ST7735_BLACK); //Закрашиваем предыдущий график чёрным цветом
      TFT.drawLine(i, MAPFLOAT(DATA[i-1], 0, MAX, HEIGHT, 18), i, MAPFLOAT(DATA[i], 0, MAX, HEIGHT, 18), ST7735_WHITE);   //Рисуем график на экране
    }
}

int MAPFLOAT(int VALUE, int MININ, int MAXIN, byte MINOUT, byte MAXOUT) { //Функия аналог MAP
  return (VALUE - MININ) * (MAXOUT - MINOUT) / (MAXIN - MININ) + MINOUT;
}


static char* DISPLAY_TEXT(float DATA, byte COUNT, byte FLOAT)       //Функция затирает предыдущие показания пробелами
{
  static char DATA_RESULT[20];                                      //Объявляем переменную
  char DATA_DISPLAY[20];                                            //Объявляем переменную
  dtostrf(DATA, COUNT, FLOAT, DATA_DISPLAY);                        //Конвертируем показания в привычные глазу данные для дальнейшего вывода на экран

  byte LEN = strlen(DATA_DISPLAY);                                  //Узнаём длину полученных данных

  for (byte i = 0; i < COUNT - (LEN - 1); ++i)                      //Вычисляем сколько пробелов не хватает
  {
    strcpy(DATA_RESULT, " ");                                       //Создаём строку из недостающих пробелов
  }
  strcat(DATA_RESULT, DATA_DISPLAY);                                //Добавляем недостающие пробелы

  return DATA_RESULT;                                               //Отдаём результат
}

void BACKGROUND() {                                                 //Рисуем сетку
  for (byte x = 9; x <= WIDGHT; x += 5) {
      for (byte y = 36; y < HEIGHT; y += 18) {
        TFT.drawFastHLine(x, y, 2, ST7735_YELLOW);
      }


    if ((x - 9) % 25 == 0) {
      TFT.drawFastHLine((x == 9 ? x : (x + 1 == WIDGHT ? x - 2 : x - 1)), 18, 3, ST7735_YELLOW);
      TFT.drawFastHLine((x == 9 ? x : (x + 1 == WIDGHT ? x - 2 : x - 1)), HEIGHT, 3, ST7735_YELLOW);
    }
  }

  for (byte x = 9; x <= WIDGHT; x += 25) {
    for (byte y = 18; y < HEIGHT; y += 5) {
      TFT.drawFastVLine(x, y, 2, ST7735_YELLOW);
    }
  }
}
