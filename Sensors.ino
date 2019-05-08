/* объект для бинарной записи */
typedef struct SensorAllData
{
  int count;
  int milisec;
  /* акселерометр */
  int accx;
  int accy;
  int accz;
  /* гироскоп */
  int gyrx;
  int gyry;
  int gyrz;
  /* магнитометер */
  int magx;
  int magy;
  int magz;
  /* давление */
  float bar;
  float alt;
  /* температура */
  float temp;
  /* GPS */
  float LatitudeBase10;
  float LongitudeBase10;
  int Hour;
  int Minute;
  int Second;
  int GPSdatastate;
}SensorAllData;
/* сам объект в памяти */
SensorAllData DataStore;
/* буффер для хранения данных и обработки фильтрами */
typedef struct SensorSet
{
  int count[SENSORBUFFERCOUNT];
  int milisec[SENSORBUFFERCOUNT];
  /* акселерометр */
  int accx[SENSORBUFFERCOUNT];
  int accy[SENSORBUFFERCOUNT];
  int accz[SENSORBUFFERCOUNT];
  /* гироскоп */
  int gyrx[SENSORBUFFERCOUNT];
  int gyry[SENSORBUFFERCOUNT];
  int gyrz[SENSORBUFFERCOUNT];
  /* магнитометер */
  int magx[SENSORBUFFERCOUNT];
  int magy[SENSORBUFFERCOUNT];
  int magz[SENSORBUFFERCOUNT];
  /* давление */
  float bar[SENSORBUFFERCOUNT];
  float alt[SENSORBUFFERCOUNT];
  /* температура */
  float temp[SENSORBUFFERCOUNT];
  /* GPS */
  float LatitudeBase10[SENSORBUFFERCOUNT];
  float LongitudeBase10[SENSORBUFFERCOUNT];
  int Hour[SENSORBUFFERCOUNT];
  int Minute[SENSORBUFFERCOUNT];
  int Second[SENSORBUFFERCOUNT];
  int GPSdatastate[SENSORBUFFERCOUNT];
}SensorSet;



SensorSet SensorBuffer;
void sensor2sensorMass(SensorAllData * mass, int index, int counter, int milisec)
{
  /* записываем данные в объект для бинарной записи */
  mass[index].count = counter;
  mass[index].milisec = milisec;
  mass[index].accx = mpu.ax;
  mass[index].accy = mpu.ay;
  mass[index].accz = mpu.az;

  mass[index].gyrx = mpu.gx;
  mass[index].gyry = mpu.gy;
  mass[index].gyrz = mpu.gz;

  mass[index].magx = mpu.mx;
  mass[index].magy = mpu.my;
  mass[index].magz = mpu.mz;

  mass[index].bar = bme.readPressure();
  mass[index].temp = bme.readTemperature();
  mass[index].alt = bme.readAltitude(1013.25);

  mass[index].LatitudeBase10 = GPSdata.LatitudeBase10;
  mass[index].LongitudeBase10 = GPSdata.LongitudeBase10;
  mass[index].Hour = GPSdata.Hour;
  mass[index].Minute = GPSdata.Minute;
  mass[index].Second = GPSdata.Second;
  mass[index].GPSdatastate = GPSstate;
}
void sensorOneSet2sensorMass(SensorAllData * mass, int trg, SensorAllData * set)
{
  mass[trg].count = set->count;
  mass[trg].milisec = set->milisec;
  mass[trg].accx = set->accx;
  mass[trg].accy = set->accy;
  mass[trg].accz = set->accz;
  mass[trg].gyrx = set->gyrx;
  mass[trg].gyry = set->gyry;
  mass[trg].gyrz = set->gyrz;
  mass[trg].magx = set->magx;
  mass[trg].magy = set->magy;
  mass[trg].magz = set->magz;
  mass[trg].bar = set->bar;
  mass[trg].temp = set->temp;
  mass[trg].alt = set->alt;
  mass[trg].LatitudeBase10 = set->LatitudeBase10;
  mass[trg].LongitudeBase10 = set->LongitudeBase10;
  mass[trg].Hour = set->Hour;
  mass[trg].Minute = set->Minute;
  mass[trg].Second = set->Second;
  mass[trg].GPSdatastate = set->GPSdatastate;
}
void replaceSensorAllDataMass(SensorAllData * mass,int trg, int src)
{
  mass[trg].count = mass[src].count;
  mass[trg].milisec = mass[src].milisec;
  mass[trg].accx = mass[src].accx;
  mass[trg].accy = mass[src].accy;
  mass[trg].accz = mass[src].accz;
  mass[trg].gyrx = mass[src].gyrx;
  mass[trg].gyry = mass[src].gyry;
  mass[trg].gyrz = mass[src].gyrz;
  mass[trg].magx = mass[src].magx;
  mass[trg].magy = mass[src].magy;
  mass[trg].magz = mass[src].magz;
  mass[trg].bar = mass[src].bar;
  mass[trg].temp = mass[src].temp;
  mass[trg].alt = mass[src].alt;
  mass[trg].LatitudeBase10 = mass[src].LatitudeBase10;
  mass[trg].LongitudeBase10 = mass[src].LongitudeBase10;
  mass[trg].Hour = mass[src].Hour;
  mass[trg].Minute = mass[src].Minute;
  mass[trg].Second = mass[src].Second;
  mass[trg].GPSdatastate = mass[src].GPSdatastate;
}
void updateLastDataMass(SensorAllData * mass, int count, int counter, int milisec)
{
  for (int i = 0; i < (count - 1); ++i)
  {
    /* code */
    replaceSensorAllDataMass(mass, i, (i+1));
  }
  sensor2sensorMass(mass,(count - 1), counter, milisec);
}

