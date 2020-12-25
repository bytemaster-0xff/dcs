import { Component, Input, OnInit } from '@angular/core';
import { RadialGaugeComponent } from '../RadialGauge/RadialGauge.component';
import { Line, SegmentsSpec } from '../TrackedDevice';

@Component({
  selector: 'app-oil-fuel-gauge',
  templateUrl: './OilFuelGauge.component.html',
  styleUrls: ['./OilFuelGauge.component.css']
})
export class OilFuelGaugeComponent extends RadialGaugeComponent implements OnInit {

  constructor() {
    super();
  }

  fuelPressureSegments: Line[] = [];
  fuelPressureMinorSegments: Line[] = [];

  oilPressureSegments: Line[] = [];
  oilPressureMinorSegments: Line[] = [];

  oilTemperatureSegments: Line[] = [];
  oilTemperatureMinorSegments: Line[] = [];

  renderMinorSegementsWithSpec(spec: SegmentsSpec) {
    let segments: Line[] = [];
    const segmentSize = spec.degrees / spec.minorSegmentCount;
    for (let idx = spec.startAngle; idx < spec.startAngle + spec.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      let y = Math.sin(radians);
      let x = Math.cos(radians);
      let segStart = spec.radius - spec.segmentLength / 2;

      segments.push({
        x1: x * segStart + spec.centerX, y1: y * segStart + spec.centerY,
        x2: x * (spec.radius - 1) + spec.centerX, y2: y * (spec.radius - 1) + spec.centerY,
      });
    }

    return segments;
  }

  renderMajorSegmentsWithSpec(spec: SegmentsSpec) {
    const segmentSize = spec.degrees / spec.segmentCount;
    let segments: Line[] = [];

    for (let idx = spec.startAngle; idx <= spec.startAngle + spec.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      let y = Math.sin(radians);
      let x = Math.cos(radians);
      let segStart = spec.radius - spec.segmentLength;
      let labelStart = spec.radius - spec.segmentLength * 2;

      const numerator = idx - spec.startAngle;
      const denominator = spec.degrees;
      const pct = numerator / denominator;
      const tickLabel = (pct * (spec.max - spec.min)) + spec.min;

      const label = idx < (spec.startAngle + spec.degrees) || spec.degrees < 360 ? tickLabel.toFixed(spec.numberDecimal) : '';

      segments.push({
        x1: x * segStart + spec.centerX, y1: y * segStart + spec.centerY,
        x2: x * spec.radius + spec.centerX, y2: y * spec.radius + spec.centerY,
        labelX: x * labelStart + spec.centerX, labelY: y * labelStart + spec.centerY,
        label: label,
        rotate: idx
      });
    }

    return segments;
  }

  ngOnInit() {
    this.centerX = this.radius + this.bezel;
    this.centerY = this.radius + this.bezel;
    this.fullWidth = this.centerX * 2;
    this.fullHeight = this.centerY * 2;

    this.oilTemperatureSegments = this.renderMajorSegmentsWithSpec(
      {
        min: 0,
        max: 100,
        startAngle: 270,
        degrees: 180,
        radius: this.radius,
        label: 'TEMP C',
        segmentCount: 10,
        minorSegmentCount: 20,
        centerX: this.radius + this.bezel,
        centerY: this.radius + this.bezel,
        segmentLength: 10,
        numberDecimal: 0
      });

    this.oilPressureSegments = this.renderMajorSegmentsWithSpec(
      {
        min: 0,
        max: 200,
        startAngle: 180,
        degrees: 180,
        radius: this.radius / 2,
        label: 'TEMP C',
        segmentCount: 4,
        minorSegmentCount: 20,
        centerX: this.radius * 0.8 + this.bezel,
        centerY: this.radius * 1.2 + this.bezel,
        segmentLength: 10,
        numberDecimal: 0
      });


    this.fuelPressureSegments = this.renderMajorSegmentsWithSpec(
      {
        min: 0,
        max: 200,
        startAngle: 0,
        degrees: 180,
        radius: this.radius / 2,
        label: 'TEMP C',
        segmentCount: 4,
        minorSegmentCount: 20,
        centerX: this.radius * 1.2 + this.bezel,
        centerY: this.radius * 1.2 + this.bezel,
        segmentLength: 7,
        numberDecimal: 0
      });


    console.log(this.oilPressureSegments);

    this.renderBezel();
  }



  @Input('fuel-pressure') set setFuelPressure(value: number) {
    this.setPointer(value);
  }

  @Input('oil-pressure') set setOilPressure(value: number) {
    this.setPointer(value);
  }

  @Input('oil-temperature') set setOilTemperature(value: number) {
    this.setPointer(value);
  }
}
