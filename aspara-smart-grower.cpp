#include "pxt.h"
#include "MicroBit.h"

#include "time.h"
#include "debug.h"
#include "aspara-smart-grower-service.h"

using namespace pxt; 

static asparaSmartGrowerService *smartGrowerService = NULL;

static uint16_t deviceTemperature = 0;
static uint8_t deviceHumidity = 0;
static uint16_t deviceLightSensor = 0;
static uint16_t deviceNutrient = 0;
static uint16_t deviceBattery = 0;
static uint8_t deviceWaterLevel = 0;
static Buffer datetime = mkBuffer(NULL, 7);
static uint8_t deviceIntensity[3] = {0, 0, 0};
static uint8_t deviceIndicatorState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t devicePumpState = 0;

static uint8_t smartGrowerCmdBuffer[14][4];
static uint8_t tempCmdBuffer[3];
static uint8_t humiCmdBuffer[2];
static uint8_t lightSensorCmdBuffer[3];
static uint8_t nutrientCmdBuffer[3];
static uint8_t batteryCmdBuffer[3];
static uint8_t waterLevelCmdBuffer[2];
static uint8_t rtcCmdBuffer[5];
static uint8_t indicatorCmdBuffer[3];
static uint8_t pumpCmdBuffer[2];
static uint8_t intensityCmdBuffer[4];

const int timeLimitCount = 15;  // uBit.wait(100), would be 1.5s

namespace asparaSmartGrower { 
  //% 
  void startAsparaSmartGrowerService(int id) { 
    if (smartGrowerService == NULL) {
      smartGrowerService = asparaSmartGrowerService::getInstance();
      if (smartGrowerService) {
        char IdName[] = "GGmB-000000";

        sprintf(&IdName[6], "%05d", id);
        smartGrowerService->setBroadcastName(IdName);
      }
    }
  }

