#pragma once
#include <JuceHeader.h>

#include "recordingthumbnail.h"
#include "AudioRecorder.h"
#include "AnalyserComponent.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent final : public juce::Component,
                            public juce::ChangeListener,
                            public juce::Button::Listener
{
public:
  //==============================================================================
  MainComponent();
  ~MainComponent();
  
  //==============================================================================
  void paint (juce::Graphics&) override;
  void resized() override;
  
  void dumpDeviceInfo();
  void logMessage (const String& m); 
  void buttonClicked (juce::Button* button) override; 

  
private:
  TextButton startButton { TRANS ("Start") };
  TextButton stopButton { TRANS ("Stop") };
  TextButton convertButton { TRANS ("Convert") };
  TextButton quitButton { TRANS ("Quit") };
  TextEditor diagnosticsBox;  
  Label helloWorldLabel { {}, TRANS ("Hello World!") };
  Path internalPath;
  std::unique_ptr<AudioDeviceSelectorComponent> audioSetupComp;  
  AudioDeviceManager audioDeviceManager;
  TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // the thread that will write our audio data to disk
  RecordingThumbnail recordingThumbnail;
  File lastRecording;
  AudioRecorder recorder  { recordingThumbnail.getAudioThumbnail() };

  AnalyserComponent analyserComponent; 

  
  
  void changeListenerCallback (ChangeBroadcaster*) override;
  
  static String getListOfActiveBits (const BigInteger& b)
  {
    StringArray bits;
    
    for (int i = 0; i <= b.getHighestBit(); ++i)
      if (b[i])
        bits.add (String (i));
    
    return bits.joinIntoString (", ");
  }
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
