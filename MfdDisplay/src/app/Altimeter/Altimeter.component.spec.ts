/* tslint:disable:no-unused-variable */
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';

import { AltimeterComponent } from './Altimeter.component';

describe('AltimeterComponent', () => {
  let component: AltimeterComponent;
  let fixture: ComponentFixture<AltimeterComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ AltimeterComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(AltimeterComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
