#pragma once

#include <JuceHeader.h>

class AnalyserComponent : public juce::AudioAppComponent,
                          private juce::Timer
                                

{
public:
  //==============================================================================
  AnalyserComponent();
  ~AnalyserComponent() override;
  
  //==============================================================================
  void paint (juce::Graphics&) override;


  void prepareToPlay (int, double) override {}
  void releaseResources() override          {}
  
  void getNextAudioBlock (const juce::AudioSourceChannelInfo&) override;

  void timerCallback() override; 

  void pushNextSampleIntoFifo (float) noexcept
  {
    // if the fifo contains enough data, set a flag to say
    // that the next frame should now be rendered..
  }

  void drawNextFrameOfSpectrum();
  void drawFrame (juce::Graphics&);

  enum {
    fftOrder = 11,
    fftSize = 1 << fftOrder,
    scopeSize = 512
  };
  
  
private:
  juce::dsp::FFT forwardFFT; 
  juce::dsp::WindowingFunction<float> window;

  float fifo [fftSize];
  float fftData[2 * fftSize];
  int fifoIndex = 0;
  bool nextFFTBlockReady = false;
  float scopeData [scopeSize];
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserComponent)
};