void sensor2sensorOneSet(int index, int milisec)
{
  /* записываем данные в объект для бинарной записи */
  DataStore.count = index;
  DataStore.milisec = milisec;
  DataStore.accx = mpu.ax;
  DataStore.accy = mpu.ay;
  DataStore.accz = mpu.az;

  DataStore.gyrx = mpu.gx;
  DataStore.gyry = mpu.gy;
  DataStore.gyrz = mpu.gz;

  DataStore.magx = mpu.mx;
  DataStore.magy = mpu.my;
  DataStore.magz = mpu.mz;

  DataStore.bar = bme.readPressure();
  DataStore.temp = bme.readTemperature();
  DataStore.alt = bme.readAltitude(1013.25);

  DataStore.LatitudeBase10 = GPSdata.LatitudeBase10;
  DataStore.LongitudeBase10 = GPSdata.LongitudeBase10;
  DataStore.Hour = GPSdata.Hour;
  DataStore.Minute = GPSdata.Minute;
  DataStore.Second = GPSdata.Second;
  DataStore.GPSdatastate = GPSstate;
}
void dataSet2bufferIndex(int index, int rcount, int rmilisec)
{
  /* заполняем данные в буффер для обработки */
  SensorBuffer.count[index] = rcount;
  SensorBuffer.milisec[index] = rmilisec;
  SensorBuffer.accx[index] = mpu.ax;
  SensorBuffer.accy[index] = mpu.ay;
  SensorBuffer.accz[index] = mpu.az;
  SensorBuffer.gyrx[index] = mpu.gx;
  SensorBuffer.gyry[index] = mpu.gy;
  SensorBuffer.gyrz[index] = mpu.gz;
  SensorBuffer.magx[index] = mpu.mx;
  SensorBuffer.magy[index] = mpu.my;
  SensorBuffer.magz[index] = mpu.mz;
  SensorBuffer.bar[index] = bme.readPressure();
  SensorBuffer.temp[index] = bme.readTemperature();
  SensorBuffer.alt[index] = bme.readAltitude(1013.25);
  SensorBuffer.LatitudeBase10[index] = GPSdata.LatitudeBase10;
  SensorBuffer.LongitudeBase10[index] = GPSdata.LongitudeBase10;
  SensorBuffer.Hour[index] = GPSdata.Hour;
  SensorBuffer.Minute[index] = GPSdata.Minute;
  SensorBuffer.Second[index] = GPSdata.Second;
  SensorBuffer.GPSdatastate[index] = GPSstate;
}
void sensorOneSet2string(String * text, struct SensorAllData * srcStore)
{
  /* конвертируем объект в строку с данными */
  // * text = "ct: " + String(srcStore->count);
  // * text += " tt: " + String(srcStore->milisec);
  // * text += " ax: " + String(srcStore->accx);
  // * text += " ay: " + String(srcStore->accy);
  // * text += " az: " + String(srcStore->accz);
  // * text += " gx: " + String(srcStore->gyrx);
  // * text += " gy: " + String(srcStore->gyry);
  // * text += " gz: " + String(srcStore->gyrz);
  // * text += " mx: " + String(srcStore->magx);
  // * text += " my: " + String(srcStore->magy);
  // * text += " mz: " + String(srcStore->magz);
  // * text += " br: " + String(srcStore->bar);
  // * text += " tm: " + String(srcStore->temp);
  // * text += " al: " + String(srcStore->alt);
  // * text += " la: " + String(srcStore->LatitudeBase10);
  // * text += " lo: " + String(srcStore->LongitudeBase10);
  // * text += " ho: " + String(srcStore->Hour);
  // * text += " mi: " + String(srcStore->Minute);
  // * text += " sc: " + String(srcStore->Second);
  // * text += " st: " + String(srcStore->GPSdatastate);
  * text = String(srcStore->count);
  * text += "\t" + String(srcStore->milisec);
  * text += "\t" + String(srcStore->accx);
  * text += "\t" + String(srcStore->accy);
  * text += "\t" + String(srcStore->accz);
  * text += "\t" + String(srcStore->gyrx);
  * text += "\t" + String(srcStore->gyry);
  * text += "\t" + String(srcStore->gyrz);
  * text += "\t" + String(srcStore->magx);
  * text += "\t" + String(srcStore->magy);
  * text += "\t" + String(srcStore->magz);
  * text += "\t" + String(srcStore->bar);
  * text += "\t" + String(srcStore->temp);
  * text += "\t" + String(srcStore->alt);
  * text += "\t" + String(srcStore->LatitudeBase10);
  * text += "\t" + String(srcStore->LongitudeBase10);
  * text += "\t" + String(srcStore->Hour);
  * text += "\t" + String(srcStore->Minute);
  * text += "\t" + String(srcStore->Second);
  * text += "\t" + String(srcStore->GPSdatastate);
}

