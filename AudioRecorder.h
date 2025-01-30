#pragma once

#include "recordingthumbnail.h"

class AudioRecorder  : public AudioIODeviceCallback
{
public:
  AudioRecorder (AudioThumbnail& thumbnailToUpdate)
    : thumbnail (thumbnailToUpdate)
  {
    backgroundThread.startThread();
  }
  
  ~AudioRecorder() override
  {
    stop();
  }
  
  //==============================================================================
  void startRecording (const File& file); 
  
  void stop();
  bool isRecording() const
  {
    return activeWriter.load() != nullptr;
  }
  
  //==============================================================================
  void audioDeviceAboutToStart (AudioIODevice* device) override
  {
    sampleRate = device->getCurrentSampleRate();
  }
  
  void audioDeviceStopped() override
  {
    sampleRate = 0;
  }
  
  void audioDeviceIOCallbackWithContext (const float* const* inputChannelData, int numInputChannels,
                                         float* const* outputChannelData, int numOutputChannels,
                                         int numSamples, const AudioIODeviceCallbackContext& context) override
  {
    ignoreUnused (context);
    
    const ScopedLock sl (writerLock);
    
    if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels()){
      activeWriter.load()->write (inputChannelData, numSamples);
      
      // Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply references our input data
      AudioBuffer<float> buffer (const_cast<float**> (inputChannelData), thumbnail.getNumChannels(), numSamples);
      thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
      nextSampleNum += numSamples;
    }
    
    // We need to clear the output buffers, in case they're full of junk..
    for (int i = 0; i < numOutputChannels; ++i)
      if (outputChannelData[i] != nullptr)
        FloatVectorOperations::clear (outputChannelData[i], numSamples);
  }
  
private:
  AudioThumbnail& thumbnail;
  TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // the thread that will write our audio data to disk
  std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter; // the FIFO used to buffer the incoming data
  double sampleRate = 0.0;
  int64 nextSampleNum = 0;
  
  CriticalSection writerLock;
  std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};
