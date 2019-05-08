// библиотека для работы с GPS устройством
#include <TroykaGPS.h>
#include <Arduino.h>
#include <GY91.h>

#include "FS.h"
#include "SD.h"
//#include "SPI.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include "HardwareSerial.h"
#include <WiFi.h>

//struct ASegPoly obj;

#include <ArduinoJson.h>

#include "SegPoly.h"

WiFiServer server(1729);

String currSSID;


int cnt = 0;
int cntMax = 3600;
bool writing = false;
bool reading = false;
bool pinging = false;

#define SERIAL1_RXPIN 16
#define SERIAL1_TXPIN 17
HardwareSerial Serial1(1);
const int speakerPin = 15;


/* I2C */
Adafruit_BMP280 bme;
MPU9250 mpu; 

typedef struct GPSdataFormat{
  float LatitudeBase10;
  float LongitudeBase10;
  int Hour;
  int Minute;
  int Second;
}GPSdataFormat;

GPSdataFormat GPSdata;

// создаём объект класса GPS и передаём в него объект Serial1 
//SoftwareSerial mySerial(10, 11); // RX, TX
GPS gps(Serial1);
// задаём размер массива для времени, даты, широты и долготы
#define MAX_SIZE_MASS 16
// массив для хранения текущего времени
char time1[MAX_SIZE_MASS];
// массив для хранения текущей даты
char date[MAX_SIZE_MASS];
// массив для хранения широты в градусах, минутах и секундах
char latitudeBase60[MAX_SIZE_MASS];
// массив для хранения долготы в градусах, минутах и секундах
char longitudeBase60[MAX_SIZE_MASS];

/* Переменные состояния */
bool GPScheck = false;
bool IMUcheck = false;
bool Brcheck = false;
bool SDcheck = false;
bool Barcheck = false;

bool onPing = false;
bool FirstMeasurement = true;

int GPSstate = -1;
/*
-1. Старт
0. Найдены спутники
1. Ошибка данных
2. Нет соединения со спутниками
*/

#define SENSORBUFFERCOUNT 100

volatile uint32_t DeviceStatus = 0;
/*
  0. Ничего
  1. Заполняем буффер
  2. Пишем данные
  3. Пишем данные с GPS
*/

/* для таймера */
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

/* счетчики */
volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;
/* период таймера в микросекундах*/
#define DURATION_TIMER 10000  //0.01 s
#define ONESECOND 1000000
/* таймеры пинга в милисекундах */
#define DURATION_PING 10
/* таймеры в сотых секундах, единицах таймера*/
volatile uint32_t MaxSilenceIsrCounter = 300;
volatile uint32_t PingIsrCounter = 10;

/* Одиночный счетчик для вспомогательных вещей*/
/* счетчик для закрытия и открытия файла данных */
volatile uint32_t tempCounter = 0;
volatile uint32_t tempMaxCounter = 10;
/* счетчик для открытия и закрытия файла логов */
volatile uint32_t logCounter = 0;
volatile uint32_t logMaxCounter = 100;
/* счетчик для запросов к GPSГЛОНАСС */
volatile uint32_t gpsCounter = 0;
volatile uint32_t gpsMaxCounter = 1000; /* 1000*0,01 = 10 секунд */

/* Функция таймера*/
void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

