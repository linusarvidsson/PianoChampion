//
//  sfPlayer.hpp
//  PianoChamp
//
//  Created by Ludvig Kratzert on 2019-03-20.
//  Copyright © 2019 Ludvig Kratzert. All rights reserved.
//

#ifndef sfPlayer_hpp
#define sfPlayer_hpp

// MidiPlayer libraries
#include "minisdl_audio.h"
#define TSF_IMPLEMENTATION
#include "tsf.h"

class sfPlayer{
private:
    SDL_AudioSpec OutputAudioSpec;
    
public:
    sfPlayer(void (*callback)(void*, Uint8*, int), const char* filename);
    tsf* soundfont;
};

sfPlayer::sfPlayer(void (*callback)(void*, Uint8*, int), const char* filename) {
    OutputAudioSpec.freq = 44100;
    OutputAudioSpec.format = AUDIO_F32;
    OutputAudioSpec.channels = 2;
    OutputAudioSpec.samples = 512; // Den här var 4096; //ska vara 512 för att det ska funka på windows
    OutputAudioSpec.callback = callback;
    // Initialize the audio system
    SDL_AudioInit(TSF_NULL);
    // Ladda soundfont
    soundfont = tsf_load_filename(filename);
    // Set the SoundFont rendering output mode
    tsf_set_output(soundfont, TSF_STEREO_INTERLEAVED, OutputAudioSpec.freq, 0.0f);
    // Request the desired audio output format
    SDL_OpenAudio(&OutputAudioSpec, TSF_NULL);
    // Start the actual audio playback here
    // The audio thread will begin to call our AudioCallback function
    SDL_PauseAudio(0);
}

#endif /* sfPlayer_hpp */
