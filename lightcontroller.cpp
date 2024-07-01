#include "okFrontPanelDLL.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

/*!
 * Created by Nico Müller, 01.07.2024
 * Used as a test for Rebekkas bit-files and as usage example for the
 * okFrontPanel library
 */
bool led_state[] = {false, false, false, false}; // state of lights to track On/Off switching

int main(int argc, char *argv[]) {
  // get bitfile string
  if (argc < 2) {
    printf("ERROR: no bitfile provided");
    return -1;
  }
  std::string bitfile = argv[1];

  if (false == okFrontPanelDLL_LoadLib(NULL)) {
    printf("ERROR FrontPanel DLL could not be loaded.\n");
    return -1;
  }

  okCFrontPanel *dev = new okCFrontPanel();

  if (okCFrontPanel::NoError == dev->OpenBySerial()) {
    printf("Opened device successfully: %s \n",
           dev->GetBoardModelString(dev->GetBoardModel()).c_str());
  } else {
    printf("ERROR: device could not be opened.  is one connected?\n");
    delete dev;
    return -1;
  }

  dev->LoadDefaultPLLConfiguration();

  // print device information
  printf("device firmware version: %d.%d\n", dev->GetDeviceMajorVersion(), dev->GetDeviceMinorVersion());
  printf("device serial number: %s\n", dev->GetSerialNumber().c_str());
  printf("device device ID: %s\n", dev->GetDeviceID().c_str());

  // load bitfile
  if (dev->ConfigureFPGA(bitfile) == okCFrontPanel::NoError) {
    printf("FPGA configuration succeeded.\n");
  } else {
    printf("ERROR: FPGA configuration failed.\n");
    delete dev;
    return -1;
  }

  if (dev->IsFrontPanelEnabled()) {
    printf("FrontPanel support enabled.\n");
  } else {
    printf("ERROR: FrontPanel support is not enabled.\n");
    return -1;
  }
  
  // individual light switches
  while (true) {
    printf("Enter a number 0-3 (-1 to skip): ");
    int input = -1;
    std::cin >> input;
    if (input >= 0 && input <= 3) {
      dev->SetWireInValue(input, !led_state[input]);
      led_state[input] = !led_state[input]; // flip bool
      dev->UpdateWireIns();
    } else if (input == -1) {
      break;
    }
  }

  // bit mask light switch
  while (true) {
    printf("Enter binary flag 0-15 (-1 to exit): ");
    int input = -1;
    std::cin >> input;
    if (input >= 0 && input <= 15) {
      dev->SetWireInValue(0x00, input);
      dev->UpdateWireIns();
    } else if (input == -1) {
      break;
    }
  }
}
