/*!
  Breakout
  @brief Simple breakout game
*/

#include "sound.hpp"
#include <gob_m5s_sd.hpp>
#include <gob_utility.hpp>
#include <gob_debug.hpp>

namespace
{
constexpr char resourceDir[] = "/res/bo2";
const char* bgmPath[] = { "bgm1.wav", "qq2_1.wav", "qr2_1.wav", "qr2_2.wav", };
const char* sfxPath[] = { "qq1_1.wav", "qq3_1.wav", };
//
}

LGFX* SoundSystem::_lcd = nullptr;
std::atomic_bool SoundSystem::_using_dma = ATOMIC_VAR_INIT(false);

bool SoundSystem::Sfx::read(const char* filename)
{
    char path[32];

    snprintf(path, sizeof(path), "%s/%s", resourceDir, filename);
    path[sizeof(path) - 1] = '\0';
    
    goblib::m5s::File f;
    f.open(path, O_READ);
    if(!f) { return false; }

    _length = f.available();
    if(!_length) { return false; }

    _buf.reset(new std::uint8_t[_length]);
    if(!_buf) { return false; }

    if(f.read(_buf.get(), _length) != _length)
    {
        _buf.release();
        return false;
    }

    return true;
}

SoundSystem::SoundSystem()
        : _taskHandle(nullptr)
        , _speaker(MAX_CHANNELS)
        , _bgmFile()
        , _bgmStream()
        , _sfxs()
        , _sfxChannel(1)
{}

// ATTENTION : To be called after releasing the DMA BUS!
void SoundSystem::setup()
{
    for(auto it = std::begin(sfxPath); it != std::end(sfxPath); ++it)
    {
        _sfxs.emplace_back(*it);
    }
    _speaker.begin();
    //    _speaker.setVolume(144);
    _speaker.setVolume(80);

    xTaskCreatePinnedToCore(accessTask, "access_task", 8192, this, uxTaskPriorityGet(nullptr), &_taskHandle, 1);
}

void SoundSystem::pump()
{

    _speaker.pump();


}

// ATTENTION : To be called after releasing the DMA BUS!
void SoundSystem::playBgm(BGM bgm)
{
    auto idx = goblib::to_underlying(bgm);
    if(idx >= goblib::size(bgmPath))
    {
        printf("Illegal bgm %u\n", idx);
        return;
    }

    char path[32];
    snprintf(path, sizeof(path), "%s/%s", resourceDir, bgmPath[idx]);
    path[sizeof(path) - 1] = '\0';

    _bgmFile.open(path);
    _bgmStream.assign(&_bgmFile);

    _speaker.play(_bgmStream, (bgm != BGM::Bgm1), _bgmChannel);
}

// ATTENTION : To be called after releasing the DMA BUS!
void SoundSystem::playSfx(SFX sfx)
{
    auto idx = goblib::to_underlying(sfx);
    if(idx >= _sfxs.size())
    {
        printf("Illegal sfx %u\n", idx);
        return;
    }
    _speaker.playWav(_sfxs[idx].buffer(), ~0U, 1, ++_sfxChannel, true);
    if(_sfxChannel >= MAX_CHANNELS) { _sfxChannel = 1; }
}

void SoundSystem::_accessTask()
{
    printf("%s\n", __PRETTY_FUNCTION__);
    for(;;)
    {
        //        printf("%s \n", __func__);
        //        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        while(_lcd->dmaBusy()) { delay(1); }
        /*
        _using_dma = true;
        _lcd->endWrite();
        _speaker.pump(); // access to SD card.
        _lcd->startWrite();
        _using_dma = false;
        */
    }
}
