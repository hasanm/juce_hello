#include "AnalyserComponent.h"

AnalyserComponent::AnalyserComponent()
  :forwardFFT(fftOrder),
   window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
  setOpaque(true);
  setAudioChannels(2, 0);
  startTimerHz(30);
  setSize(700, 500);
}

AnalyserComponent::~AnalyserComponent()
{
  shutdownAudio();
  
}

void AnalyserComponent::paint (juce::Graphics& g)
{
  g.fillAll(juce::Colours::black);

  g.setOpacity(1.0f);
  g.setColour(juce::Colours::white);
  drawFrame(g);
}

void AnalyserComponent::drawFrame (juce::Graphics& g)
{
  for (int i = 1; i < scopeSize; i++) {
    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();

    g.drawLine ( { (float) juce::jmap ( i -1, 0, scopeSize -1, 0, width),
        juce::jmap (scopeData[i-1], 0.0f, 1.0f, (float) height, 0.0f),
        (float) juce::jmap (i, 0, scopeSize -1, 0, width),
        juce::jmap (scopeData[i], 0.0f, 1.0f, (float) height, 0.0f) });
    
  } 
} 


void AnalyserComponent::timerCallback() {

  if (nextFFTBlockReady) {
    drawNextFrameOfSpectrum();
    nextFFTBlockReady = false;
    repaint();
  } 
}


void AnalyserComponent::drawNextFrameOfSpectrum() {
  window.multiplyWithWindowingTable (fftData, fftSize);

  forwardFFT.performFrequencyOnlyForwardTransform(fftData);

  auto mindB = -100.0f;
  auto maxdB = 0.0f;

  for (int i = 0; i < scopeSize; ++i) {
    auto skewedProportionX = 1.0f - std::exp (std::log (1.0f - (float) i / (float) scopeSize) * 0.2f);
    auto fftDataIndex = juce::jlimit (0, fftSize / 2, (int) (skewedProportionX * (float) fftSize * 0.5f));
    auto level = juce::jmap (juce::jlimit (mindB, maxdB, juce::Decibels::gainToDecibels (fftData[fftDataIndex])
                                           - juce::Decibels::gainToDecibels ((float) fftSize)),
                             mindB, maxdB, 0.0f, 1.0f);
    scopeData[i] = level; 
  } 
}

void AnalyserComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill){

  if (bufferToFill.buffer->getNumChannels() > 0) {
    auto* channelData = bufferToFill.buffer->getReadPointer (0, bufferToFill.startSample);

    for (auto i = 0; i < bufferToFill.numSamples; ++i) {
      pushNextSampleIntoFifo(channelData[i]);
    } 
  } 
  
}
