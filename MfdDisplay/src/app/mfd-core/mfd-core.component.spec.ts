/* tslint:disable:no-unused-variable */
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';

import { MfdCoreComponent } from './mfd-core.component';

describe('MfdCoreComponent', () => {
  let component: MfdCoreComponent;
  let fixture: ComponentFixture<MfdCoreComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ MfdCoreComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(MfdCoreComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
