#include <SoftwareSerial.h>
const int numberOfPorts=8;

SoftwareSerial serialArray[numberOfPorts] = {  
  SoftwareSerial(3,11),
  SoftwareSerial(4,11),
  SoftwareSerial(5,11),
  SoftwareSerial(6,11),
  SoftwareSerial(7,11),
  SoftwareSerial(8,11),
  SoftwareSerial(9,11),  
  SoftwareSerial(10,11)
  };

//10, 11, 12, 13, 14, 15, 50, 51,*** 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69). 
//SoftwareSerial s(8,11);


unsigned long previousMillis = 0; 
const long maxWaitInterval = 1000; 
unsigned long currentMillis ;


void setSerial(int port){
serialArray[port].setTimeout(maxWaitInterval); 
serialArray[port].begin(9600);

  }


void setup() {
//init serials
  for(int i=0;i<numberOfPorts;i++){    
    setSerial(i);    
    }

currentMillis = millis();
  // put your setup code here, to run once:
 Serial.begin(115200);       
 Serial.println("-------------------------------------");
 Serial.println("boot event");
 Serial.println("-------------------------------------");
}

 
void loop() {
  for(int i=0;i<numberOfPorts;i++){
    readBytes(i);    
    }
}

long readByte;
// our sequence
//125-16-3-0-36-12-0-1-0-114-0-0-0-0-24-0-0-17-223-13-
const int BUFFER_SIZE = 24;
char buf[BUFFER_SIZE];

void readBytes(int port){
  
  serialArray[port].listen();  
 // delay(100);
  readByte=0;
  if(serialArray[port].available()){
    while(readByte!=125) // waiting for start of message
    {
      // add a time constrain to spend max 2 seconds on waiting.
      currentMillis = millis();
      if (currentMillis - previousMillis >= maxWaitInterval) {
      previousMillis=currentMillis;
      return;
    }

    delay(10); // removing this delay screws up readed data WTF
    readByte=serialArray[port].read();  
    //   if(    readByte>0)   Serial.print(String(readByte, DEC) + ", ");
    //   Serial.println(String("byte read at start routine, port: " + String(portNumber) + " value: " + String(readByte) + " currentMillis: " + String(currentMillis)););
    }
    
    // so lets read and check if we have 16 (message lenght)
    readByte=serialArray[port].read();
   // Serial.print("\nbyte read at check routine: ");
   // Serial.println(readByte);
    if (readByte!=16)
    {
      return;
    }
  
    // now we read 2 bytes
    int rlen = serialArray[port].readBytes(buf, BUFFER_SIZE);
    
    /*
    Serial.print("buffer dump: ");
    for(int v=0; v<rlen;v++) {
      Serial.print(String(buf[v], HEX) + ", ");  
    }
     */
      Serial.println();
      decodeBuffer(port);
    }
}

int bufferCountr=0;
float current;
float batV;
float PvV;
int temp;
void decodeBuffer(int port){
        batV = float(processByte(6) * 256 + processByte(7))/10;
        PvV = float(processByte(8) * 256 + processByte(9))/10;
        current = float(processByte(10) * 256 + processByte(11))/10;
        temp = processByte(12);
        String portName = String(port+1);
        Serial.print(portName+ "-current:");
        Serial.println(current);
        Serial.print(portName + "-battery:");
        Serial.println(batV);
        Serial.print(portName + "-PvV:");
        Serial.println(PvV);
        Serial.print(portName + "-temp:");
        Serial.println(temp);
}

char *ptr;
long processByte(int pointer){
        String a = String(buf[pointer], HEX);        
        if(a.length()>2){          
          a = a.substring(a .length()-2);
          return strtoul(a.c_str(), &ptr, 16);          
          }
  else{
    return (long)buf[pointer];
    }
  
  }