void writeon()
{
  /* начинаем чтение датчиков */
  reading = true;
  /* начинаем пищать */
  pinging = true;
  /* сначала нужно заполнить буффер */
  DeviceStatus = 1;
  /* счетчик для записи данных в файл */
  tempCounter = 0;
  tempMaxCounter = 30;
  /* временный счетчик ограничивающий количество записей в файл */
  cnt = 0;
  cntMax = 1000;
}
void writeoff()
{
  /* отключаем чтение от датчиков */
  reading = false;
  /* отключаем пищалку */
  pinging = false;
  /* статус устройства - исходный */
  DeviceStatus = 0;
  /* закрываем файл на всякий случай */
  closeDataFile();
}
int loadConfig()
{
  /* открываем конфигурационный файл */
  data2log("Open config file\n");
  File ConfigFile = SD.open("/config.json");
  if (ConfigFile)
  {
    /* парсим json */
    DynamicJsonBuffer jsonBuffer;
    JsonObject& Config = jsonBuffer.parseObject(ConfigFile);
    if (!Config.containsKey("WiFiAPList"))
    {
      data2log("No AP list in file !\n");
      ConfigFile.close();
      return 0;
    }
    JsonObject& WiFiAPList = Config.get<JsonVariant>("WiFiAPList");
    data2log("WiFi list:\n");
    for (auto kv : WiFiAPList)
    {
      data2log(kv.key);
      data2log("\n");
    }
    /* Включаем режим вай фай, ищем доступные сети */
    StartSTAmode();
    int netCount = getNetworkCounts();
    bool connected2wf = false;
    for (int i = 0; ((i < netCount)&&(!connected2wf)); ++i)
    {
      /* Проверяем конкретные имена сетей */
      String ssid;
      getSSID(&ssid, i);
      if (WiFiAPList.containsKey(ssid)) {
        String pass = WiFiAPList[ssid].as<String>();
        String text = "Have config data for ";
        text += ssid + "\n";
        data2log(text.c_str());
        /* пытаемся подключиться */
        for (int j = 0; j < 3; j++)
        {
          if (ConnectASClient(ssid, pass)) {
            data2log("Connected!\n");
            /* записываем имя подключенной сети */
            currSSID = ssid;
            connected2wf = true;
            ConfigFile.close();
            /* стартуем сервер приема данных по http */
            StartServer();
            return 1;
          }
        }
      }
    }
    ConfigFile.close();
  }
  else
  {
    data2log("Failed to open config file!\n");
  }
  data2log("Not connected in STA WiFi mode \n");
  return 0;
}

void setup()
{ 
  /* открываем последовательный порт для мониторинга действий в программе */
  Serial.begin(115200);
  SerialPrintf("Establish!\r\n");
  /* запускаем пищалку */
  setupTone();
  SerialPrintf("Serial init OK\r\n");
  cyrillic2morse(String("о"));
  /* начальные значения ГЛОНАСС*/
  GPSdata.LongitudeBase10 = 0;
  GPSdata.LatitudeBase10 = 0;
  GPSdata.Hour = 0;
  GPSdata.Minute = 0;
  GPSdata.Second = 99;
  /* запускаем ГЛОНАСС */
  SerialPrintf("Begin GPS\n");
  Serial1.begin(115200, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  if (Serial1)
  {
    GPScheck = true;
  }
  /* запускаем датчики */
  SerialPrintf("Barometer\n");
  Wire.begin();
  if (bme.begin()) {  
    Barcheck = true;
  }
  uint8_t Buf;
  SerialPrintf("AccGyrMag\n");
  if ((mpu.CheckAccGyr())&&(mpu.CheckMagn(&Buf)))
    IMUcheck = true;
  mpu.Setup();
  /* запускаем карту памяти */
  SerialPrintf("SD start\n");
  if(setupSD())
  {
    SDcheck = true;
    setupFile();
    writing = true;
  }
  /* запсукаем вай фай и сервер обработки команд */
  if (!loadConfig())
    StartAPMode();

  /* настройка таймера*/
  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, DURATION_TIMER, true);

  // Start an alarm
  timerAlarmEnable(timer);


  /* уведомляем о готовности */
  if ((GPScheck)&&(IMUcheck)&&(Barcheck)&&(SDcheck))
    cyrillic2morse(String("ок"));
  else
    cyrillic2morse(String("сос"));

}
 
void loop()
{
  /* проверка таймера */
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);
    /* основные действия с датчиками*/
    listenAndGetData(isrTime);

    /* работаем с пищалкой */
    if (pinging)
    {
      if (onPing)
      {
        if (isrCount < PingIsrCounter)
        {}
      else
      {
          /* конец писка */
        pingOff();
        onPing = false;
        isrCounter = 0;
      }
    }
    else{
      if (isrCount < MaxSilenceIsrCounter)
        {}
      else
        {
            /* начало писка */
        pingOn();
        onPing = true;
        isrCounter = 0;
        }
      } 
    }
  }
}




