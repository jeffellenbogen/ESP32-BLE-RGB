/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_ONOFF "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_REDLED "f381834e-72c4-401d-a3e0-9d74517d655c"
#define CHARACTERISTIC_UUID_GREENLED "0e551086-e27b-4da6-b153-d76a3d92d406"
#define CHARACTERISTIC_UUID_BLUELED "1deb85de-e24c-4d90-ba40-5a8042139efc"

#define REDLEDPIN 25
#define GREENLEDPIN 26
#define BLUELEDPIN 27

typedef enum{LEDoff, LEDon, LEDflash} LEDmodeType;
int flashLength = 50;

int redValue = 75;
int greenValue = 0;
int blueValue = 230;

int freq =5000;
int ledChannelRed = 0;
int ledChannelGreen = 1;
int ledChannelBlue = 2;
int resolution = 8;


LEDmodeType LEDmode = LEDflash;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer)
  {
    Serial.println("Device Connected");
    flashLength = 500;
  }

  void onDisconnect(BLEServer* pServer)
  {
     Serial.println("Device Disconnected");
     flashLength = 50;
     LEDmode = LEDflash;
  }
};

class MyCallbacksONOFF: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() == 1) {
        if (value[0]=='0')
        {
          Serial.println("Turn Off LED");
          LEDmode = LEDoff;
        }
        else if (value[0]=='1')
        {
          Serial.println("Turn ON LED");
          LEDmode = LEDon;
        }       
        else
        {
          Serial.println("Error: Bad ON/OFF message, not 0 or 1");
          LEDmode = LEDflash;
        }    
      }
      else
      {
        Serial.println("Error: ON/OFF message too long");
        LEDmode = LEDflash;
      }    
    }
};

class MyCallbacksSetRED: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.print("value.c_str() ");
      Serial.print(value.c_str());
      //redValue = value.c_str();
      redValue = atoi( value.c_str() );
      if ((redValue < 0) || (redValue > 255))
      {
        Serial.println(" : Bad Red Value input");
      }
      else
      {
        Serial.print(" : Red set to ");
        Serial.println(redValue);
      }      
    }
};

class MyCallbacksSetGREEN: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.print("value.c_str() ");
      Serial.print(value.c_str());
      //greenValue = value.c_str();
      greenValue = atoi( value.c_str() );
      if ((greenValue < 0) || (greenValue > 255))
      {
        Serial.println(" : Bad Green Value input");
      }
      else
      {
        Serial.print(" : Green set to ");
        Serial.println(greenValue);
      }  
    }
};

class MyCallbacksSetBLUE: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.print("value.c_str() ");
      Serial.print(value.c_str());
      //blueValue = value.c_str();
      blueValue = atoi( value.c_str() );
      if ((blueValue < 0) || (blueValue > 255))
      {
        Serial.println(" : Bad Blue Value input");
      }
      else
      {
        Serial.print(" : Blue set to ");
        Serial.println(blueValue);
      }       
    }
};


