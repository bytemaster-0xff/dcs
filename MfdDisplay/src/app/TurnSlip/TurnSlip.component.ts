import { Component, Input, OnInit } from '@angular/core';
import { DcsClientService } from '../dcsclient.service';
import { RadialGaugeComponent } from '../RadialGauge/RadialGauge.component';

@Component({
  selector: 'app-turn-slip',
  templateUrl: './TurnSlip.component.html',
  styleUrls: ['./TurnSlip.component.css']
})
export class TurnSlipComponent extends RadialGaugeComponent implements OnInit { 
  @Input('turn') turn : number = 0;
  @Input('slip') slip : number = 0;
  
  slipX = 50;
  slipY = 20;
  turnPath = 'M0 0';
  turnValue = 0;
  slipValue = 0;

  
  constructor(dcsClient: DcsClientService) {
    super(dcsClient);
  }


  setSlipPath(slip) {
    this.slipValue = slip;
    const workingValue = (((slip - this.min) / this.max) * this.degrees);
    const corrected = (workingValue + this.start) - 90;

    const leftAngle = corrected - 10;
    const rightAngle = + corrected + 10;

    const radians = (corrected) * Math.PI / 180;
    const radiansLeft = (leftAngle) * (Math.PI / 180);
    const radiansRight = (rightAngle) * (Math.PI / 180);

    const leftStartY = this.centerY + Math.sin((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);
    const leftStartX = this.centerX + Math.cos((corrected - 90) * (Math.PI / 180)) * (this.radius * 0.03);

    const rightStartY = this.centerY + Math.sin((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);
    const rightStartX = this.centerX + Math.cos((corrected + 90) * (Math.PI / 180)) * (this.radius * 0.03);

    const tipY = this.centerY + Math.sin(radians) * (this.radius * 0.60);
    const tipX = this.centerX + Math.cos(radians) * (this.radius * 0.60);

    const leftY = this.centerY + Math.sin(radiansLeft) * (this.radius * 0.4);
    const leftX = this.centerX + Math.cos(radiansLeft) * (this.radius * 0.4);

    const rightY = this.centerY + Math.sin(radiansRight) * (this.radius * 0.4);
    const rightX = this.centerX + Math.cos(radiansRight) * (this.radius * 0.4);

    this.turnPath = `M${rightStartX} ${rightStartY} L${leftStartX} ${leftStartY} L${leftX} ${leftY} L${tipX} ${tipY} L${rightX} ${rightY} Z`;
  }

  ngOnInit() {
    this.centerX = this.radius + this.bezel;
    this.centerY = this.radius + this.bezel;
    this.fullWidth = this.centerX * 2;
    this.fullHeight = this.centerY * 2;


    this.renderBezel();

    this.setSlipPath(5);
  }
}
