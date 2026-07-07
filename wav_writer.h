#ifndef __WAV_WRITER_H
#define __WAV_WRITER_H
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
int WAV_WriteFile(const char *filename,
                  const void *pcmData,
                  unsigned int pcmDataSize,
                  unsigned int sampleRate,
                  unsigned short numChannels,
                  unsigned short bitsPerSample);
#ifdef __cplusplus
}
#endif
#endif 
