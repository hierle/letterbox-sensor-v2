//
// lorawan.ino
//


// receive callback
void recvCallback(SERVICE_LORA_RECEIVE_T * data) {
    if (data->BufferSize > 0) {
        Serial.println("Something received!");
        for (int i = 0; i < data->BufferSize; i++) {
            Serial.printf("%x", data->Buffer[i]);
        }
        Serial.print("\r\n");
    }
}

// join callback for OTAA
void joinCallback(int32_t status) {
    Serial.printf("Join status: %d\r\n", status);
}

/*************************************
 * enum type for LoRa Event
    RAK_LORAMAC_STATUS_OK = 0,
    RAK_LORAMAC_STATUS_ERROR,
    RAK_LORAMAC_STATUS_TX_TIMEOUT,
    RAK_LORAMAC_STATUS_RX1_TIMEOUT,
    RAK_LORAMAC_STATUS_RX2_TIMEOUT,
    RAK_LORAMAC_STATUS_RX1_ERROR,
    RAK_LORAMAC_STATUS_RX2_ERROR,
    RAK_LORAMAC_STATUS_JOIN_FAIL,
    RAK_LORAMAC_STATUS_DOWNLINK_REPEATED,
    RAK_LORAMAC_STATUS_TX_DR_PAYLOAD_SIZE_ERROR,
    RAK_LORAMAC_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS,
    RAK_LORAMAC_STATUS_ADDRESS_FAIL,
    RAK_LORAMAC_STATUS_MIC_FAIL,
    RAK_LORAMAC_STATUS_MULTICAST_FAIL,
    RAK_LORAMAC_STATUS_BEACON_LOCKED,
    RAK_LORAMAC_STATUS_BEACON_LOST,
    RAK_LORAMAC_STATUS_BEACON_NOT_FOUND,
 *************************************/

// send callback
void sendCallback(int32_t status) {
    if (status != RAK_LORAMAC_STATUS_OK) {
        Serial.println("Sending failed");
    }
}

// init lorawan
void lorawan_init() {
    // check for network work mode (lorawan)
    if(api.lorawan.nwm.get() != 1) {
        Serial.printf("Set Node device work mode %s\r\n", api.lorawan.nwm.set(1) ? "Success" : "Fail");
        api.system.reboot();
    }

    if (USE_OTAA) {
      // OTAA device EUI MSB first
      uint8_t node_device_eui[8] = OTAA_DEVEUI;
      // OTAA application EUI MSB first
      uint8_t node_app_eui[8] = OTAA_APPEUI;
      // OTAA application key MSB first
      uint8_t node_app_key[16] = OTAA_APPKEY;
      if (!api.lorawan.appeui.set(node_app_eui, 8)) {
          Serial.printf("ERROR: setting application EUI failed! \r\n");
          return;
      }
      if (!api.lorawan.appkey.set(node_app_key, 16)) {
          Serial.printf("ERROR: setting application key failed! \r\n");
          return;
      }
      if (!api.lorawan.deui.set(node_device_eui, 8)) {
          Serial.printf("ERROR: setting device EUI failed! \r\n");
          return;
      }
      // set network join mode to OTAA
      if (!api.lorawan.njm.set(RAK_LORA_OTAA)) {
          Serial.printf("ERROR: setting network join mode OTAA failed! \r\n");
          return;
      }
      // join the gateway
      if (!api.lorawan.join()) {
        Serial.printf("Lorawan OTAA - join fail! \r\n");
        return;
      }
      // wait for join success
      //   this will hang forever in case the join fails e.g. due to no network coverage
      while (api.lorawan.njs.get() == 0) {
          Serial.print("Wait for Lorawan join...");
          api.lorawan.join();
          delay(10000);
      }
    } // end OTAA
    else { // ABP
      // ABP device address MSB first
      uint8_t node_dev_addr[4] = ABP_DEVADDR;
      // ABP application session key
      uint8_t node_app_skey[16] = ABP_APPSKEY;
      // ABP network session key
      uint8_t node_nwk_skey[16] = ABP_NWKSKEY;
      // set the network join mode to ABP
      if (!api.lorawan.njm.set(RAK_LORA_ABP)) {
          Serial.printf("ERROR: setting network join mode ABP failed! \r\n");
          return;
      }
      // device address
      if (!api.lorawan.daddr.set(node_dev_addr, 4)) {
          Serial.printf("ERROR: setting device addr failed! \r\n");
          return;
      }
      // app session key
      if (!api.lorawan.appskey.set(node_app_skey, 16)) {
          Serial.printf("ERROR: setting application session key failed! \r\n");
          return;
      }
      // network session key
      if (!api.lorawan.nwkskey.set(node_nwk_skey, 16)) {
          Serial.printf("ERROR: setting network session key failed! \r\n");
          return;
      }
    } // end ABP

    // common for OTAA and ABP

    // Lorawan band
    if (!api.lorawan.band.set(LORAWAN_BAND)) {
        Serial.printf("ERROR: setting lora band failed! \r\n");
        return;
    }
    // class A
    if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
        Serial.printf("ERROR: setting device class failed! \r\n");
        return;
    }
    // ADR
    if (!api.lorawan.adr.set(ADR)) {
        Serial.printf("ERROR: setting adaptive data rate OFF failed! \r\n");
        return;
    }
    // data rate
    if (!api.lorawan.dr.set(DATARATE)) { 
        Serial.printf("ERROR: setting data failed! \r\n");
        return;
    }
    // TX power
    if (!api.lorawan.txp.set(TXPOWER)) { 
        Serial.printf("ERROR: setting tx power failed! \r\n");
        return;
    }
    // duty cycle
    if (!api.lorawan.dcs.set(DC)) {
        Serial.printf("ERROR: setting duty cycle OFF failed! \r\n");
        return;
    }
    // retry
    if (!api.lorawan.rety.set(RETRY)) {
        Serial.printf("ERROR: setting retry OFF failed! \r\n");
        return;
    }
    // confirm mode
    if (!api.lorawan.cfm.set(CFM)) {
        Serial.printf("ERROR: setting confirm mode OFF failed! \r\n");
        return;
    }
  
    // check Lorawan status
    Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF");
    Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");
    uint8_t assigned_dev_addr[4] = { 0 };
    api.lorawan.daddr.get(assigned_dev_addr, 4);
    Serial.printf("Device address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);
    Serial.printf("Transmit power is %d\r\n", api.lorawan.txp.get());
    Serial.println("");

    api.lorawan.registerRecvCallback(recvCallback);
    api.lorawan.registerJoinCallback(joinCallback);
    api.lorawan.registerSendCallback(sendCallback);
}


void lorawan_send(uint8_t * data, int len) {
    // DEBUG data packet
    /**
    Serial.println("Data packet:");
    for (int i = 0; i < len; i++) {
        Serial.printf("0x%02X ", data[i]);
    }
    Serial.println("");
    **/

    // send the data package via lorawan
    if (!api.lorawan.send(len, data, FPORT)) { Serial.println("lorawan_send failed"); }
}