void setup() {
  int colorDigits;
  uint8_t initColorStatus[3];
  Serial.begin(115200);

  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");

  BLEDevice::init("Jeff's ESP32 Thing");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharONOFF = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_ONOFF,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );


  BLECharacteristic *pCharRedLED = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_REDLED,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                       
  BLECharacteristic *pCharGreenLED = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_GREENLED,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  BLECharacteristic *pCharBlueLED = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_BLUELED,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharONOFF->setCallbacks(new MyCallbacksONOFF());
  initColorStatus[0]=(LEDmode+'0');
  pCharONOFF->setValue(initColorStatus,1);
  
  pCharRedLED->setCallbacks(new MyCallbacksSetRED());
  colorDigits = setColorInitValue(redValue, initColorStatus);
  pCharRedLED->setValue(initColorStatus,colorDigits);

  pCharGreenLED->setCallbacks(new MyCallbacksSetGREEN());
  colorDigits = setColorInitValue(greenValue, initColorStatus);
  pCharGreenLED->setValue(initColorStatus,colorDigits);

  pCharBlueLED->setCallbacks(new MyCallbacksSetBLUE());
  colorDigits = setColorInitValue(blueValue, initColorStatus);
  pCharBlueLED->setValue(initColorStatus,colorDigits);
  
  /* old version of setting initial colorValues
  pCharGreenLED->setCallbacks(new MyCallbacksSetGREEN());
  colorDigits = 1;
  initColorStatus[2]=greenValue%10+'0';
  if (greenValue >= 10)
  {
    initColorStatus[1]=(greenValue%100)/10+'0';  
    colorDigits++; 
  }
  if (greenValue >= 100)
  {
   initColorStatus[0]=greenValue/100+'0'; 
   colorDigits++;
  }
  pCharGreenLED->setValue(initColorStatus,colorDigits);
  */
  
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  pinMode(5,OUTPUT);

  ledcSetup(ledChannelRed, freq, resolution);
  ledcAttachPin(REDLEDPIN, ledChannelRed);
  ledcSetup(ledChannelGreen, freq, resolution);
  ledcAttachPin(GREENLEDPIN, ledChannelGreen);
  ledcSetup(ledChannelBlue, freq, resolution);
  ledcAttachPin(BLUELEDPIN, ledChannelBlue);

   ledcWrite(ledChannelRed, 255);
   ledcWrite(ledChannelGreen, 0);
   ledcWrite(ledChannelBlue, 0);  
   delay(500);  
   
   ledcWrite(ledChannelRed, 0);
   ledcWrite(ledChannelGreen, 255);
   ledcWrite(ledChannelBlue, 0); 
   delay(500);  
   
   ledcWrite(ledChannelRed, 0);
   ledcWrite(ledChannelGreen, 0);
   ledcWrite(ledChannelBlue, 255);    
   delay(500);  
   
   ledcWrite(ledChannelRed, 255);
   ledcWrite(ledChannelGreen, 255);
   ledcWrite(ledChannelBlue, 255);  
}

void loop() {

  uint32_t flashTimer;
  static uint32_t lastTime = 0;
  static int lastLEDState = 0;
  // put your main code here, to run repeatedly:
  flashTimer = millis();
  if (LEDmode == LEDoff)
  {
    digitalWrite(5, LOW);
    ledcWrite(ledChannelRed, 0);
    ledcWrite(ledChannelGreen, 0);
    ledcWrite(ledChannelBlue, 0);     
  }
  else if (LEDmode == LEDon)
  {
    digitalWrite(5,HIGH);
    ledcWrite(ledChannelRed, redValue);
    ledcWrite(ledChannelGreen, greenValue);
    ledcWrite(ledChannelBlue, blueValue);
  }
  else
  {
    if (flashTimer - lastTime > flashLength)
    {
      lastLEDState = !lastLEDState;
      digitalWrite(5,lastLEDState);
      if (lastLEDState == HIGH)
      {
         ledcWrite(ledChannelRed, redValue);
         ledcWrite(ledChannelGreen, greenValue);
         ledcWrite(ledChannelBlue, blueValue);
      }
      else
      {
         ledcWrite(ledChannelRed, 0);
         ledcWrite(ledChannelGreen, 0);
         ledcWrite(ledChannelBlue, 0);  
      }
      lastTime = millis();
    }
  } 
}

int setColorInitValue(int colorInitValue, uint8_t outputArray[])
{
  int arrayIndex = 0;

  if ((colorInitValue < 0) || (colorInitValue > 255)) // check for improper R,G, or B colorValues
  {
    Serial.println("colorInitValue ERROR!");
    return -1;
  }
  
  if (colorInitValue >= 100) // we have a 3 digit color value
  {
    outputArray[arrayIndex]=colorInitValue/100+'0'; // put the first digit into array[0]

    arrayIndex++;
  }
  
  if (colorInitValue >= 10) // we have either a 3 or 2 digit color value
  {
    outputArray[arrayIndex]=(colorInitValue%100)/10+'0';  // put the tens digit in either array[0] (2 digit color) or array[1] (3 digit color)

    arrayIndex++;
  } 

  outputArray[arrayIndex]=colorInitValue%10+'0'; // we have either a 3 or 2 or 1 digit color value and we are putting the ones digit in the appropriate slot
  // of the array. one digit goes in array[0] (1 digit color), array[1] (2 digit color), array[2] (3 digit color)
  
  return arrayIndex + 1; // arrayIndex + 1 = number of digits of the colorValue (one more than the arrayIndex)
}
