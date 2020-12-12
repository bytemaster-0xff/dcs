import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { RadialGaugeComponent } from './RadialGauge/RadialGauge.component';

@NgModule({
  declarations: [	
    AppComponent,
      RadialGaugeComponent
   ],
  imports: [
    BrowserModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