  //% 
  void setLEDlights(int white, int red, int blue) {
    static bool setLEDlights_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        smartGrowerCmdBuffer[0][0] = 0xB0;
        smartGrowerCmdBuffer[0][1] = white;
        smartGrowerCmdBuffer[0][2] = red;
        smartGrowerCmdBuffer[0][3] = blue;
        if (!setLEDlights_sem) {
          setLEDlights_sem = true;
          smartGrowerService->smartGrowerSendCmd(&smartGrowerCmdBuffer[0][0], 4);
          setLEDlights_sem = false;
        }
      }
    }
  }

  //% 
  void setLEDlight(int ledtype, int intensity) {
    static bool setLEDlight_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        smartGrowerCmdBuffer[3+ledtype][0] = 0xB1;
        smartGrowerCmdBuffer[3+ledtype][1] = ledtype;
        smartGrowerCmdBuffer[3+ledtype][2] = intensity;
        if (!setLEDlight_sem) {
          setLEDlight_sem = true;
          smartGrowerService->smartGrowerSendCmd(&smartGrowerCmdBuffer[3+ledtype][0], 3);
          setLEDlight_sem = false;
        }
      }
    }
  }

  //% 
  void setIndicator(int indicatortype, int onoff) {
    static bool setIndicator_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        smartGrowerCmdBuffer[6+indicatortype][0] = 0xB7;
        smartGrowerCmdBuffer[6+indicatortype][1] = indicatortype;
        smartGrowerCmdBuffer[6+indicatortype][2] = onoff;
        if (!setIndicator_sem) {
          setIndicator_sem = true;
          smartGrowerService->smartGrowerSendCmd(&smartGrowerCmdBuffer[6+indicatortype][0], 3);
          setIndicator_sem = false;
        }
      }
    }
  }

  //% 
  void setPump(int onoff) { 
    static bool setPump_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        smartGrowerCmdBuffer[1][0] = 0xB2;
        smartGrowerCmdBuffer[1][1] = onoff;
        if (!setPump_sem) {
          setPump_sem = true;
          smartGrowerService->smartGrowerSendCmd(&smartGrowerCmdBuffer[1][0], 2);
          setPump_sem = false;
        }
      }
    }
  }

  //% 
  uint8_t ledIntensity(int type) { 
    static bool ledIntensity_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!ledIntensity_sem) {
          ledIntensity_sem = true;
          if (intensityCmdBuffer[0] != 0xB3) {
            intensityCmdBuffer[0] = 0xB3;
            intensityCmdBuffer[1] = (uint8_t)type;
            smartGrowerService->getLedIntensity(intensityCmdBuffer);
          }
          // while(intensityCmdBuffer[0] != 0xE3) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (intensityCmdBuffer[0] == 0xE3) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              intensityCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          deviceIntensity[type] = intensityCmdBuffer[2];
          ledIntensity_sem = false;
        }
      }
    }
    return deviceIntensity[type];
  }

  //% 
  void beep(int longbeep) { 
    static bool beep_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        smartGrowerCmdBuffer[2][0] = 0xB4;
        smartGrowerCmdBuffer[2][1] = (uint8_t)longbeep;
        if (!beep_sem) {
          beep_sem = true;
          smartGrowerService->smartGrowerSendCmd(&smartGrowerCmdBuffer[2][0], 2);
          beep_sem = false;
        }
      }
    }
  }

  //% 
  uint8_t indicatorState(int type) {
    static bool indicatorState_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!indicatorState_sem) {
          indicatorState_sem = true;
          if (indicatorCmdBuffer[0] != 0xB8) {
            indicatorCmdBuffer[0] = 0xB8;
            indicatorCmdBuffer[1] = (uint8_t)type;
            smartGrowerService->getIndicatorState(indicatorCmdBuffer);
          }
          // while(indicatorCmdBuffer[0] != 0xE8) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (indicatorCmdBuffer[0] == 0xE8) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              indicatorCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          deviceIndicatorState[type] = (uint8_t)indicatorCmdBuffer[2];
          indicatorState_sem = false;
        }
      }
    }
    return deviceIndicatorState[type];
  }

  //% 
  uint8_t pumpState() {
    static bool pumpState_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!pumpState_sem) {
          pumpState_sem = true;
          if (pumpCmdBuffer[0] != 0xB5) {
            pumpCmdBuffer[0] = 0xB5;
            smartGrowerService->getPumpState(pumpCmdBuffer);
          }
          // while(pumpCmdBuffer[0] != 0xE5) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (pumpCmdBuffer[0] == 0xE5) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              pumpCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          devicePumpState = (uint16_t)pumpCmdBuffer[1];
          pumpState_sem = false;
        }
      }
    }
    return devicePumpState;
  }

  //% 
  float temperature() {
    static bool temperature_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!temperature_sem) {
          temperature_sem = true;
          if (tempCmdBuffer[0] != 0xBA) {
            tempCmdBuffer[0] = 0xBA;
            smartGrowerService->getTemperature(tempCmdBuffer);
          }
          // while(tempCmdBuffer[0] != 0xEA) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (tempCmdBuffer[0] == 0xEA) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              tempCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          deviceTemperature = (uint16_t)tempCmdBuffer[1] + ((uint16_t)tempCmdBuffer[2] << 8);
          temperature_sem = false;
        }
      } else {
        deviceTemperature = 0;
      }
    }
    return ((float)deviceTemperature / 100.0);
  }

  //% 
  uint8_t humidity() {
    static bool humidity_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!humidity_sem) {
          humidity_sem = true;
          if (humiCmdBuffer[0] != 0xBB) {
            humiCmdBuffer[0] = 0xBB;
            smartGrowerService->getHumidity(humiCmdBuffer);
          }
          // while(humiCmdBuffer[0] != 0xEB) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (humiCmdBuffer[0] == 0xEB) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              humiCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          deviceHumidity = humiCmdBuffer[1];
          humidity_sem = false;
        }
      } else {
        deviceHumidity = 0;
      }
    }
    return deviceHumidity;
  }

  //% 
  uint16_t lightsensor() {
    static bool lightsensor_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!lightsensor_sem) {
          lightsensor_sem = true;
          if (lightSensorCmdBuffer[0] != 0xBC) {
            lightSensorCmdBuffer[0] = 0xBC;
            smartGrowerService->getLightSensor(lightSensorCmdBuffer);
          }
          // while(lightSensorCmdBuffer[0] != 0xEC) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (lightSensorCmdBuffer[0] == 0xEC) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              lightSensorCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          deviceLightSensor = (uint16_t)lightSensorCmdBuffer[1] + ((uint16_t)lightSensorCmdBuffer[2] << 8);
          lightsensor_sem = false;
        }
      } else {
        deviceLightSensor = 0;
      }
    }
    return deviceLightSensor;
  }

  //% 
  uint16_t nutrient() {
    static bool nutrient_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!nutrient_sem) {
          nutrient_sem = true;
          if (nutrientCmdBuffer[0] != 0xBD) {
            nutrientCmdBuffer[0] = 0xBD;
            smartGrowerService->getNutrient(nutrientCmdBuffer);
          }
          // while(nutrientCmdBuffer[0] != 0xED) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (nutrientCmdBuffer[0] == 0xED) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              nutrientCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          deviceNutrient = (uint16_t)nutrientCmdBuffer[1] + ((uint16_t)nutrientCmdBuffer[2] << 8);
          nutrient_sem = false;
        }
      } else {
        deviceNutrient = 0;
      }
    }
    return deviceNutrient;
  }

  //% 
  float battery() {
    static bool battery_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!battery_sem) {
          battery_sem = true;
          if (batteryCmdBuffer[0] != 0xBE) {
            batteryCmdBuffer[0] = 0xBE;
            smartGrowerService->getBattery(batteryCmdBuffer);
          }
          // while(batteryCmdBuffer[0] != 0xEE) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (batteryCmdBuffer[0] == 0xEE) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              batteryCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          deviceBattery = (uint16_t)batteryCmdBuffer[1] + ((uint16_t)batteryCmdBuffer[2] << 8);
          battery_sem = false;
        }
      } else {
        deviceBattery = 0;
      }
    }
    return ((float)deviceBattery / 100.0);
  }

  //% 
  uint8_t waterlevel() {
    static bool waterlevel_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->IsBleConnected()) {
        if (!waterlevel_sem) {
          waterlevel_sem = true;
          if (waterLevelCmdBuffer[0] != 0xBF) {
            waterLevelCmdBuffer[0] = 0xBF;
            smartGrowerService->getWaterLevel(waterLevelCmdBuffer);
          }
          // while(waterLevelCmdBuffer[0] != 0xEF) {
          for(int i = 0; i < timeLimitCount; i++) {
            if (smartGrowerService->IsBleConnected()) {
              if (waterLevelCmdBuffer[0] == 0xEF) {
                i = timeLimitCount;
              } else {
                uBit.sleep(100);
              }
            } else {
              waterLevelCmdBuffer[0] = 0;
              i = timeLimitCount;
            }
          }
          deviceWaterLevel = waterLevelCmdBuffer[1];
          waterlevel_sem = false;
        }
      }
    }
    return deviceWaterLevel;
  }

  //%
  Buffer getDatetime() {
    struct tm *tm;
    const time_t consttimet = 1698796800; // 2023 11 1 00:00:00
    time_t realtime;
    static bool getDatetime_sem = false;

    if (smartGrowerService != NULL) {
      if (smartGrowerService->deviceTimeMark == 0) {
        if (smartGrowerService->IsBleConnected()) {
          if (!getDatetime_sem) {
            getDatetime_sem = true;
            if (rtcCmdBuffer[0] != 0xB6) {
              rtcCmdBuffer[0] = 0xB6;
              smartGrowerService->getRtc(rtcCmdBuffer);
            }
            // while(rtcCmdBuffer[0] != 0xE6) {
            for(int i = 0; i < timeLimitCount; i++) {
              if (smartGrowerService->IsBleConnected()) {
                if (rtcCmdBuffer[0] == 0xE6) {
                  i = timeLimitCount;
                } else {
                  uBit.sleep(100);
                }
              } else {
                rtcCmdBuffer[0] = 0;
                i = timeLimitCount;
              }
            }
            getDatetime_sem = false;
          }
        }
      } else {
        realtime = smartGrowerService->deviceTime + ((system_timer_current_time() - smartGrowerService->deviceTimeMark) / 1000);
      }
    } else {
      realtime = consttimet + (system_timer_current_time() / 1000);
    }
    tm = localtime(&realtime);

    // struct tm {
    // 	int	tm_sec;		/* seconds after the minute [0-60] */
    // 	int	tm_min;		/* minutes after the hour [0-59] */
    // 	int	tm_hour;	/* hours since midnight [0-23] */
    // 	int	tm_mday;	/* day of the month [1-31] */
    // 	int	tm_mon;		/* months since January [0-11] */
    // 	int	tm_year;	/* years since 1900 */
    // 	int	tm_wday;	/* days since Sunday [0-6] */
    // 	int	tm_yday;	/* days since January 1 [0-365] */
    // 	int	tm_isdst;	/* Daylight Savings Time flag */
    // 	long	tm_gmtoff;	/* offset from UTC in seconds */
    // 	char	*tm_zone;	/* timezone abbreviation */
    // };
    datetime->data[0] = (tm->tm_year + 1900) % 100;
    datetime->data[1] = (tm->tm_year + 1900) / 100;
    datetime->data[2] = tm->tm_mon + 1;
    datetime->data[3] = tm->tm_mday;
    datetime->data[4] = tm->tm_hour;
    datetime->data[5] = tm->tm_min;
    datetime->data[6] = tm->tm_sec;
    // timett = timett + 2;

    return datetime;
  }
}