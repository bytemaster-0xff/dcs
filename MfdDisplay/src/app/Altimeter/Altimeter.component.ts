import { Component, Input, OnInit } from '@angular/core';
import { inherits } from 'util';
import { RadialGaugeComponent } from '../RadialGauge/RadialGauge.component';

@Component({
  selector: 'app-Altimeter',
  templateUrl: './Altimeter.component.html',
  styleUrls: ['./Altimeter.component.css']
})
export class AltimeterComponent extends RadialGaugeComponent implements OnInit {

  constructor() {
    super();
  }

  pointer10KFoot = "";
  pointer1KFoot = "";
  pointer100Foot = "";

  tenKX: number;
  tenKY: number;
  center: number;

  set10KPointer(value: number)
  {
    value *= 10;
    let workingValue = (((value - this.min) / this.max) * this.degrees);
    let corrected = (workingValue + this.start) - 90;

    let center = this.radius + 2;
    this.center = center;
    let leftAngle = corrected - 4;
    let rightAngle = + corrected + 4;

    const radians = (corrected) * Math.PI / 180;
    const radiansLeft = (leftAngle) * (Math.PI / 180);
    const radiansRight = (rightAngle) * (Math.PI / 180);

    let startLeftY = center + Math.sin(radiansLeft) * (this.radius * 1);
    let startLeftX = center + Math.cos(radiansLeft) * (this.radius * 1);

    let startRightY = center + Math.sin(radiansRight) * (this.radius * 1);
    let startRightX = center + Math.cos(radiansRight) * (this.radius * 1);

    let tipY = center + Math.sin(radians) * (this.radius * 0.95);
    let tipX = center + Math.cos(radians) * (this.radius * 0.95);
    let pointerStartY = center + Math.sin(radians) * (this.radius * 0.90);
    let pointerStartX = center + Math.cos(radians) * (this.radius * 0.90);

    this.tenKX = tipX;
    this.tenKY = tipY

    this.pointer10KFoot = `M${pointerStartX} ${pointerStartY} L${startLeftX} ${startLeftY} L${startRightX} ${startRightY} Z`;
  }

  set1KPointer(value: number)
  {
    value *= 10;
    let workingValue = (((value - this.min) / this.max) * this.degrees);
    let corrected = (workingValue + this.start) - 90;

    let center = this.radius + 2;
    let leftAngle = corrected - 10;
    let rightAngle = + corrected + 10;

    const radians = (corrected) * Math.PI / 180;
    const radiansLeft = (leftAngle) * (Math.PI / 180);
    const radiansRight = (rightAngle) * (Math.PI / 180);
   
    let tipY = center + Math.sin(radians) * (this.radius * 0.5);
    let tipX = center + Math.cos(radians) * (this.radius * 0.5);

    let leftStartY = center + Math.sin((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);
    let leftStartX = center + Math.cos((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);

    let rightStartY = center + Math.sin((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);
    let rightStartX = center + Math.cos((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);

    let leftY = center + Math.sin(radiansLeft) * (this.radius * 0.3);
    let leftX = center + Math.cos(radiansLeft) * (this.radius * 0.3);

    let rightY = center + Math.sin(radiansRight) * (this.radius * 0.3);
    let rightX = center + Math.cos(radiansRight) * (this.radius * 0.3);

    this.pointer1KFoot = `M${rightStartX} ${rightStartY} L${leftStartX} ${leftStartY} L${leftX} ${leftY} L${tipX} ${tipY} L${rightX} ${rightY} Z`;
  }

  set100FtPointer(value: number)
  {
    value *= 10;
    let workingValue = (((value - this.min) / this.max) * this.degrees);
    let corrected = (workingValue + this.start) - 90;

    let center = this.radius + 2;
    let leftAngle = corrected - 3;
    let rightAngle = + corrected + 3;

    const radians = (corrected) * Math.PI / 180;
    const radiansLeft = (leftAngle) * (Math.PI / 180);
    const radiansRight = (rightAngle) * (Math.PI / 180);

    let leftStartY = center + Math.sin((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);
    let leftStartX = center + Math.cos((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);

    let rightStartY = center + Math.sin((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);
    let rightStartX = center + Math.cos((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);

    let tipY = center + Math.sin(radians) * (this.radius * 0.85);
    let tipX = center + Math.cos(radians) * (this.radius * 0.85);

    let leftY = center + Math.sin(radiansLeft) * (this.radius * 0.75);
    let leftX = center + Math.cos(radiansLeft) * (this.radius * 0.75);

    let rightY = center + Math.sin(radiansRight) * (this.radius * 0.75);
    let rightX = center + Math.cos(radiansRight) * (this.radius * 0.75);

    this.pointer100Foot = `M${rightStartX} ${rightStartY} L${leftStartX} ${leftStartY} L${leftX} ${leftY} L${tipX} ${tipY} L${rightX} ${rightY} Z`;
  }
  
  ngOnInit() {
    this.segmentCount = 10
    this.max = 10;
    super.ngOnInit();
  }
  
  setPointer(value: number) : void {
    this.value = value;
   
    this.valueLabel = value.toFixed(this.numberDecimal) + this.units;
    this.set100FtPointer((value % 1000) / 1000);
    this.set1KPointer((value % 10000) / 10000); 
    this.set10KPointer((value % 100000) / 100000);
  }
}
