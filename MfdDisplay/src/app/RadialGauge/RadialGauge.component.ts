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
  segementCount = 12;
  pointerX = 30;
  pointerY = 30;
  segments: Line[] = [];

  constructor() { }

  ngOnInit() {
    const segmentSize = 360 / this.segementCount;
    let center = this.radius + 2;

    for (let idx = this.start; idx < this.start + this.degrees; idx += segmentSize) {
      const radians = (idx - 90) * Math.PI / 180;
      let y = Math.sin(radians);
      let x = Math.cos(radians);
      let segStart = this.radius - this.segLen;
      let labelStart = this.radius - this.segLen * 2.25;
      this.segments.push({
        x1: x * segStart + center, y1: y * segStart + center,
        x2: x * this.radius + center, y2: y * this.radius + center,
        labelX: x * labelStart + center, labelY: y * labelStart + center,
        label: ((idx - this.start) * this.uintScaler).toString(),
        rotate: idx
      });
    }
    this.setPointer(this.value);
  }

  setPointer(value:number) : void {
    this.value = value;
    let corrected = (this.value + this.start) - 90;

    let center = this.radius + 2;
    let leftAngle = corrected - 90;
    let rightAngle = + corrected + 90;
  
    const radians = (corrected) * Math.PI / 180;
    const radiansLeft = (leftAngle) * (Math.PI / 180);
    const radiansRight = (rightAngle) * (Math.PI / 180);
    let startY = center - Math.sin(radians) * (this.radius * 0.2);
    let startX = center - Math.cos(radians) * (this.radius * 0.2);
    
    let tipY = center + Math.sin(radians) * (this.radius * 0.8);
    let tipX = center + Math.cos(radians) * (this.radius * 0.8);
   
    let leftY = center + Math.sin(radiansLeft) * (this.radius * 0.075);
    let leftX = center + Math.cos(radiansLeft) * (this.radius * 0.075);
   
    let rightY = center + Math.sin(radiansRight) * (this.radius * 0.075);
    let rightX = center + Math.cos(radiansRight) * (this.radius * 0.075);
   
    this.pointer = `M${startX} ${startY} L${leftX} ${leftY} L${tipX} ${tipY} L${rightX} ${rightY} Z`;
  }

  pointer = "M100 100 L90 80 L30 30 L95 105 Z";

  @Input('start') start: number = 0;
  @Input('degrees') degrees: number = 360;

  @Input('label') label: string = "empty";

  @Input('currentValue') set setCurrentValue(value: number) {
    this.setPointer(value);
  }

  @Input('radius') radius: number = 100;
  @Input('unit-scaler') uintScaler: number = 1;

  @Input('segment-count') setSegmentCount(value: number) {
    this.segementCount = value;
  }
}
