/* tslint:disable:no-unused-variable */
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';

import { MfdLeftComponent } from './mfd-left.component';

describe('MfdLeftComponent', () => {
  let component: MfdLeftComponent;
  let fixture: ComponentFixture<MfdLeftComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ MfdLeftComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(MfdLeftComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
