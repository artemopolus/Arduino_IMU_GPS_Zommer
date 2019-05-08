//вставить пример GET
// New Client.
// GET /favicon.ico HTTP/1.1
// Host: 192.168.0.1:1729
// Connection: keep-alive
// User-Agent: Mozilla/5.0 (Linux; Android 7.1.2; Redmi 4A Build/N2G47H) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 YaBrowser/18.1.0.527.00 Mobile Safari/537.36
// Accept: image/webp,image/apng,image/*,*/*;q=0.8
// Referer: http://192.168.0.1:1729/
// Accept-Encoding: gzip, deflate, sdch
// Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7

// GET /writeon HTTP/1.1
// Host: 192.168.0.1:1729
// Connection: keep-alive
// Cache-Control: max-age=0
// X-Chrome-offline: persist=0 reason=reload
// User-Agent: Mozilla/5.0 (Linux; Android 7.1.2; Redmi 4A Build/N2G47H) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 YaBrowser/18.1.0.527.00 Mobile Safari/537.36
// Upgrade-Insecure-Requests: 1
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
// Accept-Encoding: gzip, deflate, sdch
// Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7



// Нужно это парсить
// New Client.
// POST / HTTP/1.1
// host: 192.168.0.1:1729
// accept: application/json
// content-type: application/json
// content-length: 205
// Connection: close

// {"name":"post-upload","version":"1.0.0","description":"","main":"index.js","scripts":{"test":"echo \"Error: no test specified\" && exit 1"},"author":"","license":"ISC","dependencies":{"request":"^2.83.0"}}max req
// Client Disconnected.
const char * READFILE = "readfiles";

