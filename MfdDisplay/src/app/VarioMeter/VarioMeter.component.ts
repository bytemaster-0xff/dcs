import { Component, OnInit } from '@angular/core';
import { RadialGaugeComponent } from '../RadialGauge/RadialGauge.component';
import { Line } from '../TrackedDevice';

@Component({
  selector: 'app-variometer',
  templateUrl: './VarioMeter.component.html',
  styleUrls: ['./VarioMeter.component.css']
})
export class VarioMeterComponent extends RadialGaugeComponent implements OnInit { 

  constructor() {
    super();
   }

   upSegments : Line[] = [];
   upSegmentsMajor : Line[] = [];
   downSegments : Line[] = [];
   upSegmentsMinor : Line[] = []; 
   upRefArrow : string;
   downRefArrow: string;

   renderUpSegments() {
    let segStart = this.radius - this.markerLen;
    let labelStart = this.radius - this.markerLen * 2;
     
    let angles : number[] = [0, 0.5, 1, 2, 3, 4, 5, 6];

    for (let idx of angles) {
      let theta = Math.log10(idx + 1) * 200;
      
      const radiansClimb = (theta + 180) * Math.PI / 180;
      let y = Math.sin(radiansClimb);
      let x = Math.cos(radiansClimb);

      let label = idx.toString()
      if(idx < 1) {
        label = label.substr(1);
      }
      if(idx == 3 || idx == 5){
        label = ''
      }

     this.upSegments.push({
        x1: x * segStart + this.centerX, y1: y * segStart + this.centerY,
        x2: x * (this.radius - 1) + this.centerX, y2: y * (this.radius - 1) + this.centerY,
        labelX: x * labelStart + this.centerX, labelY: y * labelStart + this.centerY,
        rotate: theta,
        label: label
      });
    
      const radiansDescend = (180 - theta) * Math.PI / 180;
      y = Math.sin(radiansDescend);
      x = Math.cos(radiansDescend);
     
      this.downSegments.push({
        x1: x * segStart + this.centerX, y1: y * segStart + this.centerY,
        x2: x * (this.radius - 1) + this.centerX, y2: y * (this.radius - 1) + this.centerY,
        labelX: x * labelStart + this.centerX, labelY: y * labelStart + this.centerY,
        rotate: theta,
        label: label
      });
    }

    angles = [0.1, 0.2, 0.3, 0.4, 0.6, 0.7, 0.8, 0.9, 1.2, 1.4, 1.6, 1.8, 2.5, 3.5, 4.5, 5.5];

    for (let idx of angles) {
      let theta = Math.log10(idx + 1) * 200;
      
      const radiansClimb = (theta + 180) * Math.PI / 180;
      let y = Math.sin(radiansClimb);
      let x = Math.cos(radiansClimb);
     
     this.upSegmentsMinor .push({
        x1: x * segStart + this.centerX, y1: y * segStart + this.centerY,
        x2: x * (this.radius - 1) + this.centerX, y2: y * (this.radius - 1) + this.centerY,
      });
    
      const radiansDescend = (180 - theta) * Math.PI / 180;
      y = Math.sin(radiansDescend);
      x = Math.cos(radiansDescend);
     
      this.upSegmentsMinor.push({
        x1: x * segStart + this.centerX, y1: y * segStart + this.centerY,
        x2: x * (this.radius - 1) + this.centerX, y2: y * (this.radius - 1) + this.centerY,
      });
    }
  }

  setPointer(value: number): void {
    this.value = value;

    let theta = (Math.log10(Math.abs(this.value) + 1) * 200);
    let corrected = value > 0 ? (theta + 180) : (180 - theta);
    
    let leftAngle = corrected - 5;
    let rightAngle = corrected + 5;

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

  renderDownSegements() {
    for (let idx = 180; idx >= 0; idx -= 10) {
      const radians = (idx - 90) * Math.PI / 180;
      let y = Math.sin(radians);
      let x = Math.cos(radians);
      let segStart = this.radius - this.markerLen;
     
     this.downSegments.push({
        x1: x * segStart + this.centerX, y1: y * segStart + this.centerY,
        x2: x * (this.radius - 1) + this.centerX, y2: y * (this.radius - 1) + this.centerY,
      });
    }
}

  ngOnInit() {    
    this.centerX = this.radius + this.bezel;
    this.centerY = this.radius + this.bezel;
    this.fullWidth = this.centerX * 2;
    this.fullHeight = this.centerY * 2;

    let x = this.bezel + this.radius * 0.22;
    let y = this.bezel + this.radius - (this.radius * 0.1);

    this.upRefArrow = `M${x} ${y} L${x+4.4} ${y-10} L${x+7} ${y-3} M${x+5} ${y-11} L${x-3} ${y-7}`;
  
    y = this.bezel + this.radius + (this.radius * 0.1);
    this.downRefArrow = `M${x} ${y} L${x+4.4} ${y+10} L${x+7} ${y+3} M${x+5} ${y+11} L${x-3} ${y+7}`;
    console.log('ref', this.upRefArrow)

    this.renderBezel();
    this.renderUpSegments();
    this.setPointer(this.value);
  }
}
