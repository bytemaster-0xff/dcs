import { Component, OnDestroy, OnInit } from '@angular/core';
import * as aircraftProfile from '../Profiles/P51D.json';
import * as commonData from '../Profiles/Common.json';
import ExportDataParser from '../ExportDataParser';
import { AircraftDevice, TrackedDevice, Line } from '../TrackedDevice';

@Component({
  selector: 'app-mfd-right',
  templateUrl: './mfd-right.component.html',
  styleUrls: ['./mfd-right.component.css']
})
export class MfdRightComponent implements OnInit, OnDestroy {
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
  ias = 0;
  segments: Line[] = [];

  battery: string;
  generator: string;

  fuelLeft = 0;
  fuelRight = 0;

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

  registerListener(categoryName: string, parameter: string, changedCallback) {
    const source = categoryName === 'Altitude' ||
      categoryName === 'Heading' ||
      categoryName === 'Position' ||
      categoryName === 'Speed' ? commonData : aircraftProfile;

    const category = (source as any).default[categoryName];
    if (!category) {
      console.log(`Could not find category for ${categoryName}`);
      return;
    }

    const system = category[parameter] as AircraftDevice;
    if (!system) {
      console.error('Could not find sytem: ' + category + ' ' + parameter);
      return;
    }

    this.listeners.push({
      address: system.outputs[0].address,
      device: system,
      callback: changedCallback
    });
  }


  ngOnInit(): void {
    this.registerListener('Right Switch Panel', 'BAT', (value) => {
      this.battery = value === 1 ? 'On' : 'Off';
    });
    this.registerListener('Right Switch Panel', 'GEN', (value) => {
      this.generator = value === 1 ? 'On' : 'Off';
    });


    this.registerListener('MetadataStart', '_ACFT_NAME', (value) => { this.aircraftName = value; });

    this.registerListener('Variometer', 'VARIOMETER_VVI', (value) => { });
    this.registerListener('Altitude', 'ALT_MSL_FT', (value) => { this.altitude = value; });
    this.registerListener('Heading', 'HDG_DEG', (value) => { this.heading = value; });
    this.registerListener('Speed', 'IAS_EU', (value) => { this.ias = value; });
    this.registerListener('Light System', 'LANDING_GEAR_GREEN', (value) => { });
    this.registerListener('Light System', 'LANDING_GEAR_RED', (value) => { });
    this.registerListener('Electric System', 'AMMETER', (value) => { });
    this.registerListener('Engine System', 'ENGINE_RPM', (value) => { this.rpmValue = Math.round(value / 15) / 100; });
    this.registerListener('Engine System', 'MIXTURE_CONTROL', (value) => { });
    this.registerListener('Engine System', 'PROPELLER_RPM', (value) => { });
    this.registerListener('Engine System', 'THROTTLE', (value) => { });
    this.registerListener('Engine System', 'MANIFOLD_PRESSURE', (value) => { this.manifoldPressure = value / 1000; });
    this.registerListener('Engine System', 'OIL_PRES', (value) => { });
    this.registerListener('Fuel System', 'FUEL_SELECTOR_VALVE', (value) => { });
    this.registerListener('Fuel System', 'FUEL_SHUT_OFF_VALVE', (value) => { });
    this.registerListener('Fuel System', 'FUEL_PRESSURE', (value) => { });
    this.registerListener('Fuel System', 'FUEL_TANK_FUSELAGE', (value) => { });
    this.registerListener('Fuel System', 'FUEL_TANK_LEFT', (value) => { this.fuelLeft = value / 566; });
    this.registerListener('Fuel System', 'FUEL_TANK_RIGHT', (value) => { this.fuelRight = value / 566; });

    this.registerListener('Engine Control Panel', 'STARTER_COVER', (value) => { });

    this.registerListener('Remote Compass', 'REMOTE_COMPASS_SET', (value) => { });
    this.registerListener('Remote Compass', 'REMOTE_COMPASS_CRS', (value) => { });
    this.registerListener('Remote Compass', 'REMOTE_COMPASS_HDG', (value) => { });

    this.registerListener('Control System', 'RUDDER_TRIM', (value) => {console.log(value) });
    this.registerListener('Front Switch Box', 'IGNITION', (value) => { });
    this.registerListener('Control System', 'ELEVATOR_TRIM', (value) => { });
    this.registerListener('Control System', 'FLAPS_CONTROL_HANDLE', (value) => { });
    this.registerListener('Control System', 'AILERON_TRIM', (value) => { });

    this.dataProcess = new ExportDataParser();
    this.dataProcess.onDcsUpdate().subscribe(
      (update) => {
        const byteBuffer = new Uint16Array(update.data);

        for (const listener of this.listeners) {
          if (listener.address === update.address) {
            if (listener.callback) {
              const output = listener.device.outputs[0];
              // tslint:disable-next-line:no-bitwise
              const value = (byteBuffer[0] & output.mask) >> output.shift_by;
              listener.device.currentValue = value;
              listener.callback(value);
            }
          }
        }
      }
    );

    this.webSocket = new WebSocket('ws://10.1.1.38:5010/api/websocket');
    this.webSocket.onopen = (evt) => {
      console.log('opn');
      console.log(evt);

      console.log('Opened Connection.');
      const requestLiveData = JSON.stringify({
        datatype: 'live_data',
        data: {}
      });
      this.webSocket.send(requestLiveData);
    };

    this.webSocket.onmessage = (msg => {
      msg.data.arrayBuffer()
        .then(res => {
          const byteBuffer = new Uint8Array(res);
          for (let idx = 0; idx < byteBuffer.length; ++idx) {
            this.dataProcess.processByte(byteBuffer[idx]);
          }
        });
    });

    this.webSocket.onerror = (evt) => {
      console.log('err');
      console.log(evt);
    };
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
