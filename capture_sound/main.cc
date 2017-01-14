// Use the newer ALSA API
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <alsa/control.h>

#include "wave_header.hh"

constexpr int kRecordLengthInMs = 3000;
constexpr int kSamplingFrequency = 9600;
constexpr int kSize = 16;
constexpr int kChannels = 1;
constexpr int kBufferSize = 8;

WaveHeader CreateWaveHeader();
int StartRecord();

int main() {
  StartRecord();
}

WaveHeader CreateWaveHeader() {
  WaveHeader wave_header;
  wave_header.a[0] = 'R';  
  wave_header.a[1] = 'I';  
  wave_header.a[2] = 'F';  
  wave_header.a[3] = 'F';  
  wave_header.b = kRecordLengthInMs * kSamplingFrequency * kChannels * kSize / 8 - 8;  
  wave_header.c[0] = 'W';  
  wave_header.c[1] = 'A';  
  wave_header.c[2] = 'V';  
  wave_header.c[3] = 'E';  
  wave_header.d[0] = 'f';  
  wave_header.d[1] = 'm';  
  wave_header.d[2] = 't';  
  wave_header.d[3] = ' ';  
  wave_header.e =16;  
  wave_header.f =1;  
  wave_header.g = kChannels;  
  wave_header.h = kSamplingFrequency;  
  wave_header.i = kSamplingFrequency * kChannels * kSize/8;  
  wave_header.j = kChannels*kSize/8;  
  wave_header.k = kSize;  
  wave_header.p[0] = 'd';  
  wave_header.p[1] = 'a';  
  wave_header.p[2] = 't';  
  wave_header.p[3] = 'a';  
  wave_header.q = kRecordLengthInMs * kSamplingFrequency * kChannels * kSize / 8;  
  return wave_header;
}

int StartRecord() {
  long loops;  
  int rc;  
  int size;  
  snd_pcm_t *handle;  
  snd_pcm_hw_params_t *capture_params;  
  unsigned int val;  
  int dir;  
  snd_pcm_uframes_t frames;  
  char *buffer;  
  int fd_f;  
  int status; 

  // Open PCM device for recording (capture).
  rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) {
    std::cerr << "Unbale to open pcm device: " << snd_strerror(rc)
              << std::endl;
    return -1;
  } else {
    std::cout << "Opened pcm device: " << rc << std::endl;
  }

  // Allocate a hardware parameters object.
  snd_pcm_hw_params_alloca(&capture_params);

  // Fill it in with default values.
  snd_pcm_hw_params_any(handle, capture_params);

  // Set the desired hardware parameters.
    
  // Interleaved mode
  snd_pcm_hw_params_set_access(handle, capture_params,  
                               SND_PCM_ACCESS_RW_INTERLEAVED);  
    
  // Signed 16-bit little-endian format
  snd_pcm_hw_params_set_format(handle, capture_params,  
                               SND_PCM_FORMAT_S16_LE);  
    
  // Two channels (stereo)
  snd_pcm_hw_params_set_channels(handle, capture_params, kChannels);  
    
  // 44100 bits/second sampling rate (CD quality)
  val = kSamplingFrequency;  
  snd_pcm_hw_params_set_rate_near(handle, capture_params, &val, &dir);  
    
  // Set period size to 32 frames.
  frames = 32;  
  snd_pcm_hw_params_set_period_size_near(handle,  
                                    capture_params, &frames, &dir);  
    
  // Write the parameters to the driver
  rc = snd_pcm_hw_params(handle, capture_params);  
  if (rc < 0) {  
    fprintf(stderr,  
            "unable to set hw parameters: %s\n",  
            snd_strerror(rc));  
    return -1;
  }  
    
  // Use a buffer large enough to hold one period.
  snd_pcm_hw_params_get_period_size(capture_params, &frames, &dir);  
  size = frames * 2; /* 2 bytes/sample, 2 channels */  
  buffer = (char *) malloc(size);  
    
  // We want to loop for 5 seconds
  snd_pcm_hw_params_get_period_time(capture_params, &val, &dir);  
  loops = 5000000 / val;  
  std::cout << "Period duration in us = " << val << std::endl;
  std::cout << "Loops = " << loops << std::endl;
    
  //创建一个wave格式语音文件
  if((fd_f = open("./sound.wav", O_CREAT|O_RDWR, 0777)) == -1) {  
        perror("cannot creat the sound file");  
  }  

  WaveHeader wave_header = CreateWaveHeader();
  //写入wave文件的文件头 {
  if((status = write(fd_f, &wave_header,
                     sizeof(wave_header)))==-1) {
    perror("write to sound'head wrong!!");  
  }  

  while (loops > 0) {
    // std::cout << "Left loops = " << loops << std::endl;
    loops--;  
    rc = snd_pcm_readi(handle, buffer, frames);  
    if (rc == -EPIPE) {  
      // EPIPE means overrun
      fprintf(stderr, "overrun occurred\n");  
      snd_pcm_prepare(handle);  
    } else if (rc < 0) {  
      fprintf(stderr,  
              "error from read: %s\n",  
              snd_strerror(rc));  
    } else if (rc != (int)frames) {  
      fprintf(stderr, "short read, read %d frames\n", rc);  
    }  
                  
    if(write(fd_f, buffer, size)==-1) {  
      perror("write to sound wrong!!");  
    }  
    if (rc != size) { 
      // fprintf(stderr, "short write: wrote %d bytes\n", rc);  
    }
  }  
    
  snd_pcm_drain(handle);  
  snd_pcm_close(handle);  
  free(buffer);  
  close(fd_f);  

  return 0;
}
