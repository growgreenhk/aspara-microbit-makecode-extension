
#include "MicroBitConfig.h"

#include "aspara-smart-grower-service.h"
#include "ble_srv_common.h"

#include "pxt.h"
#include "MicroBit.h"

#define VERSION     10
#define VER_MAJOR   1
#define VER_MINOR   25

// Advertising includes
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "ble_dis.h"

#include "debug.h"

using namespace pxt;

//////////////// Initialize static members
const uint8_t asparaSmartGrowerService::base_uuid[16] = {0x4e, 0x70, 0x09, 0x30, 0xcf, 0x94, 0x43, 0x60, 0xab, 0x39, 0xc1, 0x0e, 0xdd, 0x41, 0xc9, 0xf1};
const uint16_t asparaSmartGrowerService::serviceId = 0x0930; 
const uint16_t asparaSmartGrowerService::charUUID[asparaCharCount] = {  
  0x0939,  //  Device status
  0x093d   //  Device Command
};

// const uint8_t asparaSmartGrowerService::smartGrowerCmdHeader[2] = {0xBB, 0xB0};

asparaSmartGrowerService *asparaSmartGrowerService::service = NULL; // Singleton reference to the service

// Static method for peer_manager events (Bounce it to the instance, which has access to member vars)
void asparaSmartGrowerService::static_pm_events(const pm_evt_t* p_event) {
  getInstance()->pm_events(p_event);
}

void asparaSmartGrowerService::pm_events(const pm_evt_t* p_event) {
  // if(p_event->evt_id == PM_EVT_PEER_DATA_UPDATE_SUCCEEDED) {
  if(p_event->evt_id == PM_EVT_CONN_SEC_SUCCEEDED) {
    asparaSmartGrowerService *ins = asparaSmartGrowerService::getInstance();
    if (ins) {
      ins->ubitBLEConnected = true;
      ins->connectedTimeMark = system_timer_current_time();
    }
    for(int i=asparaCharControlStatus, idx=0; i<asparaCharCount;i++, idx++) {

      // Get the CCCD
      ble_gatts_value_t data;
      memset(&data, 0, sizeof(ble_gatts_value_t));
      uint16_t value;
      data.len = 2;
      data.p_value = (uint8_t*)&value;
      sd_ble_gatts_value_get(p_event->conn_handle, charHandles(i)->cccd, &data); 
      // Update the internal characteristic flags
      chars[i].setCCCD(value);
    }
  }
}

/**
 */
asparaSmartGrowerService *asparaSmartGrowerService::getInstance()
{
    if (service == NULL)
    {
        service = new asparaSmartGrowerService();
    }
    return service;
}

void onConnected(MicroBitEvent)
{
  // asparaSmartGrowerService *ins = asparaSmartGrowerService::getInstance();
  // if (ins) {
  //   ins->ubitBLEConnected = true;
  //   ins->connectedTimeMark = system_timer_current_time();
  // }
}

void onDisconnected(MicroBitEvent)
{
  asparaSmartGrowerService *ins = asparaSmartGrowerService::getInstance();
  if (ins) {
    ins->ubitBLEConnected = false;
    ins->connectedTimeMark = system_timer_current_time();
  }
}

