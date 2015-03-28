
#ifndef DoseContainer_h
#define DoseContainer_h

#include "Arduino.h"
#include "DiscoAlert.h"


class DoseContainer {
  
  private:
  
  int containerSize;
  int currentVolume;
  char name[6];
  
  public:
  DiscoAlert dalert;
  
  DoseContainer();
  DoseContainer(char*, int, int);
  int getSize();
  int getCurrentVolume();
  void setCurrentVolume(int);
  void setSize(int);
  void add(int);
  void fill();
  void take(int);
  void checkVolume();
  char* getName();
  void setName(char*);
};



#endif
