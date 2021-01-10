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
export class AppComponent implements OnInit {
  ngOnInit(): void {

  }
}
