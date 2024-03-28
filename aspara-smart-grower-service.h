#ifndef ASPARA_SMART_GROWER_SERVICE_H
#define ASPARA_SMART_GROWER_SERVICE_H


#include "MicroBitConfig.h"

#include "MicroBitBLEManager.h"
#include "MicroBitBLEService.h"
#include "EventModel.h"
#include "debug.h"
#include "time.h"

#include"peer_manager.h"

/**
  * Class definition for a MicroBit BLE Service.
  */
class asparaSmartGrowerService : public MicroBitBLEService
{
public:
  bool serviceBLEConnected;
  bool ubitBLEConnected;
  time_t deviceTime = 1698796800; // 2023 11 1 00:00:00
  uint32_t deviceTimeMark;
  uint32_t connectedTimeMark;
  static asparaSmartGrowerService *getInstance();
  bool IsBleConnected();
  void setBroadcastName(const char *name);
  void smartGrowerSendCmd(uint8_t *cmd, uint8_t len);
  void getTemperature(uint8_t *cmd);
  void getHumidity(uint8_t *cmd);
  void getLightSensor(uint8_t *cmd);
  void getNutrient(uint8_t *cmd);
  void getBattery(uint8_t *cmd);
  void getWaterLevel(uint8_t *cmd);
  void getIndicatorState(uint8_t *cmd);
  void getPumpState(uint8_t *cmd);
  void getRtc(uint8_t *cmd);
  void getLedIntensity(uint8_t *cmd);

private:
  static asparaSmartGrowerService *service; // Singleton
  uint8_t semBLE;

  /**
   * Constructor.
   * Create a representation of the Bluetooth SIG HID Service
   * @param _ble The instance of a BLE device that we're running on.
   */
  asparaSmartGrowerService();

  /**
    * Invoked when BLE connects.
    */
  void onConnect(const microbit_ble_evt_t *p_ble_evt);

  /**
    * Invoked when BLE disconnects.
    */
  void onDisconnect(const microbit_ble_evt_t *p_ble_evt);

  /**
    * Callback. Invoked when any of our attributes are written via BLE.
    */
  void onDataWritten(const microbit_ble_evt_write_t *params);

  /**
   * Callback. Invoked when any of our attributes are read via BLE.
   */
  void onDataRead(microbit_onDataRead_t *params);

  // BLE Events...Let's monitor 'em all. 
  bool onBleEvent(const microbit_ble_evt_t *p_ble_evt);

  // Override notification process to enforce minimum time between events. 
  bool notifyChrValue( int idx, const uint8_t *data, uint16_t length);

  // Peer Manager Events (re-enable CCCDs)
  void pm_events( const pm_evt_t* p_event);

  // Peer Manager Events (re-enable CCCDs)
  static void static_pm_events( const pm_evt_t* p_event);

  // Index for each characteristic in arrays of handles and UUIDs
  typedef enum asparaCharIdx
  {
      asparaCharControlStatus,
      asparaCharControlCmd,
      asparaCharCount
  } asparaCharIdx;

  static const uint8_t base_uuid[16];
  // static const uint8_t smartGrowerCmdHeader[2];

  // Service UUID
  static const uint16_t serviceId;

  char gapName[19];
  char assignName[19];
  bool advertising;
  uint8_t status[54];
  uint8_t command[32];
  uint8_t buffer[11][5];
  uint8_t *tempCmd;
  uint8_t *humiCmd;
  uint8_t *lightSensorCmd;
  uint8_t *nutrientCmd;
  uint8_t *batteryCmd;
  uint8_t *waterlevelCmd;
  uint8_t *indicatorCmd;
  uint8_t *pumpCmd;
  uint8_t *rtcCmd;
  uint8_t *intensityCmd;

  // UUIDs for our service and characteristics
  static const uint16_t charUUID[asparaCharCount];
  
  // Data for each characteristic when they are held by Soft Device.
  MicroBitBLEChar      chars[asparaCharCount];

  int              characteristicCount()          { return asparaCharCount; };
  MicroBitBLEChar *characteristicPtr(int idx)     { return &chars[ idx]; };

  void smartGrowerStartAdvertise();
  void smartGrowerStopAdvertise();

  void setName();
};
#endif // #ifdef ASPARA_SMART_GROWER_SERVICE_H