bool asparaSmartGrowerService::IsBleConnected() {
  if (serviceBLEConnected && ubitBLEConnected) {
    if ((system_timer_current_time() - connectedTimeMark) > 2000) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

/** 
 * Constructor.
 * Create a representation of the Bluetooth SIG HID Service
 */
asparaSmartGrowerService::asparaSmartGrowerService()
{
#if DEBUG_ENABLED == 1
  DEBUG("\r\nasparaSmartGrowerService starting\r\n");
  DEBUG("\r\nVer: %d.%d.%d\r\n", VERSION, VER_MAJOR, VER_MINOR);
#endif
  ble_gap_sec_params_t sec_param;
  ret_code_t err_code;
  semBLE = 0;
  deviceTimeMark = 0;
  connectedTimeMark = system_timer_current_time();
  serviceBLEConnected = false;
  ubitBLEConnected = false;
  rtcCmd = NULL;
  advertising = true;
  uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
  uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
  // Update advertisements 
  smartGrowerStartAdvertise();

  RegisterBaseUUID(base_uuid);

  CreateService( serviceId );

  memset(status, 0, sizeof(status));
  memset(command, 0, sizeof(command));

  ble_gap_addr_t bt_addr;

  sd_ble_gap_addr_get(&bt_addr);
  sprintf(assignName, "aspLiteCntr-%02X%02X%02X", bt_addr.addr[2], bt_addr.addr[1], bt_addr.addr[0]);

  // Create the data structures that represent each of our characteristics in Soft Device.
  // iOS needs protocol mode characteristic for keyboard support
  CreateCharacteristic( asparaCharControlStatus, charUUID[ asparaCharControlStatus ],
                      (uint8_t *)status,
                      sizeof(status), sizeof(status),
                      microbit_propREAD  | microbit_propNOTIFY | microbit_propWRITE_WITHOUT ); 

  CreateCharacteristic( asparaCharControlCmd, charUUID[ asparaCharControlCmd ],
                      (uint8_t *)command,
                      sizeof(command), sizeof(command),
                      microbit_propREAD  | microbit_propNOTIFY | microbit_propWRITE_WITHOUT ); 

  // pm_register(static_pm_events); 
  pm_init();

  memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

  // Security parameters to be used for all security procedures.
  sec_param.bond           = 1;
  sec_param.mitm           = 0;
  sec_param.lesc           = 0;
  sec_param.keypress       = 0;
  sec_param.io_caps        = 0x03;
  sec_param.oob            = 0;
  sec_param.min_key_size   = 7;
  sec_param.max_key_size   = 16;
  sec_param.kdist_own.enc = 1;
  sec_param.kdist_own.id  = 1;
  sec_param.kdist_peer.enc = 1;
  sec_param.kdist_peer.id  = 1;

  err_code = pm_sec_params_set(&sec_param);

  err_code = pm_register(static_pm_events);
}

/**
  * Invoked when BLE connects.
  */
void asparaSmartGrowerService::onConnect( const microbit_ble_evt_t *p_ble_evt)
{
#if DEBUG_ENABLED == 1
  DEBUG("\r\nonConnec\r\n");
#endif
  serviceBLEConnected = true;
  smartGrowerStopAdvertise();
}

/**
  * Invoked when BLE disconnects.
  */
void asparaSmartGrowerService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{
#if DEBUG_ENABLED == 1
  DEBUG("\r\nonDisconnec\r\n");
#endif
  serviceBLEConnected = false;
  smartGrowerStartAdvertise();
}

void asparaSmartGrowerService::onDataRead( microbit_onDataRead_t *params) {
#if DEBUG_ENABLED == 1
  DEBUG("\r\nonDataRead\r\n");
#endif
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void asparaSmartGrowerService::onDataWritten( const microbit_ble_evt_write_t *params)
{
#if DEBUG_ENABLED == 1
  DEBUG("\r\nonDataWritten uuid: %X     len:%d", params->uuid.uuid, params->len);
  for (int i=0; i < params->len; i++) {
    if (params->data[i] < 0x10) {
      DEBUG("  0%X", params->data[i]);
    } else {
      DEBUG("  %X", params->data[i]);
    }
  }
  DEBUG("\r\n");
#endif
  if (params->uuid.uuid == asparaSmartGrowerService::charUUID[asparaCharControlStatus]) {
    if (params->data[0] == 0xE3) {
      memcpy(intensityCmd, params->data, params->len);
    } else if (params->data[0] == 0xE5) {
      memcpy(pumpCmd, params->data, params->len);
    } else if (params->data[0] == 0xE6) {
      if (rtcCmd) {
        memcpy(rtcCmd, params->data, params->len);
        deviceTime = ((uint32_t)rtcCmd[1] + ((uint32_t)rtcCmd[2] << 8) + ((uint32_t)rtcCmd[3] << 16) + ((uint32_t)rtcCmd[4] << 24));
      } else {
        deviceTime = ((uint32_t)params->data[1] + ((uint32_t)params->data[2] << 8) + ((uint32_t)params->data[3] << 16) + ((uint32_t)params->data[4] << 24));
      }
      deviceTimeMark = system_timer_current_time();
    } else if (params->data[0] == 0xE8) {
      memcpy(indicatorCmd, params->data, params->len);
    } else if (params->data[0] == 0xEA) {
      memcpy(tempCmd, params->data, params->len);
    } else if (params->data[0] == 0xEB) {
      memcpy(humiCmd, params->data, params->len);
    } else if (params->data[0] == 0xEC) {
      memcpy(lightSensorCmd, params->data, params->len);
    } else if (params->data[0] == 0xED) {
      memcpy(nutrientCmd, params->data, params->len);
    } else if (params->data[0] == 0xEE) {
      memcpy(batteryCmd, params->data, params->len);
    } else if (params->data[0] == 0xEF) {
      memcpy(waterlevelCmd, params->data, params->len);
    }
  }
}

bool asparaSmartGrowerService::onBleEvent(const microbit_ble_evt_t *p_ble_evt) {
  return MicroBitBLEService::onBleEvent(p_ble_evt);
}

bool asparaSmartGrowerService::notifyChrValue( int idx, const uint8_t *data, uint16_t length) {
  bool ret = false;

  if (semBLE == 0) {
    semBLE = 1;
    ret = MicroBitBLEService::notifyChrValue( idx, data, length);
    semBLE = 0;
  }
  return ret;
}

void asparaSmartGrowerService::setName() {
  int len = strlen(assignName);
  memcpy(gapName, assignName, strlen(assignName)+1);
  ble_gap_conn_sec_mode_t permissions;
  BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS( &permissions);
  MICROBIT_BLE_ECHK( sd_ble_gap_device_name_set( &permissions, (const unsigned char*)gapName, len) );
}

void asparaSmartGrowerService::smartGrowerStartAdvertise() {
  // Stop any active advertising
  smartGrowerStopAdvertise();
  setName();
  // m_advdata _must_ be static / retained!
  static ble_advdata_t m_advdata;
  // m_enc_advdata _must_ be static / retained!
  static uint8_t  m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
  // static ble_uuid_t uuid;  // UUID Struct
  uint8_t m_adv_handle;

  // uuid.type = BLE_UUID_TYPE_BLE;
  // uuid.uuid = 0x1812; // 1812 is HID 
  m_advdata.uuids_complete.uuid_cnt = 0;
  m_advdata.uuids_complete.p_uuids = NULL;
  m_advdata.include_appearance = false;
  // Name needed to be identified by Android
  m_advdata.name_type = BLE_ADVDATA_FULL_NAME;
  
  // Appearance isn't strictly needed for detection 
  sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_HID );

  // The flags below ensure "pairing mode" so it shows up in Android
  m_advdata.flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED | BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE;

  ble_gap_adv_params_t    gap_adv_params;
  memset( &gap_adv_params, 0, sizeof( gap_adv_params));
  gap_adv_params.properties.type  = true /* connectable */
                                  ? BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED
                                  : BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED;
  gap_adv_params.interval         = ( 1000 * MICROBIT_BLE_ADVERTISING_INTERVAL/* interval_ms */) / 625;  // 625 us units
  if ( gap_adv_params.interval < BLE_GAP_ADV_INTERVAL_MIN) gap_adv_params.interval = BLE_GAP_ADV_INTERVAL_MIN;
  if ( gap_adv_params.interval > BLE_GAP_ADV_INTERVAL_MAX) gap_adv_params.interval = BLE_GAP_ADV_INTERVAL_MAX;
  gap_adv_params.duration         = MICROBIT_BLE_ADVERTISING_TIMEOUT /* timeout_seconds */ * 100;              //10 ms units
  gap_adv_params.filter_policy    = false /* whitelist */
                                  ? BLE_GAP_ADV_FP_FILTER_BOTH
                                  : BLE_GAP_ADV_FP_ANY;
  gap_adv_params.primary_phy      = BLE_GAP_PHY_1MBPS;
              
  ble_gap_adv_data_t  gap_adv_data;
  memset( &gap_adv_data, 0, sizeof( gap_adv_data));
  gap_adv_data.adv_data.p_data    = m_enc_advdata;
  gap_adv_data.adv_data.len       = BLE_GAP_ADV_SET_DATA_SIZE_MAX;

  MICROBIT_BLE_ECHK( ble_advdata_encode( &m_advdata, gap_adv_data.adv_data.p_data, &gap_adv_data.adv_data.len));
  MICROBIT_BLE_ECHK( sd_ble_gap_adv_set_configure( &m_adv_handle, &gap_adv_data, &gap_adv_params));

  // Restart advertising
  // TODO / FIXME / REVIEW / WARNING: This will start adv using the static handle in the BLE Manager. 
  // Hopefully the same handle is used as the one returned by sd_ble_gap_adv_set_configure
  uBit.bleManager.advertise();
  advertising = true;
#if DEBUG_ENABLED == 1
  DEBUG("\r\nstart advertising !\r\n");
#endif
} 

void asparaSmartGrowerService::smartGrowerStopAdvertise() {
  if (advertising) {
    uBit.bleManager.stopAdvertising();
    advertising = false;
#if DEBUG_ENABLED == 1
    DEBUG("\r\nstop advertising\r\n");
#endif
  }
}

void asparaSmartGrowerService::setBroadcastName(const char *name) {
  int len = strlen(name) > sizeof(assignName) - 1 ? sizeof(assignName) - 1 : strlen(name);

  memset(assignName, 0, sizeof(assignName));
  memcpy(assignName, name, len);
  if (advertising) {
    smartGrowerStartAdvertise();
  }
}

void asparaSmartGrowerService::smartGrowerSendCmd(uint8_t *cmd, uint8_t len) {
  uint8_t i = 1;

  // memcpy(&buffer[i], smartGrowerCmdHeader, sizeof(smartGrowerCmdHeader));
  // i += sizeof(smartGrowerCmdHeader);
  memcpy(&buffer[0][i], cmd, len);
  buffer[0][0] = i + len;
#if DEBUG_ENABLED == 1
  for (int i=0; i < buffer[0]; i++) {
    if (buffer[i] < 0x10) {
      DEBUG("  0%X", buffer[i]);
    } else {
      DEBUG("  %X", buffer[i]);
    }
  }
  DEBUG("\r\n");
#endif
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[0][0], buffer[0][0])) {
      break;
    } else {
      uBit.sleep(5);
    }
  }
}

