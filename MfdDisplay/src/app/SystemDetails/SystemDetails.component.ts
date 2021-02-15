import { Component, OnInit } from '@angular/core';
import { DcsClientService } from '../dcsclient.service';

@Component({
  selector: 'app-SystemDetails',
  templateUrl: './SystemDetails.component.html',
  styleUrls: ['./SystemDetails.component.css']
})
export class SystemDetailsComponent implements OnInit {

  constructor(protected dcsClient: DcsClientService) {
  }

  ngOnInit() {
    this.dcsClient.registerListener('MetadataStart', '_ACFT_NAME', (value) => { });
    this.dcsClient.registerListener('Artificial Horizon', 'AHORIZON_BANK', (value) => {  });
    this.dcsClient.registerListener('Artificial Horizon', 'AHORIZON_PITCH', (value) => {  });
    this.dcsClient.registerListener('Variometer', 'VARIOMETER_VVI', (value) => { });
    this.dcsClient.registerListener('Altitude', 'ALT_MSL_FT', (value) => { });
    this.dcsClient.registerListener('Heading', 'HDG_DEG', (value) => { });
    this.dcsClient.registerListener('Speed', 'IAS_EU', (value) => { });
    this.dcsClient.registerListener('Light System', 'LANDING_GEAR_GREEN', (value) => { });
    this.dcsClient.registerListener('Light System', 'LANDING_GEAR_RED', (value) => { });
    this.dcsClient.registerListener('Gauge Values', 'AMMETER_VALUE', (value) => { });
    
    this.dcsClient.registerListener('Gauge Values', 'ALTIMETER_VALUE', (value) => { });
    this.dcsClient.registerListener('Gauge Values', 'HEADING_VALUE', (value) => { });
    this.dcsClient.registerListener('Gauge Values', 'ALTIMETER_VALUE', (value) => { });
    this.dcsClient.registerListener('Gauge Values', 'ENGINE_RPM_VALUE', (value) => { });
    this.dcsClient.registerListener('Engine System', 'MIXTURE_CONTROL', (value) => { });
    this.dcsClient.registerListener('Engine System', 'PROPELLER_RPM', (value) => { });
    this.dcsClient.registerListener('Engine System', 'THROTTLE', (value) => { });
    this.dcsClient.registerListener('Gauge Values', 'MANIFOLD_PRESSURE_VALUE', (value) => {  });
    this.dcsClient.registerListener('Engine System', 'OIL_PRES', (value) => { });

    this.dcsClient.registerListener('Fuel System', 'FUEL_PRESSURE', (value) => { });
    this.dcsClient.registerListener('Gauge Values', 'FUEL_TANK_FUSELAGE', (value) => { });
    this.dcsClient.registerListener('Gauge Values', 'LEFT_FUEL_TANK_VALUE', (value) => {  });
    this.dcsClient.registerListener('Gauge Values', 'RIGHT_FUEL_TANK_VALUE', (value) => { });

    this.dcsClient.registerListener('Engine Control Panel', 'SUPERCHARGER_SWITCH_COVER', (value) => { });
    this.dcsClient.registerListener('Engine Control Panel', 'SUPERCHARGER_AUTO_LOW_HIGH', (value) => { });
    this.dcsClient.registerListener('Engine Control Panel', 'FUEL_BOOSTER', (value) => { });
    this.dcsClient.registerListener('Engine Control Panel', 'OIL_DILUTE', (value) => { });
    this.dcsClient.registerListener('Engine Control Panel', 'STARTER_COVER', (value) => { });
    this.dcsClient.registerListener('Engine Control Panel', 'STARTER', (value) => { });
    this.dcsClient.registerListener('Engine Control Panel', 'PRIMER', (value) => { });
    this.dcsClient.registerListener('Fuel System', 'FUEL_SELECTOR_VALVE', (value) => { });


    this.dcsClient.registerListener('Front Switch Box', 'IGNITION', (value) => { });
    this.dcsClient.registerListener('Fuel System', 'FUEL_SHUT_OFF_VALVE', (value) => { });



    this.dcsClient.registerListener('Right Switch Panel', 'TAIL_LTS', (value) => { });
    this.dcsClient.registerListener('Right Switch Panel', 'WING_LTS', (value) => { });
    this.dcsClient.registerListener('Right Switch Panel', 'PITOT', (value) => { });
    this.dcsClient.registerListener('Right Switch Panel', 'GUN_HEAT', (value) => { });
    this.dcsClient.registerListener('Right Switch Panel', 'BAT', (value) => { });
    this.dcsClient.registerListener('Right Switch Panel', 'GEN', (value) => { });
    this.dcsClient.registerListener('Right Switch Panel', 'AMBR_REC_LT', (value) => { });
    this.dcsClient.registerListener('Right Switch Panel', 'GRN_REC_LT', (value) => { });
    this.dcsClient.registerListener('Right Switch Panel', 'RED_REC_LT', (value) => { });


    this.dcsClient.registerListener('Remote Compass', 'REMOTE_COMPASS_SET', (value) => { });
    this.dcsClient.registerListener('Remote Compass', 'REMOTE_COMPASS_CRS', (value) => { });
    this.dcsClient.registerListener('Remote Compass', 'REMOTE_COMPASS_HDG', (value) => { });

    this.dcsClient.registerListener('Control System', 'RUDDER_TRIM', (value) => { });
    
    this.dcsClient.registerListener('Control System', 'ELEVATOR_TRIM', (value) => { });
    this.dcsClient.registerListener('Control System', 'FLAPS_CONTROL_HANDLE', (value) => { });
    this.dcsClient.registerListener('Control System', 'AILERON_TRIM', (value) => { });
  }
}
