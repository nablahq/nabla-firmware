#include <Arduino.h>

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include "data_constants.h"
#include "images_directions.h"

#include <TFT_eFEX.h>
#include <TFT_eSPI.h>

// --------
// Constants
// --------
#define PASSKEY 000000
#define DEVICE_NAME "2A67"
#define FIRMWARE_VERSION "1.0.0"
#define SERVICE_UUID "25AE1441-05D3-4C5B-8281-93D4E07420CF"
#define CHAR_WRITE_UUID "25AE1443-05D3-4C5B-8281-93D4E07420CF"
#define CHAR_READ_UUID "25AE1442-05D3-4C5B-8281-93D4E07420CF"
#define CHAR_INDICATE_UUID "25AE1444-05D3-4C5B-8281-93D4E07420CF"

// ---------------------
// Variables for display
// ---------------------
static TFT_eSPI tft = TFT_eSPI();
TFT_eFEX fex = TFT_eFEX(&tft);

// --------
// Variables for BLE
// --------
static BLEServer *g_pServer = nullptr;
static BLECharacteristic *g_pCharRead = nullptr;
static BLECharacteristic *g_pCharWrite = nullptr;
static BLECharacteristic *g_pCharIndicate = nullptr;

static bool g_centralConnected = false;
static bool g_needToHandleConnectionState = true;
static uint32_t g_lastActivityTime = 0;
static bool g_isWriteDataUpdated = false;
static String g_writeData;

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// --------
// Bluetooth event callbacks
// --------
class SecurityCallback : public BLESecurityCallbacks {

  uint32_t onPassKeyRequest() { return 000000; }

  void onPassKeyNotify(uint32_t pass_key) {}

  bool onConfirmPIN(uint32_t pass_key) {
    vTaskDelay(3000);
    return true;
  }

  bool onSecurityRequest() { return true; }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
    if (cmpl.success) {
      Serial.println("   - SecurityCallback - Authentication Success");
      tft.fillScreen(TFT_BLACK);
      tft.drawCentreString("Hello", 160, 120, 4);
    } else {
      Serial.println("   - SecurityCallback - Authentication Failure*");
      g_pServer->removePeerDevice(g_pServer->getConnId(), true);
    }
    BLEDevice::startAdvertising();
  }
};

void DrawDirection(int direction) {
  const uint8_t *imageProgmem = ImageFromNumber(direction);
  if (imageProgmem) {
    tft.drawBitmap(160 - 32, 120 - 32, imageProgmem, 64, 64, TFT_WHITE);
  }
}

void bleSecurity() {
  esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
  esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
  uint8_t key_size = 16;
  uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint32_t passkey = PASSKEY;
  uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey,
                                 sizeof(uint32_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req,
                                 sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap,
                                 sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size,
                                 sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH,
                                 &auth_option, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key,
                                 sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key,
                                 sizeof(uint8_t));
}
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    Serial.println("onConnect");
    g_lastActivityTime = millis();
    g_centralConnected = true;
    g_needToHandleConnectionState = true;
  }

  void onDisconnect(BLEServer *pServer) override {
    Serial.println("onDisconnect, will start advertising");
    g_centralConnected = false;
    g_needToHandleConnectionState = true;
    BLEDevice::startAdvertising();
  }
};

class MyCharWriteDataCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    g_lastActivityTime = millis();
    g_writeData = pCharacteristic->getValue().c_str();
    g_isWriteDataUpdated = true;
  }
};

class MyCharIndicateStatusCallbacks : public BLECharacteristicCallbacks {
  void onStatus(BLECharacteristic *pCharacteristic, Status status,
                uint32_t code) override {
    std::string event("onStatus:");
    switch (status) {
    case SUCCESS_INDICATE:
      event += "SUCCESS_INDICATE";
      break;
    case SUCCESS_NOTIFY:
      event += "SUCCESS_NOTIFY";
      break;
    case ERROR_INDICATE_DISABLED:
      event += "ERROR_INDICATE_DISABLED";
      break;
    case ERROR_NOTIFY_DISABLED:
      event += "ERROR_NOTIFY_DISABLED";
      break;
    case ERROR_GATT:
      event += "ERROR_GATT";
      break;
    case ERROR_NO_CLIENT:
      event += "ERROR_NO_CLIENT";
      break;
    case ERROR_INDICATE_TIMEOUT:
      event += "ERROR_INDICATE_TIMEOUT";
      break;
    case ERROR_INDICATE_FAILURE:
      event += "ERROR_INDICATE_FAILURE";
      break;
    }
    event += ":";
    event += String(code).c_str();
    PrintEvent(event.c_str(), nullptr);
  }

  void PrintEvent(const char *event, const char *value) {
    Serial.print(event);
    Serial.print("(CharIndicateStatus)");
    if (value) {
      Serial.print(" value='");
      Serial.print(value);
      Serial.print("'");
    }
    Serial.println();
  }
};

void PrintEvent(const char *event, const char *value) {
  Serial.print(event);
  Serial.print("(CharIndicateStatus)");
  if (value) {
    Serial.print(" value='");
    Serial.print(value);
    Serial.print("'");
  }
  Serial.println();
}

