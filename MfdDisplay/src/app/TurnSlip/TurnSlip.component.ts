import { Component, Input, OnInit } from '@angular/core';
import { RadialGaugeComponent } from '../RadialGauge/RadialGauge.component';

@Component({
  selector: 'app-turn-slip',
  templateUrl: './TurnSlip.component.html',
  styleUrls: ['./TurnSlip.component.css']
})
export class TurnSlipComponent extends RadialGaugeComponent implements OnInit { 
  slipX = 50;
  slipY = 20;
  turnPath = "M0 0";
  turnValue = 0;
  slipValue = 0;

  constructor() {
    super();
  }

  setSlipPath(slip) {
    this.slipValue = slip;
    let workingValue = (((slip - this.min) / this.max) * this.degrees);
    let corrected = (workingValue + this.start) - 90;

    let leftAngle = corrected - 10;
    let rightAngle = + corrected + 10;

    const radians = (corrected) * Math.PI / 180;
    const radiansLeft = (leftAngle) * (Math.PI / 180);
    const radiansRight = (rightAngle) * (Math.PI / 180);

    let leftStartY = this.centerY + Math.sin((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);
    let leftStartX = this.centerX + Math.cos((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);

    let rightStartY = this.centerY + Math.sin((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);
    let rightStartX = this.centerX + Math.cos((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);

    let tipY = this.centerY + Math.sin(radians) * (this.radius * 0.60);
    let tipX = this.centerX + Math.cos(radians) * (this.radius * 0.60);

    let leftY = this.centerY + Math.sin(radiansLeft) * (this.radius * 0.4);
    let leftX = this.centerX + Math.cos(radiansLeft) * (this.radius * 0.4);

    let rightY = this.centerY + Math.sin(radiansRight) * (this.radius * 0.4);
    let rightX = this.centerX + Math.cos(radiansRight) * (this.radius * 0.4);

    this.turnPath = `M${rightStartX} ${rightStartY} L${leftStartX} ${leftStartY} L${leftX} ${leftY} L${tipX} ${tipY} L${rightX} ${rightY} Z`;
  }

  ngOnInit() {    
    this.centerX = this.radius + this.bezel;
    this.centerY = this.radius + this.bezel;
    this.fullWidth = this.centerX * 2;
    this.fullHeight = this.centerY * 2;


    this.renderBezel();

    this.setSlipPath(0);
  }

  @Input('turn') turn : number = 0;
  @Input('slip') slip : number = 0;
}
