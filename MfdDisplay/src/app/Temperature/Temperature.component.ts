import { Component, OnInit } from '@angular/core';
import { RadialGaugeComponent } from '../RadialGauge/RadialGauge.component';
import { Line } from '../TrackedDevice';
import { DcsClientService} from '../dcsclient.service';

@Component({
  selector: 'app-temperature',
  templateUrl: './Temperature.component.html',
  styleUrls: ['./Temperature.component.css']
})
export class TemperatureComponent extends RadialGaugeComponent implements OnInit {

  constructor(private wsClient: DcsClientService) {
    super();
  }

  ngOnInit() {
    this.centerX = this.radius + this.bezel;
    this.centerY = this.radius + this.bezel;
    this.fullWidth = this.centerX * 2;
    this.fullHeight = this.centerY * 2;
    this.markerFontSize = 10;
    this.min = -30;
    this.max = 150;
    this.degrees = 180;
    this.start = 270;
    this.segmentCount = 4;
    this.minorSegmentCount = this.segmentCount * 4
    this.renderMinorSegements();
    this.renderMajorSegments();
    this.renderBezel();

    this.setPointer(this.value);

    if (this.greenStart !== null && this.greenEnd !== null) {
      this.populateInTolerance();
    }

    if (this.redStart !== null && this.redEnd !== null) {
      this.pathUnderTolerance = this.populateOutOfTolerance(this.redStart, this.redEnd);
    }

    if (this.redStart2 !== null && this.redEnd2 !== null) {
      this.pathOverTolerance = this.populateOutOfTolerance(this.redStart2, this.redEnd2);
    }
  }
}