void setup_mainscreen() {

  String maneuver = getValue(g_writeData, '|', 0);
  String junctionDistance = getValue(g_writeData, '|', 1);
  String speedLimit = getValue(g_writeData, '|', 2);
  String eta = getValue(g_writeData, '|', 3);
  String progress = getValue(g_writeData, '|', 4);
  String totalDistance = getValue(g_writeData, '|', 5);
  String imageName = getValue(g_writeData, '|', 6);

  maneuver.toUpperCase();

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);

  fex.drawProgressBar(0, 0, 340, 10, progress.toInt(), TFT_BLACK, TFT_WHITE);

  tft.drawCentreString("DISTANCE", 50, 110, 2);
  tft.drawCentreString(totalDistance, 50, 125, 2);

  tft.drawCentreString("TAKE TIME", 270, 110, 2);
  tft.drawCentreString(eta + " min", 270, 125, 2);

  if (speedLimit.toInt() != 0) {
    tft.drawCentreString(speedLimit, 290, 220, 2);
    tft.drawCircle(290, 30, 220, TFT_RED);
  }

  DrawDirection(imageName.toInt());

  tft.drawCentreString(junctionDistance, 160, 160, 2);
}

void setup_welcome_screen() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("I'm " + String(DEVICE_NAME), 160, 120, 2);
  tft.drawCentreString("Anyone out there?", 160, 130, 2);
}

// TODO
std::string BuildStateData() {
  // String BuildStateData() {
  // const uint8_t screenWidth{CANVAS_WIDTH};
  // const uint8_t screenHeight{CANVAS_HEIGHT};

  // const String bytes[] = {DEVICE_NAME, FIRMWARE_VERSION};

  // const std::string strData(reinterpret_cast<const char *>(bytes),
  //                           sizeof(bytes));
  // return strData;

  Serial.println(
      (String(DEVICE_NAME) + "|" + String(FIRMWARE_VERSION)).c_str());
  return (String(DEVICE_NAME) + "|" + String(FIRMWARE_VERSION)).c_str();
}

// --------
// Application lifecycle: setup & loop
// --------
void setup() {
  Serial.begin(115200);
  Serial.println("BLE Peripheral setup started");

  // init BLE
  BLEDevice::init(DEVICE_NAME);
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  BLEDevice::setSecurityCallbacks(new SecurityCallback());

  g_pServer = BLEDevice::createServer();
  g_pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = g_pServer->createService(SERVICE_UUID);

  // characteristic for write
  {
    uint32_t propertyFlags = BLECharacteristic::PROPERTY_WRITE;
    BLECharacteristic *pCharWrite =
        pService->createCharacteristic(CHAR_WRITE_UUID, propertyFlags);
    pCharWrite->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                     ESP_GATT_PERM_WRITE_ENCRYPTED);
    pCharWrite->setCallbacks(new MyCharWriteDataCallbacks);
    pCharWrite->setValue("");
    g_pCharWrite = pCharWrite;
  }

  // characteristic for read
  {
    uint32_t propertyFlags = BLECharacteristic::PROPERTY_READ;
    BLECharacteristic *pCharRead =
        pService->createCharacteristic(CHAR_READ_UUID, propertyFlags);
    pCharRead->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                    ESP_GATT_PERM_WRITE_ENCRYPTED);
    pCharRead->setValue(BuildStateData());
    g_pCharRead = pCharRead;
  }

  // characteristic for indicate
  {
    uint32_t propertyFlags = BLECharacteristic::PROPERTY_INDICATE;
    BLECharacteristic *pCharIndicate =
        pService->createCharacteristic(CHAR_INDICATE_UUID, propertyFlags);
    pCharIndicate->setCallbacks(new MyCharIndicateStatusCallbacks);
    pCharIndicate->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED |
                                        ESP_GATT_PERM_WRITE_ENCRYPTED);
    pCharIndicate->addDescriptor(new BLE2902());
    pCharIndicate->setValue("");
    g_pCharIndicate = pCharIndicate;
  }

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  // this fixes iPhone connection issue (don't know how it works)
  {
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
  }
  BLEDevice::startAdvertising();

  bleSecurity();

  Serial.println("BLE Peripheral setup done, advertising");

  setup_welcome_screen();
}

void loop() {

  // handle commands received over BLE
  if (g_centralConnected) {
    if (g_needToHandleConnectionState) {
      g_needToHandleConnectionState = false;

      tft.fillScreen(TFT_BLACK);
      tft.drawCentreString("Connected", 160, 120, 2);
    }

    if (g_isWriteDataUpdated) {

      String currentData = g_writeData;
      g_isWriteDataUpdated = false;

      if (currentData.length() > 0) {

        PrintEvent("onWrite", currentData.c_str());
        setup_mainscreen();

      } else {
        Serial.println("ERROR: write data size == 0");
      }
    } else {
      uint32_t time = millis();
      if (time - g_lastActivityTime > 4000) {
        g_lastActivityTime = time;
        g_pCharIndicate->indicate();
      }
    }
  } else {
    if (g_needToHandleConnectionState && millis() > 3000) {
      g_needToHandleConnectionState = false;

      tft.fillScreen(TFT_BLACK);
      tft.drawCentreString("Disconnected", 160, 120, 2);
    }
  }
  delay(10);
}