#include "Gps.h"

GpsModule::GpsModule(TOpenFile getFile, int rxPin, int txPin) {
  getGpsLogFile = getFile;
  lastGpsFix = -1;
  longitude = -1;
  latitude = -1;
  course = -1;
  altitude = -1;
  speed_mps = -1;
  hdop = -1;
  satellites = -1;
  this->rxPin = rxPin;
  this->txPin = txPin;
}

void GpsModule::setup(){
  gpsSerial.begin(GPSBAUD, SWSERIAL_8N1, rxPin, txPin, false);
  
  //Settings Array contains the following settings: [0]NavMode, [1]DataRate1, [2]DataRate2, [3]PortRateByte1, [4]PortRateByte2, [5]PortRateByte3,
  //[6]NMEA GLL Sentence, [7]NMEA GSA Sentence, [8]NMEA GSV Sentence, [9]NMEA RMC Sentence, [10]NMEA VTG Sentence
  //NavMode:
  //Pedestrian Mode    = 0x03
  //Automotive Mode    = 0x04
  //Sea Mode           = 0x05
  //Airborne < 1G Mode = 0x06
  //
  //DataRate:
  //1Hz     = 0xE8 0x03
  //2Hz     = 0xF4 0x01
  //3.33Hz  = 0x2C 0x01
  //4Hz     = 0xFA 0x00
  //
  //PortRate:
  //4800   = C0 12 00
  //9600   = 80 25 00
  //19200  = 00 4B 00  **SOFTWARESERIAL LIMIT FOR ARDUINO UNO R3!**
  //38400  = 00 96 00  **SOFTWARESERIAL LIMIT FOR ARDUINO MEGA 2560!**
  //57600  = 00 E1 00
  //115200 = 00 C2 01
  //230400 = 00 84 03
  //
  //NMEA Messages:
  //OFF = 0x00
  //ON  = 0x01
  //
  byte settingsArray[] = {0x03, 0xFA, 0x00, 0x00, 0xE1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //
  configureUblox(settingsArray);
}

void GpsModule::loop(){
  while(gpsSerial.available())
  {
    char c = gpsSerial.read();
    if(logging){
      Serial.print(c);
    }

    if(gpsPlus.encode(c) && gpsPlus.location.isValid() && gpsPlus.location.isUpdated()){
      lastGpsFix = millis();
      float new_longitude;
      float new_latitude; 
      
      if(gpsPlus.satellites.isValid()){
        satellites = gpsPlus.satellites.value();
      }
      if(gpsPlus.hdop.isValid()){
        hdop = gpsPlus.hdop.value();
      }
      if(gpsPlus.location.isValid()){
        new_latitude = gpsPlus.location.lat();
        new_longitude = gpsPlus.location.lng();
      }
      if(gpsPlus.altitude.isValid()){
        altitude = gpsPlus.altitude.meters();
      }

      if (gpsPlus.course.isValid()){
        course = gpsPlus.course.deg();
      }
      if (gpsPlus.speed.isValid()){
        speed_mps = gpsPlus.speed.mps();
      }

      if(gpsPlus.date.isValid()){
        day = gpsPlus.date.day();
        month = gpsPlus.date.month();
        year = gpsPlus.date.year();
      }
      
      if(gpsPlus.time.isValid()){
        hour = gpsPlus.time.hour();
        minute = gpsPlus.time.minute();
        second = gpsPlus.time.second();
        hundredths = gpsPlus.time.centisecond();
      }

      // logf("charsProcessed: %i", gpsPlus.charsProcessed());
      // logf("sentencesWithFix: %i", gpsPlus.sentencesWithFix());
      // logf("failedChecksum: %i", gpsPlus.failedChecksum());

      if(!outputedDate || logGpsData){
        outputedDate = true;
        logf("GPS Data - %02u/%02u/%4u %02u:%02u:%02u.%03u; %11.8f; %11.8f; %i; %i", (int)day, month, year, hour, minute, second, hundredths, latitude, longitude, hdop, satellites);
      }

      if(new_latitude != latitude || new_longitude != longitude){
        latitude = new_latitude;
        longitude = new_longitude;
        File log = getGpsLogFile();

        log.printf("%02u/%02u/%4u %02u:%02u:%02u.%03u; %11.8f; %11.8f; %i; %i", (int)day, month, year, hour, minute, second, hundredths, latitude, longitude, hdop, satellites);
        log.close();
      }
    }    
  }
}

int GpsModule::delayFix(){
  return millis() - lastGpsFix;
}

typedef std::function<void()> TLineFunction;

void GpsModule::displayScreenPage(IDisplay *display, int position){
  display->blackOnWhite();
  displayPrintline(display, this->getName());
  display->whiteOnBlack();

  std::vector<TLineFunction> lines;
  GpsModule *that = this;

  lines.push_back([&](){
    displayPrintline(display, "Last fix:", that->delayFix(), " ms");
  });
  lines.push_back([&](){
    displayPrintline(display, "#Sat:", that->satellites, " - HDOP: ", that->hdop);
  });
  lines.push_back([&](){
    displayPrintline(display, "Lat: ", that->latitude);
  });
  lines.push_back([&](){
    displayPrintline(display, "Long: ", that->longitude);
  });
  lines.push_back([&](){
    displayPrintline(display, "Altitude: ", that->speed_mps, " m");
  });
  lines.push_back([&](){
    displayPrintline(display, "Speed: ", that->speed_mps, " m/s");
  });
  lines.push_back([&](){
    displayPrintline(display, "Course: ", that->course);
  }); 

  for(int i=0; i<3; i++){
    lines.at(i + (position * 3) % lines.size())();
  }
}

void GpsModule::configureUblox(byte *settingsArrayPointer) {
  byte gpsSetSuccess = 0;
  logf("Configuring u-Blox GPS initial state...");

  //Generate the configuration string for Navigation Mode
  byte setNav[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, *settingsArrayPointer, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  calcChecksum(&setNav[2], sizeof(setNav) - 4);

  //Generate the configuration string for Data Rate
  byte setDataRate[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, settingsArrayPointer[1], settingsArrayPointer[2], 0x01, 0x00, 0x01, 0x00, 0x00, 0x00};
  calcChecksum(&setDataRate[2], sizeof(setDataRate) - 4);

  //Generate the configuration string for Baud Rate
  byte setPortRate[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, settingsArrayPointer[3], settingsArrayPointer[4], settingsArrayPointer[5], 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  calcChecksum(&setPortRate[2], sizeof(setPortRate) - 4);

  byte setGLL[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B};
  byte setGSA[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32};
  byte setGSV[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39};
  byte setRMC[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x40};
  byte setVTG[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46};

  delay(2500);

  while(gpsSetSuccess < 3)
  {
    logf("Setting Navigation Mode... ");
    sendUBX(&setNav[0], sizeof(setNav));  //Send UBX Packet
    gpsSetSuccess += getUBX_ACK(&setNav[2]); //Passes Class ID and Message ID to the ACK Receive function
    if (gpsSetSuccess == 5) {
      gpsSetSuccess -= 4;
      setBaud(settingsArrayPointer[4]);
      delay(1500);
      byte lowerPortRate[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xB5};
      sendUBX(lowerPortRate, sizeof(lowerPortRate));
      gpsSerial.begin(9600);
      delay(2000);
    }
    if(gpsSetSuccess == 6) gpsSetSuccess -= 4;
    if (gpsSetSuccess == 10) gpsStatus[0] = true;
  }
  if (gpsSetSuccess == 3) logf("Navigation mode configuration failed.");
  gpsSetSuccess = 0;
  while(gpsSetSuccess < 3) {
    logf("Setting Data Update Rate... ");
    sendUBX(&setDataRate[0], sizeof(setDataRate));  //Send UBX Packet
    gpsSetSuccess += getUBX_ACK(&setDataRate[2]); //Passes Class ID and Message ID to the ACK Receive function
    if (gpsSetSuccess == 10) gpsStatus[1] = true;
    if (gpsSetSuccess == 5 | gpsSetSuccess == 6) gpsSetSuccess -= 4;
  }
  if (gpsSetSuccess == 3) logf("Data update mode configuration failed.");
  gpsSetSuccess = 0;


  while(gpsSetSuccess < 3 && settingsArrayPointer[6] == 0x00) {
    logf("Deactivating NMEA GLL Messages ");
    sendUBX(setGLL, sizeof(setGLL));
    gpsSetSuccess += getUBX_ACK(&setGLL[2]);
    if (gpsSetSuccess == 10) gpsStatus[2] = true;
    if (gpsSetSuccess == 5 | gpsSetSuccess == 6) gpsSetSuccess -= 4;
  }
  if (gpsSetSuccess == 3) logf("NMEA GLL Message Deactivation Failed!");
  gpsSetSuccess = 0;

  while(gpsSetSuccess < 3 && settingsArrayPointer[7] == 0x00) {
    logf("Deactivating NMEA GSA Messages ");
    sendUBX(setGSA, sizeof(setGSA));
    gpsSetSuccess += getUBX_ACK(&setGSA[2]);
    if (gpsSetSuccess == 10) gpsStatus[3] = true;
    if (gpsSetSuccess == 5 | gpsSetSuccess == 6) gpsSetSuccess -= 4;
  }
  if (gpsSetSuccess == 3) logf("NMEA GSA Message Deactivation Failed!");
  gpsSetSuccess = 0;

  while(gpsSetSuccess < 3 && settingsArrayPointer[8] == 0x00) {
    logf("Deactivating NMEA GSV Messages ");
    sendUBX(setGSV, sizeof(setGSV));
    gpsSetSuccess += getUBX_ACK(&setGSV[2]);
    if (gpsSetSuccess == 10) gpsStatus[4] = true;
    if (gpsSetSuccess == 5 | gpsSetSuccess == 6) gpsSetSuccess -= 4;
  }
  if (gpsSetSuccess == 3) logf("NMEA GSV Message Deactivation Failed!");
  gpsSetSuccess = 0;

  while(gpsSetSuccess < 3 && settingsArrayPointer[9] == 0x00) {
    logf("Deactivating NMEA RMC Messages ");
    sendUBX(setRMC, sizeof(setRMC));
    gpsSetSuccess += getUBX_ACK(&setRMC[2]);
    if (gpsSetSuccess == 10) gpsStatus[5] = true;
    if (gpsSetSuccess == 5 | gpsSetSuccess == 6) gpsSetSuccess -= 4;
  }
  if (gpsSetSuccess == 3) logf("NMEA RMC Message Deactivation Failed!");
  gpsSetSuccess = 0;

  while(gpsSetSuccess < 3 && settingsArrayPointer[10] == 0x00) {
    logf("Deactivating NMEA VTG Messages ");
    sendUBX(setVTG, sizeof(setVTG));
    gpsSetSuccess += getUBX_ACK(&setVTG[2]);
    if (gpsSetSuccess == 10) gpsStatus[6] = true;
    if (gpsSetSuccess == 5 | gpsSetSuccess == 6) gpsSetSuccess -= 4;
  }
  if (gpsSetSuccess == 3) logf("NMEA VTG Message Deactivation Failed!");

  gpsSetSuccess = 0;
  if (settingsArrayPointer[4] != 0x25) {
    logf("Setting Port Baud Rate... ");
    sendUBX(&setPortRate[0], sizeof(setPortRate));
    setBaud(settingsArrayPointer[4]);
    logf("Success!");
    delay(500);
  }
}


void GpsModule::calcChecksum(byte *checksumPayload, byte payloadSize) {
  byte CK_A = 0, CK_B = 0;
  for (int i = 0; i < payloadSize ;i++) {
    CK_A = CK_A + *checksumPayload;
    CK_B = CK_B + CK_A;
    checksumPayload++;
  }
  *checksumPayload = CK_A;
  checksumPayload++;
  *checksumPayload = CK_B;
}

void GpsModule::sendUBX(byte *UBXmsg, byte msgLength) {
  for(int i = 0; i < msgLength; i++) {
    gpsSerial.write(UBXmsg[i]);
    gpsSerial.flush();
  }
  gpsSerial.println();
  gpsSerial.flush();
}


byte GpsModule::getUBX_ACK(byte *msgID) {
  byte CK_A = 0, CK_B = 0;
  byte incoming_char;
  boolean headerReceived = false;
  unsigned long ackWait = millis();
  byte ackPacket[10] = {0xB5, 0x62, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  int i = 0;
  while (1) {
    if (gpsSerial.available()) {
      incoming_char = gpsSerial.read();
      if (incoming_char == ackPacket[i]) {
        i++;
      }
      else if (i > 2) {
        ackPacket[i] = incoming_char;
        i++;
      }
    }
    if (i > 9) break;
    if ((millis() - ackWait) > 1500) {
      logf("ACK Timeout");
      return 5;
    }
    if (i == 4 && ackPacket[3] == 0x00) {
      logf("NAK Received");
      return 1;
    }
  }

  for (i = 2; i < 8 ;i++) {
  CK_A = CK_A + ackPacket[i];
  CK_B = CK_B + CK_A;
  }
  if (msgID[0] == ackPacket[6] && msgID[1] == ackPacket[7] && CK_A == ackPacket[8] && CK_B == ackPacket[9]) {
    logf("Success ACK Received! ");
    //printHex(ackPacket, sizeof(ackPacket));
    return 10;
        }
  else {
    logf("ACK Checksum Failure: ");
    //printHex(ackPacket, sizeof(ackPacket));
    delay(1000);
    return 1;
  }
}


void GpsModule::printHex(uint8_t *data, uint8_t length) // prints 8-bit data in hex
{
  char tmp[length*2+1];
  byte first ;
  int j=0;
  for (byte i = 0; i < length; i++)
  {
    first = (data[i] >> 4) | 48;
    if (first > 57) tmp[j] = first + (byte)7;
    else tmp[j] = first ;
    j++;

    first = (data[i] & 0x0F) | 48;
    if (first > 57) tmp[j] = first + (byte)7;
    else tmp[j] = first;
    j++;
  }
  tmp[length*2] = 0;
  for (byte i = 0, j = 0; i < sizeof(tmp); i++) {
    Serial.print(tmp[i]);
    if (j == 1) {
      Serial.print(" ");
      j = 0;
    }
    else j++;
  }
  Serial.println();
}

void GpsModule::setBaud(byte baudSetting) {
  if (baudSetting == 0x12) gpsSerial.begin(4800);
  if (baudSetting == 0x4B) gpsSerial.begin(19200);
  if (baudSetting == 0x96) gpsSerial.begin(38400);
  if (baudSetting == 0xE1) gpsSerial.begin(57600);
  if (baudSetting == 0xC2) gpsSerial.begin(115200);
  if (baudSetting == 0x84) gpsSerial.begin(230400);
}