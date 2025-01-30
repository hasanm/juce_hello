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