#pragma once

#include <JuceHeader.h>

class MyProcessor : public AudioProcessor
{
public:

  MyProcessor() { }

  ~MyProcessor() {}
  
private:
  int l;
};
