

#ifndef ANDROID_AUDIO_I2S_INPUT_H
#define ANDROID_AUDIO_I2S_INPUT_H



#include <stdint.h>
#include <sys/types.h>


#define I2S_MAX_USER (128)
#define I2S_Buffer_Count (2)

// I2S read once size
#define I2S_INPUT_BUFFER_SIZE (0x2400)
// I2S buffer size
#define EDI_BUFFER_SIZE (0x2400)
#define MATV_SAMPLERATE (32000)
#define FM_SAMPLERATE   (48000)

#define EDI_SETTING_BITS_MASK (0x1FFE)
#define DATA_DIR_BITS_MASK (1<<8)
#define I2S_CLOCK_SOURCE_MASK (1<<7)
#define CLOCK_CYCLE_MASK (0x007C)
#define EDI_FORMAT_MASK (1<<1)
#define EDI_ENABLE_MASK (1<<0)

#define EDI_ENABLE_BITS (1)
#define EDI_DISABLE_BITS (0)
#define EDI_FORMAT_I2S  (1<<1)
#define WCYCLE16    (0x3c)
#define WCYCLE32   (0x7c)
#define CLOCK_SOURCE_EXTERNAL (1<<7)
#define SERIAL_DATA_INPUT_MODE (1<<8)

namespace android {

class AudioAfe;



// when call I2S start , need parameters for I2STYPE
typedef enum{
    MATV,                         //I2S Input For ATV
    FMRX,                         //I2S Input For FMRX
    FMRX_32K,                 //I2S Input For FMRX_32K
    FMRX_48K,                 //I2S Input For FMRX_48K
    I2S0OUTPUT,             //   I2S0 output
    I2S1OUTPUT,             //   I2S1 output
    HOA_SAMPLERATE,   //   use for HQA support
    NUM_OF_I2S
}I2STYPE;

typedef enum {
   I2S_NONE                = 0x0,
   I2S_CREATED          = 0x1,
   I2S0_INPUT_START   = 0x2,
   I2S0_OUTPUT_START   = 0x4,
   I2S1_OUTPUT_START   = 0x8,
   I2S_CAPABILITY = I2S1_OUTPUT_START|I2S0_OUTPUT_START|I2S0_INPUT_START
}I2SSTATE;

typedef struct
{
   char *Buffer;
   bool Valid;
}I2SInputBuffer;

typedef struct
{
   public:
   uint32_t Identity ;
   I2SInputBuffer InputBuffer[I2S_Buffer_Count];
   int32_t  Buf_idx;
   bool IsOccupied;
   bool IsStart;
}I2SClient;

typedef struct
{
   int status;
   int mode;
   int sampleRate;
}DriverInfo;


class AudioI2S
{
public:

   static AudioI2S* getInstance();
   static void freeInstance();
   static AudioI2S *UniqueI2SInstance;
   static int32_t GetI2SCapability();

   int32_t GetState(void);
   int32_t GetAudioHandle(void);
   uint32_t samplerate();
   int32_t GetSourceType();
   uint32_t GetReadBufferSize(void);

   I2SClient* GetI2SClient(int index);
   bool FindFreeClient(int32_t* ClientIndex);
   bool FindValidClient(uint32_t Identity);

   uint32_t open(void);
   bool close(uint32_t Identity);

   // weh nstart with I2STYPE ,should call set and then start
   bool start(uint32_t Identity ,I2STYPE Type,uint32_t SampleRate =0);
   bool stop(uint32_t Identity ,I2STYPE Type);
   unsigned int read(uint32_t Identity,void* buffer, uint32_t buffersize);
   bool set(uint32_t TYPE, int32_t samplerate=0);

   // state of I2S
   bool StateInIdle(void);
   bool StateInI2S0InputStart(void);
   bool StateInI2S0OutputStart(void);
   bool StateInI2S1OutputStart(void);
   bool StateChecking(uint32_t state);

   I2SInputBuffer I2SRead_Buf;
   bool ReadThreadExit;
   bool ReadThreadAvtive;

private:

   AudioI2S();
   ~AudioI2S();
   AudioI2S(const AudioI2S &);             // intentionally undefined
   AudioI2S & operator=(const AudioI2S &); // intentionally undefined

   bool setEdiControl(uint32_t par, int32_t samplerate);
   bool startInput(uint32_t Identity,I2STYPE Type);
   bool stopInput(uint32_t Identity,I2STYPE Type);
   bool startI2S0Output(uint32_t SampleRate);
   bool stopI2S0Output(void);
   bool startI2S1Output(uint32_t SampleRate);
   bool stopI2S1Output(void);
   void SetI2SState(uint32_t state);
   void ClearI2SState(uint32_t state);

   int32_t checkIdValid(uint32_t Identity);
   bool CompareIdValid(uint32_t Identity);
   bool CheckCanStop(void);

   AudioAfe *mAfe;
   pthread_t mReadThread;
   uint32_t  mSamplerate;
   int32_t mSourceType;
   int32_t mState;

   // for mutiple access to i2s , need to protect when multiple access.
   pthread_mutex_t mI2SMutex;

   // fd to control asm
   int32_t mFd;
   int32_t EdiFormat;
   int32_t mClientCount;
   I2SClient mClient[I2S_MAX_USER];
   uint32_t mReg_Value;
   uint32_t mInterrupt_Timer;
   uint32_t I2S0OutputSampleRate;
   uint32_t I2S1OutputSampleRate;

   FILE *pOutFile;

};


}; // namespace android

#endif
