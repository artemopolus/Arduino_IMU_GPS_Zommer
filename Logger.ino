File FileToData;
File File4Log;
String FilePath;
String LogPath;
  String filedirname = "/sessions";
  String logdirname = "/logs";

void setFilesLogDefault()
{
  /* устанавливаем значения логов и файлов записи по умолчанию */
  String filename = "data";
  String logname = "log";
  String fileext = ".txt";
  FilePath = filedirname + "/" + filename + "0" +fileext;
  LogPath = logdirname + "/" +logname + "0" + fileext;
  /* только пустой файл */
  writeFile(SD,FilePath.c_str(),"");
  writeFile(SD,LogPath.c_str(),"Log on\n");
}
void setupFile()
{
  /* значения имен для файлов и логов плюс расширение */
  String filename = "data";
  String logname = "log";
  String fileext = ".txt";
  int counter = 0;
  dirIsExist(SD,filedirname.c_str());
  dirIsExist(SD,logdirname.c_str());
  bool fileIsFounded = false;
  String trgfilename = filename + "0" + fileext;
  /* ищем имя, которое не занято */
  while(isFileInDir(SD, filedirname.c_str(), trgfilename.c_str()))
  {
    counter++;
    trgfilename = filename + String(counter) + fileext;
  }
  /* запоминаем пути */
  FilePath = filedirname + "/" + filename + String(counter) +fileext;
  LogPath = logdirname + "/" +logname + String(counter) + fileext;

  /* стартовые данные в файлах */
  writeFile(SD,FilePath.c_str(),"");
  writeFile(SD,LogPath.c_str(),"Log on\n");
}
void closeDataFile()
{
  /* закрываем файл записи данных */
  close2openedFile(& FileToData);
}
const size_t SerialPrintf (const char *szFormat, ...) {
  /*
 * SerialPrintf
 * Реализует функциональность printf в Serial.print
 * Применяется для отладочной печати
 * Параметры как у printf
 * Возвращает 
 *    0 - ошибка формата
 *    отрицательное чило - нехватка памяти, модуль числа равен запрашиваемой памяти
 *    положительное число - количество символов, выведенное в Serial
 */
  va_list argptr;
  va_start(argptr, szFormat);
  char *szBuffer = 0;
  const size_t nBufferLength = vsnprintf(szBuffer, 0, szFormat, argptr) + 1;
  if (nBufferLength == 1) return 0;
  szBuffer = (char *) malloc(nBufferLength);
  if (! szBuffer) return - nBufferLength;
  vsnprintf(szBuffer, nBufferLength, szFormat, argptr);
  if (Serial){
    /* льем в серийник */
    Serial.print(szBuffer);
  }
  free(szBuffer);
  return nBufferLength - 1;
}

void sensorBIN2SD(struct SensorAllData  * DataStore, size_t DataStoreSZ)
{
  /* открываем файл на нулевой итерации */
  if (tempCounter == 0)
  {
    open2appendFile(SD, FilePath.c_str(), & FileToData);
  }
  /* пишем данные */
  FileToData.write((const uint8_t *) DataStore, DataStoreSZ);
  if (tempCounter < tempMaxCounter)
  {
    tempCounter++;
  }
  else
  {
    /* закрываем файл после срабатывания счетчика */
    close2openedFile(& FileToData);
    tempCounter = 0;
  }
}

void data2log(const char * message)
{
  /* Если открыт серийник, то льем туда данные */
  if (Serial){
    Serial.print(message);
  }
  /* если можно писать на карту, то логируем по счетчику (НАДО ли????) */
  if (writing)
  {
    if (logCounter == 0)
    {
      open2appendFile(SD, LogPath.c_str(), & File4Log);
    }
    append2openedFile(&File4Log,message);
    if (logCounter < logMaxCounter)
    {
      logCounter++;
    }
    else
    {
      close2openedFile(& File4Log);
      logCounter = 0;
    }
  }
}
