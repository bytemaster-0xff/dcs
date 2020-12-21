/* tslint:disable:no-unused-variable */
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';

import { VarioMeterComponent } from './VarioMeter.component';

describe('VarioMeterComponent', () => {
  let component: VarioMeterComponent;
  let fixture: ComponentFixture<VarioMeterComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ VarioMeterComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(VarioMeterComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
