import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { RadialGaugeComponent } from './RadialGauge/RadialGauge.component';
import { AltimeterComponent } from './Altimeter/Altimeter.component';
import { VarioMeterComponent } from './VarioMeter/VarioMeter.component';
import { TurnSlipComponent } from './TurnSlip/TurnSlip.component';
import { TemperatureComponent } from './Temperature/Temperature.component';
import { OilFuelGaugeComponent } from './OilFuelGauge/OilFuelGauge.component';

@NgModule({
  declarations: [						
    AppComponent,
      RadialGaugeComponent,
      AltimeterComponent,
      VarioMeterComponent,
      TurnSlipComponent,
      TemperatureComponent,
      OilFuelGaugeComponent
   ],
  imports: [
    BrowserModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
