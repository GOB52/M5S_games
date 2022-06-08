/*!
  Breakout
  @brief Simple breakout game
*/
#ifndef BREAKOUT_SOUND_HPP
#define BREAKOUT_SOUND_HPP

#include <gob_singleton.hpp>
#include <gob_pcm_stream.hpp>
#include <gob_m5s_stream.hpp>
#include <gob_m5s_speaker.hpp>
#include <vector>
#include "definition.hpp"
#include <freertos/task.h> // TaskHandle_t
#include <freertos/semphr.h> // xSemaphoreHandle

#ifdef LGFX_USE_V1
#include <lgfx/v1_autodetect/common.hpp>
#else
#include <LovyanGFX.hpp>
#endif

#include <atomic>

class SoundSystem : public goblib::Singleton<SoundSystem>
{
  public:
    constexpr static size_t MAX_CHANNELS = 5; // for Speaker

    static LGFX* _lcd;
    static std::atomic_bool _using_dma;
    
    // Sfx resource
    class Sfx
    {
      public:
        Sfx(const char* filename = nullptr)
                : _buf()
                , _length(0)
        {
            if(filename && filename[0]) { read(filename); }
        }
        Sfx(Sfx&& o) : _buf(std::move(o._buf)), _length(o._length) {}

        ~Sfx(){}

        bool read(const char* filename);
        
        const std::uint8_t* buffer() const { return _buf.get(); }
        std::size_t length() const  { return _length; }
    
      private:
        std::unique_ptr<std::uint8_t[]> _buf;
        std::size_t _length;
    };
    
    // Resolve ambiguities.
    using PointerType = std::unique_ptr<SoundSystem>;
    using Singleton<SoundSystem>::instance;
    using Singleton<SoundSystem>::create;

    goblib::m5s::Speaker& speaker() { return _speaker; }
    
    void setup();
    void pump();

    void playSfx(SFX sfx);
    void playBgm(BGM bgm);

    static void accessTask(void* arg) { ((SoundSystem*)arg)->_accessTask(); }
    
  protected:
    friend class goblib::Singleton<SoundSystem>;
    SoundSystem();

  private:
    void _accessTask();
    
  private:
    TaskHandle_t _taskHandle;

    goblib::m5s::Speaker _speaker;

    goblib::m5s::FileStream _bgmFile;
    goblib::PcmStream _bgmStream;
    std::vector<Sfx> _sfxs;

    constexpr static int _bgmChannel = 0;
    int _sfxChannel;
};

#endif
