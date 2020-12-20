import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { RadialGaugeComponent } from './RadialGauge/RadialGauge.component';
import { AltimeterComponent } from './Altimeter/Altimeter.component';

@NgModule({
  declarations: [		
    AppComponent,
      RadialGaugeComponent,
      AltimeterComponent
   ],
  imports: [
    BrowserModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
