#include <SoftwareSerial.h>
#include <IRremote.h>
 
#define DEBUG true
#define MAX_ARRAY_SIZE 5
#define MIN_LAP_TIME 5000 // milliseconds


class RollingArray
{
public:
  int Size;
  unsigned long Id;
  unsigned long TimeStamps[MAX_ARRAY_SIZE] = {0};
  RollingArray();
  void add(unsigned long t);
  String Html();
  String ToString();
}
;
 
SoftwareSerial esp8266(4,5); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
//SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
                             
int RECV_PIN = 11;
int SEND_PIN = 13;
int LED_PIN = 12;



IRrecv irrecv(RECV_PIN);

decode_results results;

RollingArray racers[5];

unsigned long timeStamps[MAX_ARRAY_SIZE] = {0};
unsigned int timeSize = 0;

void AddTimestamp(unsigned long t, unsigned long value)
{
  //racers[0].add(t);
  //Serial.println(racers[0].ToString());
  bool found = false;
  for(int i=0;i<5;i++)
  {
    if(racers[i].Id == value)
    {
      found = true;
      racers[i].add(t);
      Serial.println(racers[i].ToString());
      return;
    }
    
    if(racers[i].Id == 0)
    {
      // empty 
      racers[i].Id = value;
      racers[i].add(t);
      Serial.println(racers[i].ToString());
      return;
    }
  }
}

void PrintTimestamps()
{
  int i = 0;
      Serial.print("==> ");
  for(int i=0;i<timeSize;i++)
  {
    Serial.print(timeStamps[i]);
        Serial.print(", ");
  }
}

void setup()
{
  Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different
  
 
  
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  
  WriteToLed(1,LED_PIN);
    irrecv.enableIRIn(); // Start the receiver
    
    // racers[0].Id = 1;
}

void WriteToLed(int value, int pin)
{
    
          pinMode(pin, OUTPUT);  
      // Send the LED Command
      if(value == 1)
      {
         digitalWrite(pin, HIGH);    // turn the LED off by making the voltage LOW
      }
      else
      {
        digitalWrite(pin, LOW);    // turn the LED off by making the voltage LOW
        //delayMicroseconds(500);
        delay(50);
        //SendCode();
        digitalWrite(pin, HIGH);
        irrecv.enableIRIn(); // Re-enable receiver
      }
}


void handleIRSensor()
{
  unsigned long time;
  if (irrecv.decode(&results)) {
    time = millis();
    

    
    
            Serial.println("values");
            //PrintTimestamps();
                        Serial.println("done");
        Serial.print("Pin:");
                Serial.print(results.decode_type);
                        Serial.print(":Val:");
    Serial.print(results.value);
    Serial.print(" Buff : ");
  /*  unsigned long decodedVal = 0;
   for(int i=1;i<results.rawlen-1;i++)
   {
     decodedVal += results.rawbuf[i];
   }
   */
//   decodedVal /= 10;
//   Serial.print(decodedVal);
   Serial.println(" Done");
   
               AddTimestamp(time, results.value);
   
    WriteToLed(0,LED_PIN);
    
    irrecv.resume(); // Receive the next value
    //WriteToLed(0,SEND_PIN);
  }
}

void SendHtml()
{
     delay(1000);
 
     int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
     
     // String webpage = "<h1>Hello</h1><h2>World!</h2><button>LED1</button>";
     
     String webpage = "";
      int i = 0;
      webpage += "<b>";
      /*unsigned long init_time = timeStamps[0];
      for(int i=1;i<timeSize;i++)
      {
        unsigned long laptime = timeStamps[i] - init_time;
        unsigned long seconds = laptime/1000;
        unsigned long deci = laptime%1000;
        deci = deci/10;
        webpage+= seconds;
        webpage+=".";
        webpage+= deci;
            webpage += "<br>";
      }*/
      for(int i=0;i<5;i++)
      {
        webpage += racers[i].Html();
        webpage += "<hr>";
      }
      webpage += "</b>";
 
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);
     
     /*webpage="<button>LED2</button>";
     
     cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend +=webpage.length();
     cipSend +="\r\n";
     
     sendData(cipSend,1000,DEBUG);
     sendData(webpage,1000,DEBUG);*/
 
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendData(closeCommand,3000,DEBUG);
}
 
void loop()
{
  handleIRSensor();
  if(esp8266.available()) // check if the esp is sending a message 
  {
    /*
    while(esp8266.available())
    {
      // The esp has data so display its output to the serial window 
      char c = esp8266.read(); // read the next character.
      Serial.write(c);
    } */
    
    if(esp8266.find("+IPD,"))
    {
     SendHtml();
    }
  }
}
 
 
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}


/************* Rolling Array ******************/
RollingArray::RollingArray()
{
  Size = 0;
  Id = 0;
}

void RollingArray::add(unsigned long t)
{
    if(Size > 0)
    {
      // check if this can be added
      if(t - TimeStamps[Size-1] < MIN_LAP_TIME)
      {
        return; // cannot add this 
      }
    }
    
    if(Size >= MAX_ARRAY_SIZE)
    {
      //  we need to re-shuffle
      for(int i=0;i<Size-1;i++)
      {
        TimeStamps[i]=TimeStamps[i+1];
      }
      TimeStamps[Size-1]=t;
    }
    else
    {
      TimeStamps[Size++] = t;
    }
}

String RollingArray::Html()
{
  String outStr = "<b>Racer ";
  outStr += Id;
  outStr += "<br>";
  for(int i=1;i<Size;i++)
  {
    unsigned long laptime = TimeStamps[i] - TimeStamps[i-1];
    unsigned long seconds = laptime/1000;
    unsigned long deci = laptime%1000;
    deci = deci/10;
    outStr+= seconds;
        outStr+=".";
    outStr+= deci;
        outStr += "<br>";
  }
	return outStr;
}
String RollingArray::ToString()
{
	String outStr = "";
          for(int i=1;i<Size;i++)
          {
            unsigned long laptime = TimeStamps[i] - TimeStamps[i-1];
            unsigned long seconds = laptime/1000;
            unsigned long deci = laptime%1000;
            deci = deci/10;
            outStr+= seconds;
                outStr+=".";
            outStr+= deci;
                outStr += ", ";
          }
	return outStr;
}
