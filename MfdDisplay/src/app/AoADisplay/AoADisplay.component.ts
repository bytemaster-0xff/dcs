import { Component, OnInit } from '@angular/core';
import { DcsClientService } from '../dcsclient.service';

@Component({
  selector: 'app-aoa-display',
  templateUrl: './AoADisplay.component.html',
  styleUrls: ['./AoADisplay.component.css']
})
export class AoADisplayComponent implements OnInit {

  constructor(protected dcsClient: DcsClientService) { }

  bankInterval = 0.5;
  pitchInterval = 0.5;

  count = 0
  bank = 0;
  pitch = 0;

  updated(){
    this.count++;
  }

  ngOnInit() {
    this.dcsClient.registerListener('Artificial Horizon', 'AHORIZON_BANK', (value) => { this.bank = (value - 32768) / 100.0; });
    this.dcsClient.registerListener('Artificial Horizon', 'AHORIZON_PITCH', (value) => { this.pitch = (value - 32768) / 100.0; this.updated() });


    /*    window.setInterval(() => {
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
        }, 50);*/
  }
}