void asparaSmartGrowerService::getLedIntensity(uint8_t *cmd) {
  intensityCmd = cmd;
  buffer[1][0] = 3;
  buffer[1][1] = cmd[0];
  buffer[1][2] = cmd[1];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[1][0], buffer[1][0])) {
      break;
    } else {
      uBit.sleep(5+2);
    }
  }
}

void asparaSmartGrowerService::getTemperature(uint8_t *cmd) {
  tempCmd = cmd;
  buffer[2][0] = 2;
  buffer[2][1] = cmd[0];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[2][0], buffer[2][0])) {
      break;
    } else {
      uBit.sleep(5+6);
    }
  }
}

void asparaSmartGrowerService::getHumidity(uint8_t *cmd) {
  humiCmd = cmd;
  buffer[3][0] = 2;
  buffer[3][1] = cmd[0];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[3][0], buffer[3][0])) {
      break;
    } else {
      uBit.sleep(5+8);
    }
  }
}

void asparaSmartGrowerService::getLightSensor(uint8_t *cmd) {
  lightSensorCmd = cmd;
  buffer[4][0] = 2;
  buffer[4][1] = cmd[0];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[4][0], buffer[4][0])) {
      break;
    } else {
      uBit.sleep(5+6);
    }
  }
}

void asparaSmartGrowerService::getNutrient(uint8_t *cmd) {
  nutrientCmd = cmd;
  buffer[5][0] = 2;
  buffer[5][1] = cmd[0];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[5][0], buffer[5][0])) {
      break;
    } else {
      uBit.sleep(5+8);
    }
  }
}

