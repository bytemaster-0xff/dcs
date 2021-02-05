import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { RadialGaugeComponent } from './RadialGauge/RadialGauge.component';
import { AltimeterComponent } from './Altimeter/Altimeter.component';
import { VarioMeterComponent } from './VarioMeter/VarioMeter.component';
import { TurnSlipComponent } from './TurnSlip/TurnSlip.component';
import { TemperatureComponent } from './Temperature/Temperature.component';
import { OilFuelGaugeComponent } from './OilFuelGauge/OilFuelGauge.component';
import { RouterModule, Routes } from '@angular/router';
import { MfdLeftComponent } from './mfd-left/mfd-left.component';
import { MfdRightComponent } from './mfd-right/mfd-right.component';
import { SystemDetailsComponent } from './SystemDetails/SystemDetails.component';
import { MfdCoreComponent } from './mfd-core/mfd-core.component';
import { AoAComponentComponent } from './AoAComponent/AoAComponent.component';

const routes: Routes = [
  { path: 'mfd/left', component: MfdLeftComponent },
  { path: 'gauge/aoa', component: AoAComponentComponent },
  { path: 'mfd/right', component: MfdRightComponent },
  { path: 'sys', component: SystemDetailsComponent },
  { path: 'home', component: AppComponent },
  { path: '', redirectTo: '/home', pathMatch: 'full' },
];

@NgModule({
  declarations: [
    AppComponent,
    RadialGaugeComponent,
    AltimeterComponent,
    VarioMeterComponent,
    TurnSlipComponent,
    TemperatureComponent,
    OilFuelGaugeComponent,
    MfdRightComponent,
    MfdCoreComponent,
    SystemDetailsComponent,
    MfdLeftComponent,
      AoAComponentComponent
   ],
  imports: [
    BrowserModule,
    RouterModule.forRoot(routes)
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
