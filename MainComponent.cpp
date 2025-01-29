#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
  audioSetupComp.reset (new AudioDeviceSelectorComponent (audioDeviceManager,
                                                          0, 256, 0, 256, true, true, true, false));
  addAndMakeVisible (audioSetupComp.get());  
  
  addAndMakeVisible (helloWorldLabel);

  helloWorldLabel.setFont (FontOptions (40.00f, Font::bold));
  helloWorldLabel.setJustificationType (Justification::centred);
  helloWorldLabel.setEditable (false, false, false);
  helloWorldLabel.setColour (Label::textColourId, Colours::black);
  helloWorldLabel.setColour (TextEditor::textColourId, Colours::black);
  helloWorldLabel.setColour (TextEditor::backgroundColourId, Colour (0x00000000));


  addAndMakeVisible (diagnosticsBox);
  diagnosticsBox.setMultiLine (true);
  diagnosticsBox.setReturnKeyStartsNewLine (true);
  diagnosticsBox.setReadOnly (true);
  diagnosticsBox.setScrollbarsShown (true);
  diagnosticsBox.setCaretVisible (false);
  diagnosticsBox.setPopupMenuEnabled (true);  

  addAndMakeVisible (startButton);
  addAndMakeVisible (stopButton);
  addAndMakeVisible (quitButton);

  quitButton.onClick = [] { JUCEApplication::quit(); };  
  setSize (600, 400);
}

MainComponent::~MainComponent()
{
  audioDeviceManager.removeChangeListener (this);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::FontOptions (16.0f));
    g.setColour (juce::Colours::white);
    // g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{

  helloWorldLabel.setBounds (152, 80, 296, 48);
  startButton.setBounds (getWidth() - 576, getHeight() - 60, 120, 32);
  stopButton.setBounds (getWidth() - 376, getHeight() - 60, 120, 32);  
  quitButton.setBounds (getWidth() - 176, getHeight() - 60, 120, 32);


  auto r =  getLocalBounds().reduced (4);
  audioSetupComp->setBounds (r.removeFromTop (proportionOfHeight (0.65f)));  
  diagnosticsBox.setBounds (r);  
}
