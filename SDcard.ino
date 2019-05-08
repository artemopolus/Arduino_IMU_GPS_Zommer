void dirIsExist(fs::FS &fs, const char * dirname)
{
    Serial.printf("Check Dir: %s\n", dirname);
    File root = fs.open(dirname);
    if((!root)||(!root.isDirectory()))
    {
        Serial.printf("Creating Dir: %s\n", dirname);
        if(fs.mkdir(dirname)){
            Serial.println("Dir created");
        } else {
            Serial.println("mkdir failed");
        }
    }
    root.close();
}
bool isFileInDir(fs::FS &fs, const char * dirname, const char * filename)
{
    String text = "Search file ";
    text += filename;
    text += " in ";
    text += dirname;
    text += "\n";
    data2log(text.c_str());
    File root = fs.open(dirname);
    File file = root.openNextFile();
    String Target = String(dirname) +'/' + String(filename);
    //data2log(Target.c_str());
    while(file){
        if (Target == file.name())
        {
            text = "FILE: ";
            text += file.name();
            text += " SIZE: ";
            text += file.size();
            text += "\n";
            data2log(text.c_str());
            data2log("have one\n");
            return true;
        }
        file = root.openNextFile();
    }
    data2log("no  items!\n");
    return false;
}
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    SerialPrintf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        SerialPrintf("Failed to open directory\n");
        return;
    }
    if(!root.isDirectory()){
        SerialPrintf("Not a directory\n");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            SerialPrintf("  DIR : %s\n",file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            
            //Serial.print
            SerialPrintf("  FILE: %s SIZE: %s\n",file.name(),file.size());
            //Serial.print(file.name());
            //Serial.print("  SIZE: ");
            //Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
void writeFile(fs::FS &fs, const char * path, const char * message){
    SerialPrintf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        SerialPrintf("Failed to open file for writing\n");
        return;
    }
    if(file.print(message)){
        SerialPrintf("File written\n");
    } else {
        SerialPrintf("Write failed\n");
    }
    file.close();
}



bool appendFile(fs::FS &fs, const char * path, const char * message){
    //if(Serial)
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
      if(Serial)
        Serial.println("Failed to open file for appending\n");
        return false;
    }
    if(file.print(message)){
      //if(Serial)
        //Serial.println("Message appended\n");
    } else {
      if(Serial)
        Serial.println("Append failed\n");
    }
    file.close();
    return true;
}
bool open2appendFile(fs::FS &fs, const char * path, File * file)
{
    //SerialPrintf("Appending to file: %s\n", path);
    *file = fs.open(path, FILE_APPEND);
    //*file = fs.open(path, O_CREAT | O_WRITE);
    if(!(*file)){
        SerialPrintf("Failed to open file for appending\n");
        return false;
    }
    else
        return true;
}
void append2openedFile(File * file, const char * message)
{
    if(file->print(message)){
      // if (Serial){
      //   Serial.println("Message appended\n");
      // }
      // file->flush();
    } else {
      if (Serial)
        Serial.println("Append failed\n");
    }
}
void close2openedFile (File * file)
{
    file->close();
}

void readFile(fs::FS &fs, const char * path){
    SerialPrintf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        SerialPrintf("Failed to open file for reading\n");
        return;
    }

    SerialPrintf("Read from file: ");
    while(file.available()){
        //Serial.write(file.read());
        SerialPrintf("%d",file.read());
    }
    file.close();
}
bool setupSD()
{
  if(!SD.begin()){
        SerialPrintf("Card Mount Failed");
        
        return false;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        SerialPrintf("No SD card attached\n");
        return false;
    }

    SerialPrintf("SD Card Type: ");
    if(cardType == CARD_MMC){
        SerialPrintf("MMC\n");
    } else if(cardType == CARD_SD){
        SerialPrintf("SDSC\n");
    } else if(cardType == CARD_SDHC){
        SerialPrintf("SDHC\n");
    } else {
        SerialPrintf("UNKNOWN\n");
    }
    return true;
}
void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}
void deleteFilesInDir(fs::FS &fs, const char * dirname)
{
    String text = "Delete files ";
    text += " in ";
    text += dirname;
    text += "\n";
    Serial.print(text.c_str());
    File root = fs.open(dirname);
    File file = root.openNextFile();
    //String Target = String(dirname) +'/' + String(filename);
    //data2log(Target.c_str());
    while(file){
        text = "FILE: ";
        text += file.name();
        text += " SIZE: ";
        text += file.size();
        text += "\n";
        Serial.print(text.c_str());
        if(fs.remove(file.name())){
            Serial.print("File deleted\n");
        } else {
            Serial.print("Delete failed\n");
        }
        file = root.openNextFile();
    }
    return;
}