void dataStore2SD()
{
  /* сохраняем данные на карту памяти */
  sensorBIN2SD(& DataStore, sizeof(DataStore));
}

void replaceBuffer(int index, int src)
{
  /* переписываем данные одного измерения в другой */
  SensorBuffer.count[index] = SensorBuffer.count[src];
  SensorBuffer.milisec[index] = SensorBuffer.milisec[src];

	SensorBuffer.accx[index] = SensorBuffer.accx[src];
	SensorBuffer.accy[index] = SensorBuffer.accy[src];
	SensorBuffer.accz[index] = SensorBuffer.accz[src];

	SensorBuffer.gyrx[index] = SensorBuffer.gyrx[src];
	SensorBuffer.gyry[index] = SensorBuffer.gyry[src];
	SensorBuffer.gyrz[index] = SensorBuffer.gyrz[src];

	SensorBuffer.magx[index] = SensorBuffer.magx[src];
	SensorBuffer.magx[index] = SensorBuffer.magy[src];
	SensorBuffer.magx[index] = SensorBuffer.magz[src];

	SensorBuffer.bar[index] = SensorBuffer.bar[src];
	SensorBuffer.temp[index] = SensorBuffer.temp[src];
	SensorBuffer.alt[index] = SensorBuffer.alt[src];

	SensorBuffer.LatitudeBase10[index] = SensorBuffer.LatitudeBase10[src];
	SensorBuffer.LongitudeBase10[index] = SensorBuffer.LongitudeBase10[src];
	SensorBuffer.Hour[index] = SensorBuffer.Hour[src];
	SensorBuffer.Minute[index] = SensorBuffer.Minute[src];
	SensorBuffer.Second[index] = SensorBuffer.Second[src];
	SensorBuffer.GPSdatastate[index] = SensorBuffer.GPSdatastate[src];
}


