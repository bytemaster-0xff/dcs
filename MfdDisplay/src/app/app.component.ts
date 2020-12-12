import { Component, OnDestroy, OnInit } from '@angular/core';
import * as aircraftProfile from './Profiles/P51D.json';
import * as commonData from './Profiles/Common.json';
import ExportDataParser from './ExportDataParser';
import { AircraftDevice, TrackedDevice, Line } from './TrackedDevice';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit, OnDestroy {
  title = 'MfdDisplay';

  lineLen = 200;
  interval = 2;
  listeners: TrackedDevice[] = [];
  webSocket: WebSocket;
  dataProcess: ExportDataParser;

  centerX = 150;
  centerY = 150;
  radius = 100;
  segLen = 10;
  value = 1300;
  segementCount = 12;

  rpmValue = 0;

  segments: Line[] = [];

  battery: string;
  generator: string;

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

  registerListener(categoryName: string, parameter: string, changedCallback) {
    var source = categoryName == "Altitude" ||
                 categoryName == "Heading" ||
                 categoryName == "Position" ||
                 categoryName == "Speed" ? commonData : aircraftProfile;

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
      console.log('Battery On Status :' + value);
      this.battery = value === 1 ? 'On' : 'Off';
    });
    this.registerListener('Right Switch Panel', 'GEN', (value) => {
      console.log('Generator Status :' + value);
      this.generator = value === 1 ? 'On' : 'Off';
    });

    this.registerListener('Variometer', 'VARIOMETER_VVI', (value) => { });
    this.registerListener('Altitude', 'ALT_MSL_FT', (value) => { });
    this.registerListener('Heading', 'HDG_DEG', (value) => { });
    this.registerListener('Speed', 'IAS_EU', (value) => { });
    
    this.registerListener('Engine System', 'ENGINE_RPM', (value) => { console.log('Engine RPM :' + value); });
    this.registerListener('Engine System', 'MIXTURE_CONTROL', (value) => { console.log('Mixture Control :' + value); });
    this.registerListener('Engine System', 'PROPELLER_RPM', (value) => { console.log('Engine RPM :' + value); });
    this.registerListener('Engine System', 'THROTTLE', (value) => { console.log('Engine RPM :' + value); });
    this.registerListener('Engine System', 'MANIFOLD_PRESSURE', (value) => { console.log('Manifold Pressure :' + value); });
    
    this.registerListener('Fuel System', 'FUEL_SELECTOR_VALVE', (value) => { console.log('Fuel Selector :' + value); });
    this.registerListener('Fuel System', 'FUEL_SHUT_OFF_VALVE', (value) => { console.log('Fuel Shutoff :' + value); });
    this.registerListener('Fuel System', 'FUEL_PRESSURE', (value) => { console.log('Fuel Pressure :' + value); });
    this.registerListener('Fuel System', 'FUEL_TANK_FUSELAGE', (value) => { console.log('Fuel Fuselage:' + value); });
    this.registerListener('Fuel System', 'FUEL_TANK_LEFT', (value) => { console.log('Fuel Left :' + value); });
    this.registerListener('Fuel System', 'FUEL_TANK_RIGHT', (value) => { console.log('Fuel Right : ' + value); });

    this.registerListener('Engine Control Panel', 'STARTER_COVER', (value) => { });

    this.registerListener('Remote Compass', 'REMOTE_COMPASS_SET', (value) => { });
    this.registerListener('Remote Compass', 'REMOTE_COMPASS_CRS', (value) => { });
    this.registerListener('Remote Compass', 'REMOTE_COMPASS_HDG', (value) => { });

    this.registerListener('Control System', 'RUDDER_TRIM', (value) => { console.log('Rudder Trim :' + value); });
    this.registerListener('Front Switch Box', 'IGNITION', (value) => { console.log('Front Switch Box :' + value); });
    this.registerListener('Control System', 'ELEVATOR_TRIM', (value) => { console.log('Elevator :' + value); });
    this.registerListener('Control System', 'FLAPS_CONTROL_HANDLE', (value) => { console.log('Flaps :' + value); });
    this.registerListener('Control System', 'AILERON_TRIM', (value) => { console.log('Aileron Trim :' + value); });

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

      window.setInterval(() => {
        this.rpmValue += this.interval;
        if(this.rpmValue > 280) {
          this.interval = -1;
        }
        
        if(this.rpmValue < 0) {
          this.interval = 1;
        }
      });

    this.webSocket = new WebSocket('ws://10.1.1.57:5010/api/websocket');
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
}
