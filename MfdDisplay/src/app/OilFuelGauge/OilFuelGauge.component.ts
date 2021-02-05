import { Component, Input, OnInit } from '@angular/core';
import { DcsClientService } from '../dcsclient.service';
import { RadialGaugeComponent } from '../RadialGauge/RadialGauge.component';
import { Line, SegmentsSpec } from '../TrackedDevice';

@Component({
  selector: 'app-oil-fuel-gauge',
  templateUrl: './OilFuelGauge.component.html',
  styleUrls: ['./OilFuelGauge.component.css']
})
export class OilFuelGaugeComponent extends RadialGaugeComponent implements OnInit {

  constructor(dcsClient: DcsClientService) {
    super(dcsClient);
  }

  fuelPressureSegments: Line[] = [];
  fuelPressureMinorSegments: Line[] = [];

  oilPressureSegments: Line[] = [];
  oilPressureMinorSegments: Line[] = [];

  oilTemperatureSegments: Line[] = [];
  oilTemperatureMinorSegments: Line[] = [];

  pointerOilTemp: string;
  pointerOilPressure: string;
  pointerFuelPressure: string;

  oilPressureSpec: SegmentsSpec =
    {
      min: 0,
      max: 200,
      reverse: false,
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
    };

  oilTempSpec: SegmentsSpec =
    {
      min: 0,
      max: 100,
      reverse: false,
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
    };

  fuelPressureSpec: SegmentsSpec =
    {
      min: 0,
      max: 25,
      reverse: true,
      startAngle: 0,
      degrees: 180,
      radius: this.radius / 2,
      label: 'TEMP C',
      segmentCount: 5,
      minorSegmentCount: 20,
      centerX: this.radius * 1.2 + this.bezel,
      centerY: this.radius * 1.2 + this.bezel,
      segmentLength: 7,
      numberDecimal: 0
    };

  renderMinorSegementsWithSpec(spec: SegmentsSpec) {
    const segments: Line[] = [];
    const segmentSize = spec.degrees / spec.minorSegmentCount;
    for (let idx = spec.startAngle; idx < spec.startAngle + spec.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      const y = Math.sin(radians);
      const x = Math.cos(radians);
      const segStart = spec.radius - spec.segmentLength / 2;

      segments.push({
        x1: x * segStart + spec.centerX, y1: y * segStart + spec.centerY,
        x2: x * (spec.radius - 1) + spec.centerX, y2: y * (spec.radius - 1) + spec.centerY,
      });
    }

    return segments;
  }

  renderMajorSegmentsWithSpec(spec: SegmentsSpec) {
    const segmentSize = spec.degrees / spec.segmentCount;
    const segments: Line[] = [];

    for (let idx = spec.startAngle; idx <= spec.startAngle + spec.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      const y = Math.sin(radians);
      const x = Math.cos(radians);
      const segStart = spec.radius - spec.segmentLength;
      const labelStart = spec.radius - spec.segmentLength * 2;

      const numerator = idx - spec.startAngle;
      const denominator = spec.degrees;
      const pct = numerator / denominator;
      let tickLabel = (pct * (spec.max - spec.min)) + spec.min;
      if (spec.reverse) {
        tickLabel = spec.max - tickLabel;
      }
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

    this.oilTemperatureSegments = this.renderMajorSegmentsWithSpec(this.oilTempSpec);
    this.oilPressureSegments = this.renderMajorSegmentsWithSpec(this.oilPressureSpec);
    this.fuelPressureSegments = this.renderMajorSegmentsWithSpec(this.fuelPressureSpec);

    this.renderBezel();
  }

  setPointerWithSpec(spec: SegmentsSpec, value: number): string {
    if (spec.reverse) {
      value = spec.max - value;
    }

    //  this.value = value;
    //    this.valueLabel = value.toFixed(spec.numberDecimal) + this.units;
    const workingValue = (((value - spec.min) / (spec.max - spec.min)) * spec.degrees);
    const corrected = (workingValue + spec.startAngle) - 90;

    const leftAngle = corrected - 5;
    const rightAngle = + corrected + 5;

    const radians = (corrected) * Math.PI / 180;
    const radiansLeft = (leftAngle) * (Math.PI / 180);
    const radiansRight = (rightAngle) * (Math.PI / 180);

    const leftStartY = spec.centerY + Math.sin((corrected - 90) * (Math.PI / 180)) * (spec.radius * 0.03);
    const leftStartX = spec.centerX + Math.cos((corrected - 90) * (Math.PI / 180)) * (spec.radius * 0.03);

    const rightStartY = spec.centerY + Math.sin((corrected + 90) * (Math.PI / 180)) * (spec.radius * 0.03);
    const rightStartX = spec.centerX + Math.cos((corrected + 90) * (Math.PI / 180)) * (spec.radius * 0.03);

    const tipY = spec.centerY + Math.sin(radians) * (spec.radius * 0.75);
    const tipX = spec.centerX + Math.cos(radians) * (spec.radius * 0.75);

    const leftY = spec.centerY + Math.sin(radiansLeft) * (spec.radius * 0.6);
    const leftX = spec.centerX + Math.cos(radiansLeft) * (spec.radius * 0.6);

    const rightY = spec.centerY + Math.sin(radiansRight) * (spec.radius * 0.6);
    const rightX = spec.centerX + Math.cos(radiansRight) * (spec.radius * 0.6);

    return `M${rightStartX} ${rightStartY} L${leftStartX} ${leftStartY} L${leftX} ${leftY} L${tipX} ${tipY} L${rightX} ${rightY} Z`;
  }

  @Input('fuel-pressure') set setFuelPressure(value: number) {
    this.pointerFuelPressure = this.setPointerWithSpec(this.fuelPressureSpec, value);
  }

  @Input('oil-pressure') set setOilPressure(value: number) {
    this.pointerOilPressure = this.setPointerWithSpec(this.oilPressureSpec, value);
  }

  @Input('oil-temperature') set setOilTemperature(value: number) {
    this.pointerOilTemp = this.setPointerWithSpec(this.oilTempSpec, value);
  }
}