void asparaSmartGrowerService::getBattery(uint8_t *cmd) {
  batteryCmd = cmd;
  buffer[6][0] = 2;
  buffer[6][1] = cmd[0];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[6][0], buffer[6][0])) {
      break;
    } else {
      uBit.sleep(5+6);
    }
  }
}

void asparaSmartGrowerService::getWaterLevel(uint8_t *cmd) {
  waterlevelCmd = cmd;
  buffer[7][0] = 2;
  buffer[7][1] = cmd[0];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[7][0], buffer[7][0])) {
      break;
    } else {
      uBit.sleep(5+8);
    }
  }
}

void asparaSmartGrowerService::getIndicatorState(uint8_t *cmd) {
  indicatorCmd = cmd;
  buffer[8][0] = 3;
  buffer[8][1] = cmd[0];
  buffer[8][2] = cmd[1];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[8][0], buffer[8][0])) {
      break;
    } else {
      uBit.sleep(5+2);
    }
  }
}

void asparaSmartGrowerService::getPumpState(uint8_t *cmd) {
  pumpCmd = cmd;
  buffer[9][0] = 2;
  buffer[9][1] = cmd[0];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[9][0], buffer[9][0])) {
      break;
    } else {
      uBit.sleep(5+6);
    }
  }
}

void asparaSmartGrowerService::getRtc(uint8_t *cmd) {
  rtcCmd = cmd;
  buffer[10][0] = 2;
  buffer[10][1] = cmd[0];
  for(int k=0; k < 500; k++) {
    if (notifyChrValue( asparaCharControlCmd, &buffer[10][0], buffer[10][0])) {
      break;
    } else {
      uBit.sleep(5+2);
    }
  }
}