bool ConnectASClient(String ssid, String pass) {
  
  String text = "Connecting to ";
  text += ssid;
  text += "\n";

  data2log(text.c_str());

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  WiFi.begin(ssid.c_str(), pass.c_str());
  byte tries = 12;
  while ((--tries) && (WiFi.status() != WL_CONNECTED)) {
    text = String(tries);
    text += " attempts to connect the left\r\n";
    data2log(text.c_str());
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    data2log("Connection is failed\n");
    return false;
  }
  else {
    /* 
    Иначе удалось подключиться отправляем сообщение
    о подключении и выводим адрес IP 
    */
    text = "Connected to ";
    text += ssid;
    text += "\nIP address: ";
    text += WiFi.localIP().toString();
    text += "\n";
    data2log(text.c_str());
    return true;
  }
}

int StartSTAmode()
{

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
}
int getNetworkCounts()
{
  int n = WiFi.scanNetworks();
  data2log("scan networks\n");
  if (n == 0) {
    data2log("no avaiable networks\n");
  }
  else
  {
    String text = "networks count: ";
    text = String(n);
  }
  return n;
}
void getSSID(String * ssid, int index)
{
  * ssid = WiFi.SSID(index);
}
void StartServer()
{
  data2log("Start server for http\n");
  server.begin();
}

