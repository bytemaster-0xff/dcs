#include <Arduino.h>
#include <Servo.h>

//#define TESTMODE

#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

//#define AUX_PANEL
//#define FUEL_IGN
#define TRIM_PANEL
//#define LANDING
//#define GAUGE_BANK_1

#ifdef GAUGE_BANK_1
DcsBios::ServoOutput engineRpm(0x5078, 11, 544, 2400);
#endif

#ifdef SIX_PACK
DcsBios::ServoOutput ahorizonBank(0x503e,10, 544, 2400);
DcsBios::ServoOutput ahorizonPitch(0x5040,11, 544, 2400);
#endif
\

//#define LANDING
#ifdef LANDING
DcsBios::PushButton landingGearControl1Handle("LANDING_GEAR_CONTR0L_HANDLE", 2);
DcsBios::PushButton landingGearControl2Handle("LANDING_GEAR_CONTR0L_HANDLE", 3, true);
DcsBios::Switch2Pos unsafeLndGearLtTest("UNSAFE_LND_GEAR_LT_TEST", 4);
DcsBios::Switch2Pos safeLndGearLtTest("SAFE_LND_GEAR_LT_TEST", 5);

DcsBios::LED landingGearGreen(0x500e, 0x2000, 6);
DcsBios::LED landingGearRed(0x500e, 0x4000, 7);
void initialize() {}
#endif

#define TRIM_PANEL
#ifdef TRIM_PANEL
DcsBios::SwitchMixture mixtureControl("MIXTURE_CONTROL", 2, 3);

DcsBios::RotaryEncoder rudderTrim("RUDDER_TRIM", "-2400", "+2400", 4, 5);
DcsBios::RotaryEncoder aileronTrim("AILERON_TRIM", "-2400", "+2400", 6, 7);
DcsBios::RotaryEncoder elevatorTrime("ELEVATOR_TRIM", "-2400", "+2400", 8, 9);

//DcsBios::ServoOutput flapsControlHandleOutput(0x502a, 11, 544, 2400);

//DcsBios::PotentiometerEWMA<5, 128, 5> propellerRpm("PROPELLER_RPM", A0);
//DcsBios::PotentiometerEWMA<5, 128, 5> carbColdAir("CARB_COLD_AIR", A1);
//DcsBios::PotentiometerEWMA<5, 128, 5> carbCoWarmAir("CARB_WARM_AIR", A2);
DcsBios::PotentiometerEWMA<5, 128, 5> flapsControlHandle("FLAPS_CONTROL_HANDLE", A3);
String VERISON = "TRIM_PANEL";  

void initialize() {}
#endif

//#define AUX_PANEL
#ifdef AUX_PANEL
//DcsBios::Switch2Pos tailLts("AUX5_SW1", 5);
//DcsBios::Switch2Pos wingLts("AUX5_SW2", 6);

DcsBios::PushButton openCanopy("CANOPY_HAND_CRANK", "0", 5);
DcsBios::PushButton closeCanopy("CANOPY_HAND_CRANK", "65535", 6);

DcsBios::RotaryEncoder canopyHandCrank("CANOPY_HAND_CRANK", "-3200", "+3200", 5, 6);

DcsBios::Switch2Pos piot("AUX4_SW1", 7);
DcsBios::Switch2Pos gunHeat("AUX4_SW2", 8);
DcsBios::Switch2Pos bat("AUX3_SW1", 9);
DcsBios::Switch2Pos gen("AUX3_SW2", 10);
DcsBios::Switch2Pos amberRecLt("AUX2", 11);
DcsBios::Switch2Pos greenRecLt("AUX1", 12);

void initialize() {}
#endif


//#define RIGHT_PANEL
#ifdef RIGHT_PANEL
DcsBios::Switch2Pos tailLts("TAIL_LTS", 2);
DcsBios::Switch2Pos wingLts("WING_LTS", 3);
DcsBios::Switch2Pos piot("PITOT", 4);
DcsBios::Switch2Pos gunHeat("GUN_HEAT", 5);
DcsBios::Switch2Pos bat("BAT", 6);
DcsBios::Switch2Pos gen("GEN", 7);
DcsBios::Switch2Pos amberRecLt("AMBR_REC_LT", 8);
DcsBios::Switch2Pos greenRecLt("GRN_REC_LT", 9);
DcsBios::Switch2Pos redRecLt("RED_REC_LT", 10);




unsigned int mapAmmeter(unsigned int newValue) {  
    // right boundary = 800 = 200 amps
    // left boundary = 2150 = 0 amps
    // range = 1350
    // scaler = 6.75;
    

     return 2150 - (newValue * 6.75);
}

DcsBios::ServoOutput ammeter(0x50b0,11, 800, 2150, mapAmmeter);

#ifdef TESTMODE
Servo _servo = Servo();
#endif

void initialize() {  
  DcsBios::sendDcsBiosMessage("STARTER_COVER", "1");
  DcsBios::sendDcsBiosMessage("SUPERCHARGER_SWITCH_COVER", "1");
}
#endif

//#define FUEL_IGN
#ifdef FUEL_IGN

DcsBios::Switch3Pos superCharger("SUPERCHARGER_AUTO_LOW_HIGH", 11, 12);
DcsBios::Switch2Pos fuelBooster("FUEL_BOOSTER", 10);
DcsBios::Switch2Pos oilDilute("OIL_DILUTE", 9);
DcsBios::Switch2Pos starter("STARTER", 8);
DcsBios::Switch2Pos primer("PRIMER", 7);
DcsBios::Switch2Pos fuelShutOffValve("FUEL_SHUT_OFF_VALVE", 6);
DcsBios::LED highBlowerLamp(0x502e, 0x0008, 5);

// Make sure you modify SwitchMultiPos to ignore checking anything with a -1
//const byte ignitionPins[7] = {255, 8, 9, 10};
DcsBios::Switch4UniquePos ignition("IGNITION", A0, A1, A2, A3);
DcsBios::FuelShutoff fuelSelectorValve("FUEL_SELECTOR_VALVE", A4, A5, A6);

void initialize() {
  DcsBios::sendDcsBiosMessage("STARTER_COVER", "1");
  DcsBios::sendDcsBiosMessage("SUPERCHARGER_SWITCH_COVER", "1");
}

#endif

void cmdHandler(unsigned char cmd, unsigned int address, int value)
{ 
  switch (cmd)
  {
  case 0x00:
    DcsBios::sendState();
    initialize();
    break;
  }
}

void setup()
{
  Serial.println("Welcome - Calibration Mode");

#ifndef TESTMODE  
  DcsBios::setup();  
  DcsBios::RegisterCommandCallback(cmdHandler);  
#else
  //_servo.attach(11);  
  Serial.begin(115200);
  
#endif  
}

void loop()
{
#ifndef TESTMODE  
  DcsBios::loop();  
#else
  String input = Serial.readStringUntil('\n');
  if (input.length() > 0)
  {
    if (input == "zero")
    {
      Serial.println("zero");
    }
    else
    {
      String cmd = input.substring(0, input.indexOf(" "));
      String param = input.substring(input.indexOf(" ") + 1);
      Serial.println("Handling: " + cmd + " - " + param);
      int paramValue = atoi(param.c_str());
      if (cmd == "setstep")
      {
//        onStepperValueChanged(paramValue);
      }
      else if (cmd == "servo")
      {
   //     _servo.write(paramValue);
      }
    }
  }
#endif
}