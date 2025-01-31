#include "AudioRecorder.h"

void AudioRecorder::stop()
{
  // First, clear this pointer to stop the audio callback from using our writer object..
  {
    const ScopedLock sl (writerLock);
    activeWriter = nullptr;
  }

  // Now we can delete the writer object. It's done in this order because the deletion could
  // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
  // the audio callback while this happens.
  threadedWriter.reset();
}



//==============================================================================
void AudioRecorder::startRecording (const File& file)
{
  stop();
  
  if (sampleRate > 0)
    {
      // Create an OutputStream to write to our destination file...
      file.deleteFile();
      
      if (auto fileStream = std::unique_ptr<FileOutputStream> (file.createOutputStream()))
        {
          // Now create a WAV writer object that writes to our output stream...
          WavAudioFormat wavFormat;
          
          if (auto writer = wavFormat.createWriterFor (fileStream.get(), sampleRate, 1, 16, {}, 0))
          // if (auto writer = wavFormat.createWriterFor (fileStream.get(), 16000, 1, 16, {}, 0))            
            {
              fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)
              
              // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
              // write the data to disk on our background thread.
              threadedWriter.reset (new AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768));
              
              // Reset our recording thumbnail
              thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
              nextSampleNum = 0;
              
              // And now, swap over our active writer pointer so that the audio callback will start using it..
              const ScopedLock sl (writerLock);
              activeWriter = threadedWriter.get();
            }
        }
    }
}


bool AudioRecorder::isRecording() const
{
  return activeWriter.load() != nullptr;
}


void AudioRecorder::audioDeviceAboutToStart (AudioIODevice* device)
{
  sampleRate = device->getCurrentSampleRate();
}


void AudioRecorder::audioDeviceStopped()
{
  sampleRate = 0;
}



void AudioRecorder::audioDeviceIOCallbackWithContext (const float* const* inputChannelData, int numInputChannels,
                                       float* const* outputChannelData, int numOutputChannels,
                                       int numSamples, const AudioIODeviceCallbackContext& context)
{
  ignoreUnused (context);
  
  const ScopedLock sl (writerLock);
  
  if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels()){
    activeWriter.load()->write (inputChannelData, numSamples);

    // float *converted = new float[numSamples];    
    
    // auto converter = new AudioData::ConverterInstance <AudioData::Pointer <AudioData::Float32, AudioData::NativeEndian, AudioData::NonInterleaved, AudioData::Const>, AudioData::Pointer <AudioData::Float32, AudioData::NativeEndian, AudioData::NonInterleaved, AudioData::Const> > (1, 1);
    // converter->convertSamples(converted, inputChannelData, numSamples);
    // delete converter; 
    // delete[] converted; 

    // std::unique_ptr<AudioData::Converter> conv (new AudioData::ConverterInstance<AudioData::Pointer<F1, E1, AudioData::NonInterleaved, AudioData::Const>,
    //                                             AudioData::Pointer<F2, E2, AudioData::NonInterleaved, AudioData::NonConst>>());
    // conv->convertSamples (converted, inputChannelData, numSamples);

    AudioBuffer<float> buffer (const_cast<float**> (inputChannelData), thumbnail.getNumChannels(), numSamples);

    // MemoryAudioSource memorySource(buffer, false);
    // ResamplingAudioSource resamplingSource (&memorySource, false, buffer.getNumChannels());
    // const auto resampleRatio = 2.75625;
    // const auto finalSize = roundToInt (buffer.getNumSamples() / resampleRatio);
    // resamplingSource.prepareToPlay (finalSize, sampleRate * resampleRatio);
    // AudioBuffer<float> result (buffer.getNumChannels(), finalSize);
    // resamplingSource.getNextAudioBlock ({ &result, 0, result.getNumSamples() });

    thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
    nextSampleNum += numSamples;
  }
  
  // We need to clear the output buffers, in case they're full of junk..
  for (int i = 0; i < numOutputChannels; ++i)
    if (outputChannelData[i] != nullptr)
      FloatVectorOperations::clear (outputChannelData[i], numSamples);
}