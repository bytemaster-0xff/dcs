import { Component, Input, OnInit } from '@angular/core';
import { createProviderInstance } from '@angular/core/src/view/provider';
import { Line } from '../TrackedDevice';

@Component({
  selector: 'app-RadialGauge',
  templateUrl: './RadialGauge.component.html',
  styleUrls: ['./RadialGauge.component.css']
})
export class RadialGaugeComponent implements OnInit {
  value = 0;
  pointerX = 30;
  pointerY = 30;
  segments: Line[] = [];
  minorSegments: Line[] = [];
  valueLabel = "0.0";
  centerX: number;
  centerY: number;

  fullWidth: number;
  fullHeight: number;

  pathInTolerance = "";
  pathOverTolerance = "";

  bezelPath = "M0 0";

  constructor() { }

  renderMinorSegements() {
    const segmentSize = this.degrees / this.minorSegmentCount;
    for (let idx = this.start; idx <= this.start + this.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      let y = Math.sin(radians);
      let x = Math.cos(radians);
      let segStart = this.radius - this.markerLen / 2;
     
     this.minorSegments.push({
        x1: x * segStart + this.centerX, y1: y * segStart + this.centerY,
        x2: x * (this.radius - 1) + this.centerX, y2: y * (this.radius - 1) + this.centerY,
      });
    }

    console.log(this.minorSegments);
  }

  renderMajorSegments() {
    const segmentSize = this.degrees / this.segmentCount;
   
    for (let idx = this.start; idx <= this.start + this.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      let y = Math.sin(radians);
      let x = Math.cos(radians);
      let segStart = this.radius - this.markerLen;
      let labelStart = this.radius - this.markerLen * 2;
     
      const numerator = idx - this.start;
      const denominator =  this.degrees;
      const pct = numerator / denominator;
      const tickLabel = (pct * (this.max - this.min)) + this.min;
     
      const label = idx < (this.start + this.degrees) || this.degrees < 360 ? tickLabel.toFixed(this.numberDecimal) : '';
       
      this.segments.push({
        x1: x * segStart + this.centerX, y1: y * segStart + this.centerY,
        x2: x * this.radius + this.centerX, y2: y * this.radius + this.centerY,
        labelX: x * labelStart + this.centerX, labelY: y * labelStart + this.centerY,
        label: label,
        rotate: idx
      });
    }
  }

  renderBezel() {
    let bezelOffset1 = this.fullWidth * 0.25;
    let bezelOffset2 = this.fullWidth * 0.04;

    this.bezelPath = `M${bezelOffset1} 0 `;
    this.bezelPath += `L${this.fullWidth - bezelOffset1} 0 `;
    this.bezelPath += `L${this.fullWidth - bezelOffset2} ${bezelOffset2} `;
    this.bezelPath += `L${this.fullWidth} ${bezelOffset1} `;
    this.bezelPath += `L${this.fullWidth} ${this.fullHeight - bezelOffset1} `;
    this.bezelPath += `L${this.fullWidth - bezelOffset2} ${this.fullHeight - bezelOffset2} `;
    this.bezelPath += `L${this.fullWidth - bezelOffset1} ${this.fullHeight} `;
    this.bezelPath += `L${bezelOffset1} ${this.fullHeight} `;
    this.bezelPath += `L${bezelOffset2} ${this.fullHeight - bezelOffset2} `;
    this.bezelPath += `L0 ${this.fullHeight - bezelOffset1} `;
    this.bezelPath += `L0 ${bezelOffset1} `;
    this.bezelPath += `L${bezelOffset2} ${bezelOffset2} `;
    this.bezelPath += 'Z';
  }

  ngOnInit() {
    this.centerX = this.radius + this.bezel;
    this.centerY = this.radius + this.bezel;
    this.fullWidth = this.centerX * 2;
    this.fullHeight = this.centerY * 2;

    this.renderBezel();
    this.renderMajorSegments();

    if(this.minorSegmentCount > 0)
    {
      this.renderMinorSegements();
    }

    this.setPointer(this.value); 

    if(this.greenStart !== null && this.greenEnd !== null)
    {
      this.populateInTolerance();
    }
 
    if(this.redStart !== null && this.redEnd !== null)
    {
      this.populateOverTolerance();
    }
  }

