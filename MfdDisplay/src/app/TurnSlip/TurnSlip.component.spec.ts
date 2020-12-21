/* tslint:disable:no-unused-variable */
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';

import { TurnSlipComponent } from './TurnSlip.component';

describe('TurnSlipComponent', () => {
  let component: TurnSlipComponent;
  let fixture: ComponentFixture<TurnSlipComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ TurnSlipComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(TurnSlipComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
