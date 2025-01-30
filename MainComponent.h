#pragma once
#include <JuceHeader.h>

#include "recordingthumbnail.h"
#include "AudioRecorder.h"


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
  void buttonClicked (juce::Button* button) override
  {
    if (button == &startButton) {
      logMessage("Start");
      SafePointer<MainComponent> safeThis (this);
      auto parentDir = File::getSpecialLocation (File::userDocumentsDirectory);
      lastRecording = parentDir.getNonexistentChildFile ("JUCERecording", ".wav");
      recorder.startRecording(lastRecording);
    } else if (button == &stopButton) {
      logMessage("Stop");
      recorder.stop();
    } 
  }
  
private:
  TextButton startButton { TRANS ("Start") };
  TextButton stopButton { TRANS ("Stop") };
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

  
  
  void changeListenerCallback (ChangeBroadcaster*) override
  {
    dumpDeviceInfo();
  }  
  
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
