

#include "DiscoAlert.h"

void defaultDiscoAlertHandler(DiscoAlert* de){

  // do nothing;
  // use this if you just want to print to serial or something simple every time that doesn't need to be set up somehwere else. 

}

DiscoAlert::FireFunc DiscoAlert::fireAlert = defaultDiscoAlertHandler;

void DiscoAlert::setAlertHandler(void (*handler)(DiscoAlert*)){
  fireAlert = handler;
}

DiscoAlert::DiscoAlert(){
  active = false;
  level = 0;
}

DiscoAlert::DiscoAlert(int alevel, char* atext, char* btext){

  level = alevel;
  setText(0, atext);
  setText(1, btext);

  active = false;
}

DiscoAlert::DiscoAlert(int alevel, const __FlashStringHelper* atext, const __FlashStringHelper* btext){

  level = alevel;
  setText(0, atext);
  setText(1, btext);

  active = false;
}


char* DiscoAlert::getText(int aline){
  return text[aline];
}


void DiscoAlert::setText(int aline, char* atext){
  if(aline <= 1){
    strcpy(text[aline], atext);
  }
}

void DiscoAlert::setText(int aline, const __FlashStringHelper* atext){
  if(aline <= 1){
    strcpy_P(text[aline], (char*)atext);
  }
}

int DiscoAlert::getLevel(){
  return level;
}

void DiscoAlert::setLevel(int alev){
  level = alev;
}


boolean DiscoAlert::isActive(){
  return active;
}

void DiscoAlert::setActive(boolean aboo){
  if(active != aboo){
    if(aboo){
      fireAlert(this);
    }
    active = aboo;
  }
}

void DiscoAlert::setActive(boolean aboo, int alev, char* atex0, char* atex1){
  setLevel(alev);
  setText(0, atex0);
  setText(1, atex1);
  setActive(aboo);
}

void DiscoAlert::setActive(boolean aboo, int alev, const __FlashStringHelper* atex0, const __FlashStringHelper* atex1){
  setLevel(alev);
  setText(0, atex0);
  setText(1, atex1);
  setActive(aboo);
}

void DiscoAlert::setActive(boolean aboo, int alev, char* atex0, const __FlashStringHelper* atex1){
  setLevel(alev);
  setText(0, atex0);
  setText(1, atex1);
  setActive(aboo);
}

void DiscoAlert::setActive(boolean aboo, int alev, const __FlashStringHelper* atex0, char* atex1){
  setLevel(alev);
  setText(0, atex0);
  setText(1, atex1);
  setActive(aboo);
}
