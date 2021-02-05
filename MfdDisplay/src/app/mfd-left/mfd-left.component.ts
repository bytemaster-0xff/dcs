import { Component, OnDestroy, OnInit } from '@angular/core';
import * as aircraftProfile from '../Profiles/P51D.json';
import * as commonData from '../Profiles/Common.json';
import ExportDataParser from '../ExportDataParser';
import { AircraftDevice, TrackedDevice, Line } from '../TrackedDevice';
import { DcsClientService } from '../dcsclient.service';

@Component({
  selector: 'app-mfd-left',
  templateUrl: './mfd-left.component.html',
  styleUrls: ['./mfd-left.component.css']
})
export class MfdLeftComponent implements OnInit, OnDestroy {
  title = 'MfdDisplay';

  lineLen = 200;
  interval = 2;
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
  ias = 0;
  segments: Line[] = [];

  battery: string;
  generator: string;

  fuelLeft = 0;
  fuelRight = 0;

  constructor(protected dcsClient: DcsClientService) {
  }

  ngOnDestroy(): void {
    if (this.webSocket) {
      this.webSocket.close();
      this.webSocket = null;
    }
  }

  public toggleBat(): void {
    let msg = 'BAT ';
    if (this.battery === 'Off') {
      msg += '1';
    } else {
      msg += '0';
    }

    const json = JSON.stringify({
      datatype: 'input_command',
      data: msg
    });

    this.webSocket.send(json);
  }

  sendBoolAction(action: string, value: boolean) {
    if (value) {
      action += ' 1';
    } else {
      action += ' 0';
    }

    const json = JSON.stringify({
      datatype: 'input_command',
      data: action
    });

    console.log(json);

    this.webSocket.send(json);
  }


  ngOnInit(): void {
    this.dcsClient.registerListener('Right Switch Panel', 'BAT', (value) => {
      this.battery = value === 1 ? 'On' : 'Off';
    });
    this.dcsClient.registerListener('Right Switch Panel', 'GEN', (value) => {
      this.generator = value === 1 ? 'On' : 'Off';
    });


    this.dcsClient.registerListener('MetadataStart', '_ACFT_NAME', (value) => { this.aircraftName = value; });

    this.dcsClient.registerListener('Variometer', 'VARIOMETER_VVI', (value) => { });
    this.dcsClient.registerListener('Altitude', 'ALT_MSL_FT', (value) => { this.altitude = value; });
    this.dcsClient.registerListener('Heading', 'HDG_DEG', (value) => { this.heading = value; });
    this.dcsClient.registerListener('Speed', 'IAS_EU', (value) => { this.ias = value; });
    this.dcsClient.registerListener('Light System', 'LANDING_GEAR_GREEN', (value) => { });
    this.dcsClient.registerListener('Light System', 'LANDING_GEAR_RED', (value) => { });
    this.dcsClient.registerListener('Electric System', 'AMMETER', (value) => { });
    this.dcsClient.registerListener('Engine System', 'ENGINE_RPM', (value) => { this.rpmValue = Math.round(value / 15) / 100; });
    this.dcsClient.registerListener('Engine System', 'MIXTURE_CONTROL', (value) => { });
    this.dcsClient.registerListener('Engine System', 'PROPELLER_RPM', (value) => { });
    this.dcsClient.registerListener('Engine System', 'THROTTLE', (value) => { });
    this.dcsClient.registerListener('Engine System', 'MANIFOLD_PRESSURE', (value) => { this.manifoldPressure = value / 1000; });
    this.dcsClient.registerListener('Engine System', 'OIL_PRES', (value) => { });
    this.dcsClient.registerListener('Fuel System', 'FUEL_SELECTOR_VALVE', (value) => { });
    this.dcsClient.registerListener('Fuel System', 'FUEL_SHUT_OFF_VALVE', (value) => { });
    this.dcsClient.registerListener('Fuel System', 'FUEL_PRESSURE', (value) => { });
    this.dcsClient.registerListener('Fuel System', 'FUEL_TANK_FUSELAGE', (value) => { });
    this.dcsClient.registerListener('Fuel System', 'FUEL_TANK_LEFT', (value) => { this.fuelLeft = value / 566; });
    this.dcsClient.registerListener('Fuel System', 'FUEL_TANK_RIGHT', (value) => { this.fuelRight = value / 566; });

    this.dcsClient.registerListener('Engine Control Panel', 'STARTER_COVER', (value) => { });

    this.dcsClient.registerListener('Remote Compass', 'REMOTE_COMPASS_SET', (value) => { });
    this.dcsClient.registerListener('Remote Compass', 'REMOTE_COMPASS_CRS', (value) => { });
    this.dcsClient.registerListener('Remote Compass', 'REMOTE_COMPASS_HDG', (value) => { });

    this.dcsClient.registerListener('Control System', 'RUDDER_TRIM', (value) => { console.log(value) });
    this.dcsClient.registerListener('Front Switch Box', 'IGNITION', (value) => { });
    this.dcsClient.registerListener('Control System', 'ELEVATOR_TRIM', (value) => { });
    this.dcsClient.registerListener('Control System', 'FLAPS_CONTROL_HANDLE', (value) => { });
    this.dcsClient.registerListener('Control System', 'AILERON_TRIM', (value) => { });
  }

  setValue(action: string, value: number) {
    action += ` ${value}`;
    const json = JSON.stringify({
      datatype: 'input_command',
      data: action
    });

    console.log(json);

    this.webSocket.send(json);
  }

  startAction(action: string) {
    this.sendBoolAction(action, true);
  }

  endAction(action: string) {
    this.sendBoolAction(action, false);
  }
}
