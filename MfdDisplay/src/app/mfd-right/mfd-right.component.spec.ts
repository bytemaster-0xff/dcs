/* tslint:disable:no-unused-variable */
import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { By } from '@angular/platform-browser';
import { DebugElement } from '@angular/core';

import { MfdRightComponent } from './mfd-right.component';

describe('MfdRightComponent', () => {
  let component: MfdRightComponent;
  let fixture: ComponentFixture<MfdRightComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ MfdRightComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(MfdRightComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