void updateBuffer(int rcount, int rmilisec)
{
  /* удаляем один строку данных и смещаем все наверх на ту же строчку */
	for (int i = 0; i < (SENSORBUFFERCOUNT - 1); ++i)
	{
		/* записываем в текущую строчку данные с последующей */
		replaceBuffer((i),(i+1));
	}
  /* заполняем послднюю строчку новыми данными */
	dataSet2bufferIndex((SENSORBUFFERCOUNT - 1),rcount,rmilisec);
}
void allBuffer2serial()
{
  /* вываливаем данные из буффера в серийный порт */
	for (int i = 0; i < SENSORBUFFERCOUNT; ++i)
	{
		/* По строчке */
		buffer2serial(i);
	}
}
void buffer2serial(int index)
{
  /* отправляем все данные строчкой в порт */
	SerialPrintf("Sa%06da%06dSa%3.6fa%3.6fa%02da%02da%02dSa%2.2fa%8.2fa%4.2fSa%+06da%+06da%+06da%+06da%+06da%+06da%+06da%+06da%+06d\n",SensorBuffer.count,SensorBuffer.milisec,SensorBuffer.LatitudeBase10[index],SensorBuffer.LongitudeBase10[index],SensorBuffer.Hour[index],SensorBuffer.Minute[index], SensorBuffer.Second[index],SensorBuffer.temp[index], SensorBuffer.bar[index], SensorBuffer.alt[index],SensorBuffer.accx[index], SensorBuffer.accy[index], SensorBuffer.accz[index], SensorBuffer.gyrx[index], SensorBuffer.gyry[index], SensorBuffer.gyrz[index], SensorBuffer.magx[index], SensorBuffer.magy[index], SensorBuffer.magz[index]);
}

int updateBufferAndPrintf( int valcounter, const int milisec)
{
  //Serial.print("i:");
  //Serial.println(millis());
  /* опрашиваем ГЛОНАСС по счетчику */
  if (gpsCounter < gpsMaxCounter)
    gpsCounter++;
  else{
    updateGPS();
    gpsCounter = 0;
  } 
  /* опрашиваем инерциальные датчики*/
	updateInert();
  /* флаг заполнения буффера */
	int flag = 0;

	if (valcounter < SENSORBUFFERCOUNT)
	{
    /* заполняем буффер */
		dataSet2bufferIndex(valcounter,valcounter,milisec);
    /* данные в объект для бинарной записи */
    sensor2sensorOneSet(valcounter, milisec);
    /* записывем объект бинарно */
    dataStore2SD();
		flag = 0;
	}
	else
	{
    /* обновляем буффер */
		updateBuffer(valcounter, milisec);
    /* данные в объект для бинарной записи */
    sensor2sensorOneSet(valcounter, milisec);
    /* записывем объект бинарно */
    dataStore2SD();
		flag = 1;
	}
	return flag;
}
void listenAndGetData(const int milisec)
{
  /* ждем запросов по вай-фай */
	listenClients();
	if (reading)
	{
		if (cnt < cntMax)
		{
			int res = updateBufferAndPrintf(cnt,milisec);
			if(res)
			{
				DeviceStatus = 1;
				if(GPSstate == 0)
					DeviceStatus = 2;
			}
			cnt++;
		}
		else
		{
			if (writing){				
				data2log("Finish writing file!");
			}
      writeoff();
		}
	}
}




void updateGPS()
{
  /* если пришли данные с gps-модуля */
  if (gps.available()) {
    /* считываем данные и парсим */
    gps.readParsing();
    /* проверяем состояние GPS-модуля */
    switch(gps.getState()) {
      /* всё OK */
      case GPS_OK:
        GPSstate = 0;
        GPSdata.LatitudeBase10 = gps.getLongitudeBase10();
        GPSdata.LongitudeBase10 = gps.getLongitudeBase10();
        GPSdata.Hour = gps.getHour();
        GPSdata.Minute = gps.getMinute();
        GPSdata.Second = gps.getSecond();
        break;
      /* ошибка данных */
      case GPS_ERROR_DATA:
        GPSstate = 1;
        break;
      /* нет соединение со спутниками */
      case GPS_ERROR_SAT:
        GPSstate = 2;
        break;
    }
    
  }
}

void updateInert()
{
  /* обновляем информацию по аксерометру */
    mpu.updateAcc();
    /* гироскоп */
    mpu.updateGyr();
    /* магнитометр */
    mpu.updateMag();
}