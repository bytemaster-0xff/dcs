import { Component, Input, OnInit } from '@angular/core';
import { DcsClientService } from '../dcsclient.service';
import { RadialGaugeComponent } from '../RadialGauge/RadialGauge.component';

@Component({
  selector: 'app-turn-slip',
  templateUrl: './TurnSlip.component.html',
  styleUrls: ['./TurnSlip.component.css']
})
export class TurnSlipComponent extends RadialGaugeComponent implements OnInit {
  @Input() turn = 0;
  @Input() slip = 0;

  ballX = 180;
  ballY = 180;

  slipX = 50;
  slipY = 20;
  turnPath = 'M0 0';

    markerPath = 'M0 -7.5 L5 -2.5 L5 7.5 L-5 7.5 L-5 -2.5 Z';
 // slipPath = 'M 0 5 L 0 25 A 1 0.5, 180, 0 0, 180 25 L 180 5  A 1 0.5, -180, 0 1, 00 5 Z';
   slipPath = `M 0 5 L 0 25 A 1 1, 180, 0 0, ${this.fullWidth / 2} 0 Z`;
  slipTransform = 'translate(130,300)';

  turnValue = 0;
  slipValue = 0;

  translateIt(x: number, y: number): string {
    let trs = `translate(${x*this.fullWidth}, ${y*this.fullHeight})`;
    return trs;
  }

  constructor(dcsClient: DcsClientService) {
    super(dcsClient);
  }

  setSlip(slip: number) {
    this.ballX = (slip * 1.3) + this.centerX;
     this.ballY = 13 * Math.cos((Math.PI / 180.0) * slip) + (this.fullHeight * 0.705);

  }

  setTurnPath(turn) {
    this.turnValue = turn;
    const workingValue = (((turn - this.min) / this.max) * this.degrees);
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

  dir = 0;

  ngOnInit() {
    this.centerX = this.radius + this.bezel;
    this.centerY = this.radius + this.bezel;
    this.fullWidth = this.centerX * 2;
    this.fullHeight = this.centerY * 2;

    window.setInterval(() => {
      if(this.dir == 0) {
        if(this.slip > 90) {
          this.dir = 1
        }
        else {
          this.slip += 2;
        }
      }
      else {
        if(this.slip < -90) {
          this.dir = 0;
        }
        else
        this.slip -= 2;
      }
      this.setSlip(this.slip);
    }, 50)


    this.renderBezel();

    this.setTurnPath(5);

//M 0 0 L 0 0 A 30 10, 0, 0 0, 300 50 L 300 0 L 0 0 Z
//M 0 0 L 50 0 A 30 10, 0, 0 0, 300 50 L 300 0 L 0 0 Z
    let rx = 60;
    let ry = 10;
    let rotate = 0;
    let sweepFlag = 0;
    let largeArgFlag = 0;
    let x = 270;
    let y = 34;

    this.slipPath = ` M 0 0 L 0 ${y} A ${rx} ${ry}, ${rotate}, ${largeArgFlag} ${sweepFlag}, ${x} ${y} L ${x} 0 L 0 0 Z`;
    console.log(this.slipPath)

    this.ballX = this.centerX;
    this.ballY = this.fullHeight * 0.73

  }
}
