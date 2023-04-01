#include "EspNowAgent.h"
EspNowAgent *EspNowAgent::instance = NULL;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  EspNowAgent::Instance()->onDataSent(mac_addr, status);
}
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  EspNowAgent::Instance()->onDataRecv(mac_addr, incomingData, len);
}