void StartAPMode()
{ // Отключаем WIFI
  //  //debug(F("Starting Ap mode\r\n"));
  WiFi.disconnect();
  delay(100);
  // Меняем режим на режим точки доступа
  WiFi.mode(WIFI_AP);
  // Задаем настройки сети

  // Включаем WIFI в режиме точки доступа с именем и паролем
  const char * apname = "SuperPuperTracker";
  const char * pass  = "12345678";
  WiFi.softAP(apname, pass);
  WiFi.softAPConfig(IPAddress(192, 168, 0, 1), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0));
  Serial.println(WiFi.softAPIP());
  server.begin();
}
void listenClients()
{
  //Serial.println("Listen");
  WiFiClient client = server.available();   // listen for incoming clients
  int reqCount = 0;
  const int MAXREQCOUNT = 10;



  if (client) {                             // if you get a client,

    uint32_t ReqType = 0;
    /*
    0. Ничего
    1. GET
    2. POST
    3. Ждем POST данные
    */
    String LastCmd = "";

    int JsonLen = -1;

    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          /* Тут мы получили конец строки */
          if (currentLine.length() == 0) {
            
              //sendMainPage(&client);
            //sendOkPage(&client);
            // break out of the while loop:
            //break;
            /* Здесь мы получили пустую строку
            если тип ГЕТ, то конец
            если тип ПОСТ, то надо ждать данных
            */
            if(ReqType == 1)
            {
                break;
            }
            if(ReqType == 2)
            {
              data2log("wait json\n");
              ReqType = 3;
            }
          } else {    // if you got a newline, then clear currentLine:
            /* Здесь мы получили строку с данными, которые надо обработать
            а потом очистить 
            Если мы ждем данных ПОСТ, то их надо получить и проверить
            */
            String Ans = "";
            int LineAns = httpGETparse(currentLine, &Ans);
            //data2log(currentLine.c_str());
            //data2log("Answer: ");
           // data2log(String(LineAns).c_str());
            //data2log("\n");
            if (LineAns)
            {
              ReqType = 1;
              if (LineAns == 1)
              {
                /* главная страница */
                sendMainPage(&client);

              }
              if (LineAns == 2)
              {
                /* главная страница */
                writeon();
                sendMainPage(&client);

              }
              if (LineAns == 3)
              {
                /* главная страница */
                writeoff();
                sendMainPage(&client);

              }
              if (LineAns == 4)
              {
                /* файлы из СД */
                sendSDFileList(& client, filedirname.c_str());

              }
              if (LineAns == 5)
              {
                /* логи из СД */
                sendSDFileList(& client, logdirname.c_str());
              }
              if (LineAns == 6)
              {
                /* чтение файла */
                LastCmd = Ans;
                readSDfileBINpage(& client, LastCmd.c_str());
              }
              if (LineAns == 7)
              {
                /* удаляем файлы */
                deleteFilesInDir(SD, filedirname.c_str());
                deleteFilesInDir(SD, logdirname.c_str());
                /* обновляем имена логов */
                setFilesLogDefault();
                /* отправляем ответ */
                sendStatusPage(& client, "Files are deleted!");
              }
              if (LineAns == 8)
              {
                data2log("ping on\n");
                pinging = true;
                //durationSilence = 3000;
                sendMainPage(&client);
              }
              if (LineAns == 9)
              {
                data2log("start get barometer base 100 times\n");
                tempCounter = 0;
                tempMaxCounter = 100;
                sendMainPage(&client);
              }
            }
            
            if (ReqType == 0)
            {
              LineAns = httpPOSTparse(currentLine);
              if (LineAns == 1)
                ReqType = 2;
            }
            if (ReqType == 2)
            {
              int ans = isPOSTconlen(currentLine);
              if (ans != -1){
                data2log("get Json length");
                data2log(String(ans).c_str());
                data2log("\n");
                JsonLen = ans;
              }
            }
            currentLine = "";
            // if (ReqType == 3)
            // {
            //   /* считать json */
            //   data2log("read json file\n");
            //   ReqType = 0;
            //   break;
            // }
          }
        } else
        { 
          if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
            if (ReqType == 3)
            {
              if (JsonLen == currentLine.length())
              {
                  data2log("read json file\n");
                  data2log(currentLine.c_str());
                  data2log("\n");
                  parsePOSTjson(currentLine.c_str());
                  ReqType = 0;
                  sendOkPage(& client);
                  break;
              }
            }
          }
          
        }
        //data2log(currentLine.c_str());
        // Check to see if the client request was "GET /H" or "GET /L":

        // if (currentLine.endsWith("GET /writeon")) {
        //   writeon();
        // }
        // if (currentLine.endsWith("GET /writeoff")) {
        //   writeoff();
        // }
        // if (currentLine.endsWith("GET /files")) {
        //   sendSDFileList(& client, filedirname.c_str());
        // }
        // if (currentLine.endsWith("GET /logs")) {
        //   sendSDFileList(& client, logdirname.c_str());
        // }
        parseURL(&client, &currentLine);

      }
      else
      {
        if (reqCount > MAXREQCOUNT)
        {
          /* Если клиента нет, то выходим через несколько проверок */
          //data2log(String(currentLine.length()).c_str());
          //data2log("max req\n");
          //sendOkPage(&client);
          break;
        }
        else
          reqCount++;
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
void sendOkPage(WiFiClient * client)
{
  client->println("HTTP/1.1 200 OK");
  //client->println("Content-type:text/html");
  client->println();

}
void sendMainPage(WiFiClient * client)
{
  data2log("send main page\n");
  client->println("HTTP/1.1 200 OK");
  client->println("Content-type:text/html");
  client->println();

  // the content of the HTTP response follows the header:
  
  client->print("Click <a href=\"/writeon\">here</a> to start write data to sd<br><br>");
  client->print("Click <a href=\"/writeoff\">here</a> to stop write data to sd<br><br>");

  client->print("Click <a href=\"/files\">here</a> to list of files <br><br>");
  client->print("Click <a href=\"/logs\">here</a> to list of logs <br><br>");
  client->print("Click <a href=\"/deletefiles\">here</a> to delete all files and logs <br><br>");

  client->print("Click <a href=\"/ping\">here</a> to ping <br><br>");
  
  client->print("Click <a href=\"/barfix\">here</a> to get current bar val <br><br>");

  client->print("Status: <br>");
  client->print("IMU:");
  if (IMUcheck) client->println("ok");
    else client->println("problem");
  client->print("<br>Barometer:");
  if (Barcheck) client->println("ok");
    else client->println("problem");
  client->print("<br>GPS:");
  if (GPScheck) client->println("ok");
    else client->println("problem");
  client->print("<br>SD:");
  if (SDcheck) client->println("ok");
    else client->println("problem");
  client->print("<br>GPS status:");
  if (GPSstate == 0)
  {
    client->print("data captured");
  }
  if (GPSstate == 2)
  {
    client->print("sat. not found");
  }
  client->print("<br><br>");

  client->print("reading from sensors:");
  if (reading)
    client->print("yes");
  else
    client->print("no");
  client->print("<br><br>");
  client->print("writing to SD:");
  if (writing)
    client->print("yes");
  else
    client->print("no");
  client->print("<br><br>");
  // The HTTP response ends with another blank line:
  client->println();
}
void sendStatusPage(WiFiClient * client, const char * status)
{
  client->flush();
  client->println("HTTP/1->1 200 OK");
  client->println("Content-type:text/html");
  client->println();
  client->print("Message: <br>");
  client->print(status);
  data2log("[STATUS]: ");
  data2log(status);
  data2log("\n");
}

void sendTestPage(WiFiClient * client,String * text)
{
  client->flush();
  client->println("HTTP/1->1 200 OK");
  client->println("Content-type:text/html");
  client->println();
  client->println(text->c_str());
}
void parseURL(WiFiClient * client,String * text)
{
  String cmdStr = "GET /command/write?";
  String varStr = "tim=000000";
  if (text->length() < (cmdStr.length() + varStr.length()))
    return;
  if (!text->startsWith(cmdStr))
    return;
  int cmdStrLen = cmdStr.length();
  if ((text[0][cmdStrLen] == 't')&&(text[0][cmdStrLen+1] == 'i')&&(text[0][cmdStrLen+2] == 'm'))
  {
    int val = text->substring((cmdStrLen+4),(text->length())).toInt();
    client->flush();
    client->println("HTTP/1.1 200 OK");
    client->println("Content-type:text/html");
    client->println();
    client->println("get:");
    client->println(val);
  }
}

