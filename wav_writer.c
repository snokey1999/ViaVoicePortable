#include "wav_writer.h"
#include <string.h>
#pragma pack(push, 1)
typedef struct {
    char     riffId[4];        
    unsigned int   riffSize;   
    char     waveId[4];        
    char     fmtId[4];         
    unsigned int   fmtSize;    
    unsigned short audioFormat; 
    unsigned short numChannels;
    unsigned int   sampleRate;
    unsigned int   byteRate;   
    unsigned short blockAlign; 
    unsigned short bitsPerSample;
    char     dataId[4];        
    unsigned int   dataSize;   
} WAVHeader;
#pragma pack(pop)
int WAV_WriteFile(const char *filename,
                  const void *pcmData,
                  unsigned int pcmDataSize,
                  unsigned int sampleRate,
                  unsigned short numChannels,
                  unsigned short bitsPerSample)
{
    FILE *fp;
    WAVHeader header;
    if (!filename || !pcmData || pcmDataSize == 0) return -1;
    fp = fopen(filename, "wb");
    if (!fp) return -1;
    memcpy(header.riffId, "RIFF", 4);
    header.riffSize = 36 + pcmDataSize;
    memcpy(header.waveId, "WAVE", 4);
    memcpy(header.fmtId, "fmt ", 4);
    header.fmtSize = 16;
    header.audioFormat = 1;  
    header.numChannels = numChannels;
    header.sampleRate = sampleRate;
    header.bitsPerSample = bitsPerSample;
    header.blockAlign = numChannels * (bitsPerSample / 8);
    header.byteRate = sampleRate * header.blockAlign;
    memcpy(header.dataId, "data", 4);
    header.dataSize = pcmDataSize;
    if (fwrite(&header, sizeof(WAVHeader), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    if (fwrite(pcmData, 1, pcmDataSize, fp) != pcmDataSize) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}
