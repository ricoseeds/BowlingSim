### Bowling Simulation
Assignment 2 on on computer graphics where we have to render scene and implement basic collision detection and possibly physics.


###### Run in Mac
```
$ g++ src/main.cpp src/ShaderProgram.cpp src/Texture2D.cpp src/Camera.cpp src/Mesh.cpp  -framework OpenGl -I/usr/local/include -lglfw -lGLEW 

$ ./a.out
```

###### Run in Linux
```
g++ -std=c++11 src/main.cpp src/ShaderProgram.cpp src/Texture2D.cpp src/Camera.cpp src/Mesh.cpp -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -lGLEW
```
###### Sample output

![screenshot 2019-02-27 at 4 16 47 am](https://user-images.githubusercontent.com/17334660/53452200-a7de5b80-3a46-11e9-9fb2-282d4d70788f.png)

###### LINKS
https://github.com/FakeTruth/SkeletalAnimation (immediate mode)

https://github.com/RagnarrIvarssen/Assimp-Tutorial-LWJGL-3

##### SDL flags


#include <SDL2/SDL.h>
#include <iostream>
#define MUS_PATH "background.wav"

// prototype for our audio callback
// see the implementation for more information
void my_audio_callback(void *userdata, Uint8 *stream, int len);

// variable declarations
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play

/*
** PLAYING A SOUND IS MUCH MORE COMPLICATED THAN IT SHOULD BE
*/
int main(int argc, char *argv[])
{

    // Initialize SDL.
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        return 1;

    // local variables
    static Uint32 wav_length;      // length of our sample
    static Uint8 *wav_buffer;      // buffer containing our audio file
    static SDL_AudioSpec wav_spec; // the specs of our piece of music

    /* Load the WAV */
    // the specs, length and buffer of our wav are filled
    if (SDL_LoadWAV(MUS_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL)
    {
        return 1;
    }
    // set the callback function
    wav_spec.callback = my_audio_callback;
    wav_spec.userdata = NULL;
    // set our global static variables
    audio_pos = wav_buffer; // copy sound buffer
    audio_len = wav_length; // copy file length

    /* Open the audio device */
    if (SDL_OpenAudio(&wav_spec, NULL) < 0)
    {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    /* Start playing */

    // SDL_PauseAudio(1000);
    while (1)
    {
        wav_spec.callback = my_audio_callback;
        wav_spec.userdata = NULL;
        // set our global static variables
        audio_pos = wav_buffer; // copy sound buffer
        audio_len = wav_length; // copy file length

        SDL_PauseAudio(0);
        while (audio_len > 0)
        {
            SDL_Delay(10000);
        }
        // SDL_CloseAudio();
        // SDL_FreeWAV(wav_buffer);
    }

    // SDL_PauseAudio(0);

    // wait until we're don't playing
    while (audio_len > 0)
    {
        SDL_Delay(10000);
    }

    // shut everything down
    SDL_CloseAudio();
    SDL_FreeWAV(wav_buffer);
}

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
    // std::cout << len << std::endl;

    if (audio_len == 0)
        return;

    len = (len > audio_len ? audio_len : len);
    //SDL_memcpy (stream, audio_pos, len);
    // simply copy from one buffer into the other
    // std::cout << audio_pos;
    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME); // mix from one buffer into another

    audio_pos += len;
    audio_len -= len;
    // std::cout << audio_pos;
    // std::cout << audio_len;
}

//g++ 21_sound_effects_and_music.cpp  -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2