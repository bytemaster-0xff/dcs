import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-aoa-display',
  templateUrl: './AoADisplay.component.html',
  styleUrls: ['./AoADisplay.component.css']
})
export class AoADisplayComponent implements OnInit {

  constructor() { }

  bankInterval = 0.5;
  pitchInterval = 0.5;

  bank = 0;
  pitch = 0;

  ngOnInit() {
    window.setInterval(() => {
      this.bank += this.bankInterval;
      this.pitch += this.pitchInterval;

      if (this.pitch > 30) {
        this.pitchInterval = -.5;
      }

      if (this.pitch < -30) {
        this.pitchInterval = .5;
      }

      if (this.bank > 90) {
        this.bankInterval = -.5;
      }

      if (this.bank < -90) {
        this.bankInterval = .5;
      }
    }, 50);
  }

}
