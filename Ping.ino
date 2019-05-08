#define MORSE_DOT  '*'
#define MORSE_TIRE  '-'
#define DOT_DURATION 100
#define TIRE_DURATION 200
#define PAUSE_DURATION 100
#define PAUSE_BTWN_SBLS 70

// Максимальная длина символа азбуки Морзе (в точках и тире)
#define MAX_MORSE_SYMBOL_LENGTH  8
// Буфер для записи морзе-символа.
byte morseSymbol[MAX_MORSE_SYMBOL_LENGTH];
unsigned int morseSymbolLen;

// Таблица кодов Морзе. N-ный элемент кода соответствует n-ному символу раскладки.
char* codeMorse[] = {
  "*-","-***","*--","--*","-**","*","***-","--**","**","*---",
  "-*-","*-**","--","-*","---","*--*","*-*","***","-","**-",
  "**-*","****","-*-*","---*","----","--*-","-*--","-**-","**-**","**--",
  "*-*-",
  "*----","**---","***--","****-","*****","-****","--***","---**","----*","-----",
  "......","*-*-*-","---***","-*-*-","-*--*-","*----*","*-**-*","-****-","-**-*","**--**","--**--",
  "-***-","********","*--*-*","**-*-",
  ""
};
char layoutCyrillic[52] = {
  'а','б','в','г','д','е','ж','з','и','й',
  'к','л','м','н','о','п','р','с','т','у',
  'ф','х','ц','ч','ш','щ','ы','ь','э','ю',
  'я',
  '1','2','3','4','5','6','7','8','9','0',
  '.',',',':',';','(','\'','\'','-','/','?',
  '!'
};
const int layoutCyrillicCount = 52;

//// Кириллическая раскладка.
//char* layoutCyrillic[] = {
//  "а","б","в","г","д","е","ж","з","и","й",
//  "к","л","м","н","о","п","р","с","т","у",
//  "ф","х","ц","ч","ш","щ","ы","ь","э","ю",
//  "я",
//  "1","2","3","4","5","6","7","8","9","0",
//  ".",",",":",";","(","\'","\"","-","/","?",
//  "!"," *DELIMITER* "," *ERR* ","@"," *END* ",
//  ""
//  };
// Латинская раскладка.
//char* layoutLatin[] = {
//  "a","b","w","g","d","e","v","z","i","j",    //10
//  "k","l","m","n","o","p","r","s","t","u",    //20
//  "f","h","c","ö","ch","q","y","x","é","ü",   //30
//  "ä",                                        //31
//  "1","2","3","4","5","6","7","8","9","0",    //41
//  ".",",",":",";","(","\'","\"","-","/","?",  //51
//  "!"                                         //52
//  //," *DELIMITER* "," *ERR* ","@"," *END* ", //57
//  //""
//};



void pingTone()
{
  const int pingsCount = 10;
  const int milisec = 100;
  for (int i = 0; i <pingsCount; i++)
  {
    digitalWrite(speakerPin, LOW);
    delay(milisec);
    digitalWrite(speakerPin, HIGH);
    delay(milisec);
  }
}
void pingOn()
{
  digitalWrite(speakerPin, LOW);
}
void pingOff()
{
  digitalWrite(speakerPin, HIGH);
}

void pingVarTone(float persent, const int fullDuration, const int maxpings, const int pause)
{
  if (persent > 1)
    persent = 1;
  int PingCount = int(maxpings*(1 - persent));
  if (PingCount == 0)
    PingCount = 1;
  int duration = int(fullDuration/PingCount - pause);
  for (int i = 0; i < PingCount; ++i)
  {
    /* 
    * PingCount*(pause + duration) = fullDuration
    * PingCount = maxpings*(1 - persent)
     */
    digitalWrite(speakerPin, LOW);
    delay(duration);
    digitalWrite(speakerPin, HIGH);
    delay(pause);
  }
}
void setupTone()
{
  pinMode(speakerPin, OUTPUT);
  digitalWrite(speakerPin, HIGH);
}
void cyrillic2morse(String text)
{
  for (int i = 0; i < text.length();i++)
  {
    for (int j = 0; j < layoutCyrillicCount; j++)
    {
      if(text[i] == layoutCyrillic[j])
      {
        for (int k = 0; ((k < MAX_MORSE_SYMBOL_LENGTH)&&(codeMorse[j][k] != '\0')); k++)
        {
          if (codeMorse[j][k] == MORSE_DOT)
            pingMorse(DOT_DURATION);
          if (codeMorse[j][k] == MORSE_TIRE)
            pingMorse(TIRE_DURATION);
        }
        
      }
    }
    delay(PAUSE_BTWN_SBLS);
  }
}
void pingMorse(const int milisec)
{
    digitalWrite(speakerPin, LOW);
    delay(milisec);
    digitalWrite(speakerPin, HIGH);
    delay(PAUSE_DURATION);
}