  populateInTolerance() {
    let start = (((this.greenStart - this.min) / (this.max - this.min)) * this.degrees);
    let startDegrees = ((start + this.start) - 90);

    let end = (((this.greenEnd - this.min) / (this.max - this.min)) * this.degrees);
    let endDegrees = ((end + this.start) - 90);

    let startY = this.centerY + Math.sin((startDegrees) * (Math.PI / 180)) * (this.radius * 0.9);
    let startX = this.centerX + Math.cos((startDegrees) * (Math.PI / 180)) * (this.radius * 0.9);

    let endY = this.centerY + Math.sin((endDegrees) * (Math.PI / 180)) * (this.radius * 0.9);
    let endX = this.centerX + Math.cos((endDegrees) * (Math.PI / 180)) * (this.radius * 0.9);

    this.pathInTolerance = `M${startX} ${startY} A ${this.radius} ${this.radius} ${end-start}, 0 1, ${endX} ${endY}`;
  }

  populateOverTolerance() {
    let start = (((this.redStart - this.min) / (this.max - this.min)) * this.degrees);
    let startDegrees = ((start + this.start) - 90);

    let end = (((this.redEnd - this.min) / (this.max - this.min)) * this.degrees);
    let endDegrees = ((end + this.start) - 90);

    let startY = this.centerY + Math.sin((startDegrees) * (Math.PI / 180)) * (this.radius * 0.9);
    let startX = this.centerX + Math.cos((startDegrees) * (Math.PI / 180)) * (this.radius * 0.9);

    let endY = this.centerY + Math.sin((endDegrees) * (Math.PI / 180)) * (this.radius * 0.9);
    let endX = this.centerX + Math.cos((endDegrees) * (Math.PI / 180)) * (this.radius * 0.9);

    this.pathOverTolerance = `M${startX} ${startY} A ${this.radius} ${this.radius} ${end-start}, 0 1, ${endX} ${endY}`;
    console.log("RED", this.pathOverTolerance);
  }


  setPointer(value: number): void {
      this.value = value;
      this.valueLabel = value.toFixed(this.numberDecimal) + this.units;
      let workingValue = (((value - this.min) / (this.max - this.min)) * this.degrees);
      let corrected = (workingValue + this.start) - 90;

      let leftAngle = corrected - 5;
      let rightAngle = + corrected + 5;

      const radians = (corrected) * Math.PI / 180;
      const radiansLeft = (leftAngle) * (Math.PI / 180);
      const radiansRight = (rightAngle) * (Math.PI / 180);

      let leftStartY = this.centerY + Math.sin((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);
      let leftStartX = this.centerX + Math.cos((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);
  
      let rightStartY = this.centerY + Math.sin((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);
      let rightStartX = this.centerX + Math.cos((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);
  
      let tipY = this.centerY + Math.sin(radians) * (this.radius * 0.75);
      let tipX = this.centerX + Math.cos(radians) * (this.radius * 0.75);

      let leftY = this.centerY + Math.sin(radiansLeft) * (this.radius * 0.6);
      let leftX = this.centerX + Math.cos(radiansLeft) * (this.radius * 0.6);

      let rightY = this.centerY + Math.sin(radiansRight) * (this.radius * 0.6);
      let rightX = this.centerX + Math.cos(radiansRight) * (this.radius * 0.6);

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

  @Input('bezel') bezel: number = 20;
  @Input('min') min: number = 0;
  @Input('max') max: number = 100;
  @Input('number-decimal') numberDecimal: number = 0;

  @Input('segment-count') segmentCount: number = 12;

  @Input('marker-len') markerLen: number = 10;
  @Input('marker-font-size') markerFontSize: number = 16;

  @Input('minor-segment-count') minorSegmentCount: number = 0;

  @Input('green-start') greenStart: number = null;
  @Input('green-end') greenEnd: number = null;
  @Input('red-start') redStart: number = null;;
  @Input('red-end') redEnd: number = null;
}
