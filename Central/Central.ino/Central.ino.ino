/**
 * Bluetooth Classic Example
 * Scan for devices - asyncronously, print device as soon as found
 * query devices for SPP - SDP profile
 * connect to first device offering a SPP connection
 * 
 * Example python server:
 * source: https://gist.github.com/ukBaz/217875c83c2535d22a16ba38fc8f2a91
 *
 * Tested with Raspberry Pi onboard Wifi/BT, USB BT 4.0 dongles, USB BT 1.1 dongles, 
 * 202202: does NOT work with USB BT 2.0 dongles when esp32 aduino lib is compiled with SSP support!
 *         see https://github.com/espressif/esp-idf/issues/8394
 *         
 * use ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE in connect() if remote side requests 'RequireAuthentication': dbus.Boolean(True),
 * use ESP_SPP_SEC_NONE or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE in connect() if remote side has Authentication: False
 */

#include <stdarg.h>
#include <map>
#include <BluetoothSerial.h>
#include <M5Unified.h> 

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;


#define BT_DISCOVER_TIME  10000
esp_spp_sec_t sec_mask=ESP_SPP_SEC_NONE; // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to request pincode confirmation
esp_spp_role_t role=ESP_SPP_ROLE_SLAVE; // or ESP_SPP_ROLE_MASTER

// std::map<BTAddress, BTAdvertisedDeviceSet> btDeviceList;

void printBoth(bool ln,const char* format, ...) {
  va_list args;
  va_start(args, format);

  // フォーマットされた文字列を格納するためのバッファを作成
  char buffer[256]; // 適切なサイズを設定してください
  vsnprintf(buffer, sizeof(buffer), format, args);

  if(ln) println(buffer);
  else print(buffer);

  va_end(args);
}

void println(const char* str) {
  Serial.println(str);         // シリアルモニターにHello World!!と1行表示
  M5.Display.println(str);       // 画面に1行表示
}

void print(const char* str){
  Serial.print(str);         // シリアルモニターにHello World!!と1行表示
  M5.Display.print(str);       // 画面に1行表示
}

void setup() {
  auto cfg = M5.config();       // M5Stack初期設定用の構造体を代入
  M5.begin(cfg);                // M5デバイスの初期化
  if(! SerialBT.begin("ESP32test", true) ) {
    Serial.println("========== serialBT failed!");
    abort();
  }
  // SerialBT.setPin("1234"); // doesn't seem to change anything
  // SerialBT.enableSSP(); // doesn't seem to change anything


  printBoth(true,"Starting discoverAsync...");
  BTScanResults* btDeviceList = SerialBT.getScanResults();  // maybe accessing from different threads!
  if (SerialBT.discoverAsync([](BTAdvertisedDevice* pDevice) {
      // BTAdvertisedDeviceSet*set = reinterpret_cast<BTAdvertisedDeviceSet*>(pDevice);
      // btDeviceList[pDevice->getAddress()] = * set;
      Serial.printf(">>>>>>>>>>>Found a new device asynchronously: %s\n", pDevice->toString().c_str());
    } )
    ) {
    delay(BT_DISCOVER_TIME);
    printBoth(false,"Stopping discoverAsync... ");
    SerialBT.discoverAsyncStop();
    printBoth(true,"discoverAsync stopped");
    delay(5000);
    if(btDeviceList->getCount() > 0) {
      BTAddress addr;
      int channel=0;
      printBoth(true,"Found devices:");
      for (int i=0; i < btDeviceList->getCount(); i++) {
        BTAdvertisedDevice *device=btDeviceList->getDevice(i);
        printBoth(false," - %s %s %d\n", device->getAddress().toString().c_str(), device->getName().c_str(), device->getRSSI());
        std::map<int,std::string> channels=SerialBT.getChannels(device->getAddress());
        printBoth(false,"scanned for services, found %d\n", channels.size());
        for(auto const &entry : channels) {
          Serial.printf(" channel %d (%s)\n", entry.first, entry.second.c_str());
        }
        if(channels.size() > 0) {
          addr = device->getAddress();
          channel=channels.begin()->first;
        }
      }
      if(addr) {
        printBoth(false,"connecting to %s - %d\n", addr.toString().c_str(), channel);
        SerialBT.connect(addr, channel, sec_mask, role);
      }
    } else {
      printBoth(true,"Didn't find any devices");
    }
  } else {
    printBoth(true,"Error on discoverAsync f.e. not workin after a \"connect\"");
  }
}


String sendData="+";

void loop() {
  if(! SerialBT.isClosed() && SerialBT.connected()) {
    if( SerialBT.write((const uint8_t*) sendData.c_str(),sendData.length()) != sendData.length()) {
      printBoth(true,"tx: error");
    } else {
      printBoth(false,"tx: %s",sendData.c_str());
    }
    if(SerialBT.available()) {
      printBoth(false,"rx: ");
      while(SerialBT.available()) {
        int c=SerialBT.read();
        if(c >= 0) {
          printBoth(false,"%c",(char) c);
        }
      }
      printBoth(true,"");
    }
  } else {
    printBoth(true,"not connected");
  }
  delay(1000);
}