int httpPOSTparse(const String message)
{
    if(message.substring(0,4) == "POST")
    {
        data2log("POST request\n");
        int NextTrgCh = message.indexOf(' ');
        int PrevTrgCh = 0;
        /* ищем последнее слово */
        while(NextTrgCh != -1)
        {
            PrevTrgCh = NextTrgCh;
            NextTrgCh = message.indexOf(' ',(PrevTrgCh + 1));
        }
        /* парсим последнее слово*/
        String HttpStr = message.substring((PrevTrgCh+1), (message.length()));
        data2log(HttpStr.c_str());
        if (HttpStr.substring(0,4) == "HTTP")
        {
            data2log("POST request version ");
            if(HttpStr.length() > 4)
                data2log(HttpStr.substring(4,(HttpStr.length() - 1 )).c_str());
            data2log("\n");
            return 1;
        }
    }
    return 0;
}
bool isPOSTcontype(const String message)
{
    String Trg = "content-type";
    if (message.length() > (Trg.length() + 3))
    {
        if (message.substring(0,(Trg.length()-1)) == Trg)
        {
            Trg += ": application/json";
            if (message.length() == Trg.length())
            {
                if (message == Trg)
                {
                    data2log("POST json type\n");
                    return true;
                }
            }
        }
    }
    return false;
}
int isPOSTconlen(const String message)
{
    String Trg = "content-length";
    /* слово + ": 1" */
    if (message.length() > (Trg.length() + 3))
    {
        data2log(message.substring(0,Trg.length()).c_str());
        if (message.substring(0,Trg.length()) == Trg)
        {
            String num = message.substring((Trg.length() + 1),(message.length()));
            int ans = num.toInt();
            data2log("Wait data length ");
            data2log(num.c_str());
            data2log("\n");
            return ans;
        }
    }
    return -1;
}
int httpGETparse(const String message, String * result)
{
    /* если у нас GET */
    //data2log(message.substring(0,3).c_str());
    //data2log("]\n");
	if ((message.substring(0,3) == "GET")&&(message[4] == '/'))
	{
        data2log("GET request\n");
		int TrgCh = 5;
		uint cCh = 0;
		int NextTrgCh = message.indexOf(' ',TrgCh);
		if (NextTrgCh == -1)
		{
            /* wait */
			return 0;
		}
		else
		{
            /* Строка запроса + длина с версией HTTP/1.1*/           
            if (message.length() > (NextTrgCh + 4 + 2)){               
                String HttpStr = message.substring((NextTrgCh + 1),(NextTrgCh + 5));
                //data2log(HttpStr.c_str());
                if (HttpStr == "HTTP")
                {
                    data2log("from client: GET HTTP request version ");
                    data2log(message.substring((NextTrgCh + 5),(message.length() )).c_str());
                    data2log("\n");
                    /* сам запрос */
                    String InfStr = message.substring(TrgCh,NextTrgCh);
                    data2log("[");
                    //InfStr.remove(0);
                    data2log(InfStr.c_str());
                    data2log("]\n");
                    /* парсим */
                    NextTrgCh = InfStr.indexOf('/');
                    if (NextTrgCh == -1)
                    {
                        /* обычный запрос */
                        data2log("smpl request\n");
                        if (InfStr == "")
                        {
                            data2log("need to send main page...\n");
                            return 1;
                        }
                        if (InfStr == "writeon") {
                            data2log("GET [CMD]: write start\n");
                            return 2;
                        }
                        if (InfStr == "writeoff") {
                            data2log("GET [CMD]: write stop\n");
                            return 3;
                        }
                        if (InfStr == "files") {
                            data2log("GET [FILES]: files request\n");
                            return 4;
                        }
                        if (InfStr == "logs") {
                            data2log("GET [FILES]: logs request\n");
                            return 5;
                        }
                        if (InfStr == "deletefiles")
                        {
                            data2log("GET [FILES]: delete all\n");
                            return 7;
                        }
                        if (InfStr == "ping")
                        {
                            data2log("GET [CMD]: ping on\n");
                            return 8;
                        }
                        if (InfStr == "barfix")
                        {
                            data2log("GET [CMD]: get barometer null point\n");
                            return 9;
                        }
                    }
                    else
                    {
                        data2log(InfStr.substring(0,(NextTrgCh)).c_str());
                        if ((InfStr.substring(0,(NextTrgCh)) == READFILE)&&((NextTrgCh + 2) < InfStr.length()))    
                        {
                            /* запрос файла */
                            data2log("GET read files\n");
                            *result = '/' + InfStr.substring((NextTrgCh + 1),(InfStr.length()));
                            return 6;
                        }
                    }
                }
                else
                {
                    data2log("non HTTP ???\n");
                    return 0;
                }
            }
            else
            {
                /* */
                return 0;
            }
			
		}
	}
	return 0;
}
void sendSDFileList(WiFiClient * client, const char * dirname)
{
  client->flush();
  client->println("HTTP/1.1 200 OK");
  client->println("Content-type:text/html");
  client->println();
  client->print("Click <a href=\"/\">back</a> to main<br><br>");
  client->println("DIR:");
  client->println(dirname);
  client->println(":<br>");

  File root = SD.open(dirname);
    if(!root){
        client->println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        client->println("Not a directory");
        return;
    }
    client->println("<br>");
    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            client->println("DIR: ");
            client->println(file.name());
        } else {
            //"Click <a href=\"/files\">here</a> to list of files <br><br>"
            //("FILE: <a href=\"/files/dirname/file.name\">file.name()</a> SIZE: file.size()");
            String Text2send = "FILE: <a href=\"/";
            Text2send += String(READFILE) + String(file.name());
            Text2send += String("\">") + String(file.name()) + "</a>";
            Text2send += String(" SIZE: ") + String(file.size());
            client->println(Text2send.c_str());
            //client->println("FILE: ");
            //client->println(file.name());
            //client->println("  SIZE: ");
            //client->println(file.size());
        }
        client->println("<br>");
        file = root.openNextFile();
    }

}
void readSDFilePage(WiFiClient * client, const char * path){
    //data2log("Reading file: %s\n", path);
    data2log("Read from ");
    data2log(path);
    data2log("\n");
    File file = SD.open(path);
    if(!file){
        data2log("Failed to open file for reading\n");
        return;
    }

    data2log("Read from file: ");
    client->flush();
    client->println("HTTP/1.1 200 OK");
    client->println("Content-type:text/html");
    client->println();
    client->println(path);
    while(file.available()){
        //Serial.write(file.read());
        //SerialPrintf("%d",file.read());
        String buffer = file.readStringUntil('\n');
        client->println(buffer.c_str());
        client->println("<br>");
    }
    file.close();
}
void readSDfileBINpage(WiFiClient * client, const char * path)
{
    data2log("Read from ");
    data2log(path);
    data2log("\n");
    File file = SD.open(path);
    if(!file){
        data2log("Failed to open file for reading\n");
        return;
    }

    data2log("Read from file: ");
    client->flush();
    client->println("HTTP/1.1 200 OK");
    client->println("Content-type:text/html");
    client->println();
    client->print("Click <a href=\"/\">back</a> to main. DATA: ");
    client->println(path);
    client->println("<br>");
    while(file.available())
    {
        struct SensorAllData DataStoreSD;
        file.read((uint8_t *)&DataStoreSD, sizeof(DataStoreSD));
        String str;
        sensorOneSet2string(&str, &DataStoreSD);
        str += "<br>";
        client->println(str.c_str());
    }
}