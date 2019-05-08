void parsePOSTjson(const char * str)
{
	DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(str);
    if (json.containsKey("WiFiAPList"))
    {
    	JsonObject& WiFiAPList = json.get<JsonVariant>("WiFiAPList");
    	data2log("add to WiFi list:\n");
    	for (auto kv : WiFiAPList)
    	{
      		data2log(kv.key);
      		data2log("\n");
      		addString2ConfigJson("WiFiAPList", kv.key, kv.value);
    	}
    	config2log();
    }
}
void addString2ConfigJson(String SubFolder, String key, String value)
{
	data2log("Open config file\n");
	File ConfigFile = SD.open("/config.json");
	if (!ConfigFile)
	{
		data2log("Failed to open config file!\n");
		return;
	}
	/* парсим json */
	DynamicJsonBuffer jsonBuffer;
	JsonObject& Config = jsonBuffer.parseObject(ConfigFile);
	if (!Config.containsKey(SubFolder.c_str()))
	{
		data2log("No AP list in file !\n");
		ConfigFile.close();
		return;
	}
	String text = "add to ";
	text += SubFolder;
	text += " key: ";
	text += key;
	text += " value: ";
	text += value;
	text += "\n";
	data2log(text.c_str());
	JsonObject& TrgJson = Config.get<JsonVariant>(SubFolder.c_str());
	TrgJson.set(key, value.c_str());
	Config.set(SubFolder, TrgJson);
	Config.printTo(ConfigFile);
	ConfigFile.close();
}
void config2log()
{
	File ConfigFile = SD.open("/config.json");
	DynamicJsonBuffer jsonBuffer;
	JsonObject& Config = jsonBuffer.parseObject(ConfigFile);
	String text;
	Config.printTo(text);
	data2log("config:\n");
	data2log(text.c_str());
	data2log("\n");
	ConfigFile.close();
}
