import { Component, Input, OnInit } from '@angular/core';
import { createProviderInstance } from '@angular/core/src/view/provider';
import { Line } from '../TrackedDevice';

@Component({
  selector: 'app-RadialGauge',
  templateUrl: './RadialGauge.component.html',
  styleUrls: ['./RadialGauge.component.css']
})
export class RadialGaugeComponent implements OnInit {
  segLen = 10;
  value = 0;
  pointerX = 30;
  pointerY = 30;
  segments: Line[] = [];
  minorSegments: Line[] = [];
  valueLabel = "0.0";

  constructor() { }

  renderMinorSegements() {
    const segmentSize = this.degrees / this.minorSegmentCount;
    let center = this.radius + 2;

    for (let idx = this.start; idx <= this.start + this.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      let y = Math.sin(radians);
      let x = Math.cos(radians);
      let segStart = this.radius - this.segLen / 2;
     
     this.minorSegments.push({
        x1: x * segStart + center, y1: y * segStart + center,
        x2: x * this.radius + center, y2: y * this.radius + center,
      });
    }

    console.log(this.minorSegments);
  }

  ngOnInit() {
    const segmentSize = this.degrees / this.segmentCount;
    let center = this.radius + 2;

    for (let idx = this.start; idx <= this.start + this.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      let y = Math.sin(radians);
      let x = Math.cos(radians);
      let segStart = this.radius - this.segLen;
      let labelStart = this.radius - this.segLen * 2;
     
      const numerator = idx - this.start;
      const denominator =  this.degrees;
      const pct = numerator / denominator;
      const tickLabel = (pct * this.max) + this.min;
     
      const label = idx < (this.start + this.degrees) || this.degrees < 360 ? tickLabel.toFixed(this.numberDecimal) : '';
       
      this.segments.push({
        x1: x * segStart + center, y1: y * segStart + center,
        x2: x * this.radius + center, y2: y * this.radius + center,
        labelX: x * labelStart + center, labelY: y * labelStart + center,
        label: label,
        rotate: idx
      });
    }

    if(this.minorSegmentCount > 0)
    {
      this.renderMinorSegements();
    }

    this.setPointer(this.value); 
  }

  setPointer(value: number): void {
      this.value = value;
      this.valueLabel = value.toFixed(this.numberDecimal) + this.units;
      let workingValue = (((value - this.min) / this.max) * this.degrees);
      let corrected = (workingValue + this.start) - 90;

      let center = this.radius + 2;
      let leftAngle = corrected - 5;
      let rightAngle = + corrected + 5;

      const radians = (corrected) * Math.PI / 180;
      const radiansLeft = (leftAngle) * (Math.PI / 180);
      const radiansRight = (rightAngle) * (Math.PI / 180);

      let leftStartY = center + Math.sin((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);
      let leftStartX = center + Math.cos((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);
  
      let rightStartY = center + Math.sin((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);
      let rightStartX = center + Math.cos((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);
  
      let tipY = center + Math.sin(radians) * (this.radius * 0.8);
      let tipX = center + Math.cos(radians) * (this.radius * 0.8);

      let leftY = center + Math.sin(radiansLeft) * (this.radius * 0.65);
      let leftX = center + Math.cos(radiansLeft) * (this.radius * 0.6);

      let rightY = center + Math.sin(radiansRight) * (this.radius * 0.6);
      let rightX = center + Math.cos(radiansRight) * (this.radius * 0.6);

      this.pointer = `M${rightStartX} ${rightStartY} L${leftStartX} ${leftStartY} L${leftX} ${leftY} L${tipX} ${tipY} L${rightX} ${rightY} Z`;
  }

  pointer = "M100 100 L90 80 L30 30 L95 105 Z";

  @Input('start') start: number = 0;
  @Input('degrees') degrees: number = 360;

  @Input('label') label: string = "empty";

  @Input('units') units: string = "";

  @Input('currentValue') set setCurrentValue(value: number) {
    this.setPointer(value);
  }

  @Input('radius') radius: number = 100;
  @Input('unit-scaler') uintScaler: number = 1;

  @Input('min') min: number = 0;
  @Input('max') max: number = 100;
  @Input('number-decimal') numberDecimal: number = 0;

  @Input('segment-count') segmentCount: number = 12;

  @Input('minor-segment-count') minorSegmentCount: number = 0;
}
