#ifndef WIFI_COM_H
#define WIFI_COM_H

#include <WiFi.h>
#include <esp_now.h>
#include <functional>

#define WIFI_CHANNEL 0

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len);

typedef std::function<void(const uint8_t *incomingData, int len)> TOnDataReceived;

class EspNowAgent {
private:
  static EspNowAgent* instance;
  TOnDataReceived externalDataReceivedCallback;
public:
  bool initialized = false;
  EspNowAgent(){
    if(instance!=NULL){
      Serial.println("Error: Only one instance of EspNowAgent is allowed");
      delay(500);
      esp_restart();
    }
    instance = this;
  }

  static EspNowAgent* Instance(){
    if(instance == NULL){
      instance = new EspNowAgent();
    }
    return instance;
  }

  void registerCallback(TOnDataReceived externalDataReceivedCallback){
    this->externalDataReceivedCallback = externalDataReceivedCallback;
  }

  bool initialize(){
    if(initialized)
      return true;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_STA);
    delay(25);
    
    String mac = WiFi.macAddress();
    Serial.print("Station address: ");
    Serial.print(mac);
    Serial.println();

    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return false;
    }

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    for (int ii = 0; ii < 6; ++ii) {
      peerInfo.peer_addr[ii] = (uint8_t)0xff;
    }
    peerInfo.channel = WIFI_CHANNEL;
    peerInfo.encrypt = false;

    esp_err_t status = esp_now_add_peer(&peerInfo);
    if (status != ESP_OK) {
      Serial.printf("Failed to add peer %X\n", status);
      return false;
    }

    status = esp_now_register_send_cb(OnDataSent);
    if (ESP_OK != status)
    {
      Serial.println("Could not register send callback");
      return false;
    }

    status = esp_now_register_recv_cb(OnDataRecv);
    if (ESP_OK != status)
    {
      Serial.println("Could not register receive callback");
      return false;
    }
    initialized = true;
    return true;
  }

  void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if(ESP_NOW_SEND_SUCCESS != status){
      Serial.printf("Failed to send last packet (to: %02X:%02X:%02x:%02X:%02X:%02X)\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    }
  }

  void onDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    Serial.printf("Bytes received: %i\r\n", len);
    Serial.printf(" (from: %02X:%02X:%02x:%02X:%02X:%02X)\r\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.write(incomingData, len);
    Serial.println();
    if(this->externalDataReceivedCallback != NULL){
      this->externalDataReceivedCallback(incomingData, len);
    }    
  }

  bool sendData (const uint8_t * data, size_t size)
  {
    uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    return esp_now_send(broadcastAddress, data, size) == ESP_OK;
  }

  // could fill a buffer to send data on loop if idle or buffer full
  void sendDataAsync (const uint8_t * data, size_t size)
  {
    initialize();
    sendData(data, size);
  }

  void loop(){
    // could wait for buffer full, or burst handling
  }
};
#endif