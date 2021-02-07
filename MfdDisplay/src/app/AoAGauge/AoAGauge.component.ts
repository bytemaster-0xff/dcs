import { Component, Input, OnInit } from '@angular/core';

@Component({
  selector: 'app-aoa-gauge',
  templateUrl: './AoAGauge.component.html',
  styleUrls: ['./AoAGauge.component.css']
})
export class AoAGaugeComponent implements OnInit {
  pitchTransform: string;
  bankTransform: string;

  @Input() set pitch(value: number) {
    this.pitchTransform = `translate(0 ${value * 4})`;
  }

  @Input() set bank(value: number) {
    this.bankTransform = `rotate(${value + 90} 240 240)`;
  }

  constructor() { }

  ngOnInit() {
    this.bankTransform = `rotate(90 240 240)`;
    this.pitchTransform = `translate(0 0)`;
  }
}
