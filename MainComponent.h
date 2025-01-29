#pragma once

#include "recordingthumbnail.h"
#include <JuceHeader.h>

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

    void dumpDeviceInfo()
    {
        logMessage ("--------------------------------------");
        logMessage ("Current audio device type: " + (audioDeviceManager.getCurrentDeviceTypeObject() != nullptr
                                                     ? audioDeviceManager.getCurrentDeviceTypeObject()->getTypeName()
                                                     : "<none>"));

        if (AudioIODevice* device = audioDeviceManager.getCurrentAudioDevice())
        {
            logMessage ("Current audio device: "   + device->getName().quoted());
            logMessage ("Sample rate: "    + String (device->getCurrentSampleRate()) + " Hz");
            logMessage ("Block size: "     + String (device->getCurrentBufferSizeSamples()) + " samples");
            logMessage ("Output Latency: " + String (device->getOutputLatencyInSamples())   + " samples");
            logMessage ("Input Latency: "  + String (device->getInputLatencyInSamples())    + " samples");
            logMessage ("Bit depth: "      + String (device->getCurrentBitDepth()));
            logMessage ("Input channel names: "    + device->getInputChannelNames().joinIntoString (", "));
            logMessage ("Active input channels: "  + getListOfActiveBits (device->getActiveInputChannels()));
            logMessage ("Output channel names: "   + device->getOutputChannelNames().joinIntoString (", "));
            logMessage ("Active output channels: " + getListOfActiveBits (device->getActiveOutputChannels()));
        }
        else
        {
            logMessage ("No audio device open");
        }
    }

    void logMessage (const String& m)
    {
        diagnosticsBox.moveCaretToEnd();
        diagnosticsBox.insertTextAtCaret (m + newLine);
    }

  void buttonClicked (juce::Button* button) override
  {
    if (button == &startButton) {
      logMessage("Start");
    } else if (button == &stopButton) {
      logMessage("Stop");
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
  // AudioThumbnail& thumbnail;  


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
