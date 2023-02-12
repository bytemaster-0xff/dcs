import { isFormattedError } from '@angular/compiler';
import { Component, OnDestroy, OnInit } from '@angular/core';
import { DcsClientService } from '../dcsclient.service';
import ExportDataParser from '../ExportDataParser';
import { Line, TrackedDevice } from '../TrackedDevice';

@Component({
  selector: 'app-main',
  templateUrl: './main.component.html',
  styleUrls: ['./main.component.css']
})
export class MainComponent implements OnInit, OnDestroy {
  title = 'MfdDisplay';

  lineLen = 200;
  interval = 2;
  listeners: TrackedDevice[] = [];
  webSocket: WebSocket;
  dataProcess: ExportDataParser;

  aircraftName = 'No Aircraft';

  centerX = 150;
  centerY = 150;
  radius = 100;
  segLen = 10;
  value = 1300;
  segementCount = 12;
  manifoldPressure = 0;
  rpmValue = 16;
  altitude = 0;
  heading = 0;
  airspeed = 0;
  segments: Line[] = [];

  oilTemperature = 0;
  oilPressure = 0;
  fuelPressure = 0;

  carbTemperature = 0;
  coolantTemperature = 0;

  climbRate = 0;

  battery: string;
  generator: string;

  fuelLeft = 0;
  fuelRight = 0;

  slip = 0;
  turn = 0;

  bank = 0;
  pitch = 0;

  caged= false;

  constructor(protected dcsClient: DcsClientService) {

  }

  ngOnInit(): void {
    this.dcsClient.registerListener('Metadata', '_ACFT_NAME', (value) => { this.aircraftName = value; });

    this.dcsClient.registerListener('Turn Indicator', 'SLIPBALL', (value) => { this.slip = (value - 32767) / 400; });
    this.dcsClient.registerListener('Turn Indicator', 'TURN_INDICATOR', (value) => { this.turn = (value - 32767) / 10000; });

    this.dcsClient.registerListener('Variometer', 'VARIOMETER_VVI', (value) => { this.climbRate = (value - 32767) / 10000; });

    this.dcsClient.registerListener('Gauge Values', 'ALTIMETER_VALUE', (value) => { this.altitude = value; });
    this.dcsClient.registerListener('Gauge Values', 'HEADING_VALUE', (value) => { this.heading = value; });
    this.dcsClient.registerListener('Gauge Values', 'AIRSPEED_MPH_VALUE', (value) => { this.airspeed = value; });
    this.dcsClient.registerListener('Gauge Values', 'MANIFOLD_PRESSURE_VALUE', (value) => { this.manifoldPressure = value; });
    this.dcsClient.registerListener('Gauge Values', 'ENGINE_RPM_VALUE', (value) => { this.rpmValue = value / 100.0; });
    this.dcsClient.registerListener('Gauge Values', 'OIL_PRESSURE_VALUE', (value) => { this.oilPressure = value; });
    this.dcsClient.registerListener('Gauge Values', 'OIL_TEMPERATURE_VALUE', (value) => { this.oilTemperature = value; });
    this.dcsClient.registerListener('Gauge Values', 'FUEL_PRESSURE_VALUE', (value) => { this.fuelPressure = value; });
    this.dcsClient.registerListener('Gauge Values', 'FUEL_PRESSURE_VALUE', (value) => { this.fuelPressure = value; });

    this.dcsClient.registerListener('Light System', 'LANDING_GEAR_GREEN', (value) => { });
    this.dcsClient.registerListener('Light System', 'LANDING_GEAR_RED', (value) => { });
    this.dcsClient.registerListener('Electric System', 'AMMETER', (value) => { });

    this.dcsClient.registerListener('Artificial Horizon', 'AHORIZON_CAGED', (value) => {
         console.log('is caged', value)
         this.caged = value > 0;
      });
    this.dcsClient.registerListener('Artificial Horizon', 'AHORIZON_PITCH', (value) => { this.pitch = (value - 32768) / 500.0; });


    this.dcsClient.registerListener('Engine System', 'CARB_TEMP', (value) => { this.carbTemperature = value / 1000.0; });
    this.dcsClient.registerListener('Engine System', 'COOLANT_TEMP', (value) => { this.coolantTemperature = value / 1000.0; });

    this.dcsClient.registerListener('Fuel System', 'FUEL_TANK_FUSELAGE', (value) => { });
    this.dcsClient.registerListener('Fuel System', 'FUEL_TANK_LEFT', (value) => { this.fuelLeft = value / 566; });
    this.dcsClient.registerListener('Fuel System', 'FUEL_TANK_RIGHT', (value) => { this.fuelRight = value / 566; });
  }

  ngOnDestroy(): void {
    if (this.webSocket) {
      this.webSocket.close();
      this.webSocket = null;
    }
  }

  toggleCaged() {
    if(!this.caged){
      this.setValue('CAGE_BUTTON', 1)
      this.caged = true;
    }
    else {
      this.setValue('CAGE_BUTTON', 0)
      this.caged = false;
    }

  }
  
  setValue(action: string, value: number) {
    action += ` ${value}`;
    const json = JSON.stringify({
      datatype: 'input_command',
      data: action
    });

    console.log(json);

    this.dcsClient.webSocket.send(json);
  }

}
