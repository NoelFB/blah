#include "blah_audio.h"

#define STB_VORBIS_HEADER_ONLY
#include "third_party/stb_vorbis.c"

#define CUTE_SOUND_IMPLEMENTATION
#define CUTE_SOUND_FORCE_SDL
#include "third_party/cute_sound.h"

namespace Blah
{
	namespace Internal
	{
		bool init(void* os_handle, unsigned play_frequency_in_Hz, int buffered_samples)
		{
			cs_error_t err = cs_init(os_handle, play_frequency_in_Hz, buffered_samples, NULL);
			if (err != CUTE_SOUND_ERROR_NONE) {
				Log::error(cs_error_as_string(err));
				return false;
			} else {
				return true;
			}
		}

		void shutdown()
		{
			cs_shutdown();
		}
	}

	Audio::Audio(void* audio)
	{
		m_ptr = audio;
	}

	Audio::~Audio()
	{
		cs_free_audio_source((cs_audio_source_t*)m_ptr);
	}

	u64 Audio::get_sample_count()
	{
		return m_ptr ? ((cs_audio_source_t*)m_ptr)->sample_count : 0;
	}

	AudioRef Audio::create(const FilePath& path)
	{
		FileStream fs(path, FileMode::OpenRead);
		if (!fs.is_readable())
			return AudioRef();

		if (path.ends_with(".ogg")) {
			return create_from_ogg(fs);
		} else if (path.ends_with(".wav")) {
			return create_from_wav(fs);
		}

		Log::error("Unable to load unrecognized audio file type.");
		return AudioRef();
	}

	AudioRef Audio::create_from_wav(Stream& stream)
	{
		if (!stream.is_readable())
		{
			Log::error("Unable to load audio as the Stream was not readable");
			return AudioRef();
		}

		// read into buffer
		Vector<u8> buffer;
		buffer.reserve((int)stream.length());
		stream.read(buffer.data(), stream.length());

		// load wav file from memory using cute_sound.h
		cs_error_t err;
		void* audio = cs_read_mem_wav((void*)buffer.data(), (size_t)buffer.size(), &err);
		if (!audio) {
			Log::error(cs_error_as_string(err));
			return AudioRef();
		}
		else
			return AudioRef(new Audio(audio));
	}

	AudioRef Audio::create_from_ogg(Stream& stream)
	{
		if (!stream.is_readable())
		{
			Log::error("Unable to load audio as the Stream was not readable");
			return AudioRef();
		}

		// read into buffer
		Vector<u8> buffer;
		buffer.reserve((int)stream.length());
		stream.read(buffer.data(), stream.length());

		// load ogg file from memory using cute_sound.h
		cs_error_t err;
		void* audio = cs_read_mem_ogg((void*)buffer.data(), (size_t)buffer.size(), &err);
		if (!audio) {
			Log::error(cs_error_as_string(err));
			return AudioRef();
		}
		else
			return AudioRef(new Audio(audio));
	}

	void set_global_pan(float pan)
	{
		cs_set_global_pan(pan);
	}

	void set_global_volume(float volume)
	{
		cs_set_global_volume(volume);
	}

	void set_global_pause(bool true_for_paused)
	{
		cs_set_global_pause(true_for_paused);
	}

	void set_sound_volume(float volume)
	{
		cs_set_playing_sounds_volume(volume);
	}

	void set_music_volume(float volume)
	{
		cs_music_set_volume(volume);
	}

	namespace Music
	{
		void play(AudioRef audio, float fade_in_time)
		{
			cs_music_play((cs_audio_source_t*)audio->get_backend_handle(), fade_in_time);
		}

		void stop(float fade_out_time)
		{
			cs_music_stop(fade_out_time);
		}

		void set_volume(float volume)
		{
			cs_music_set_volume(volume);
		}

		void set_loop(bool true_to_loop)
		{
			cs_music_set_loop(true_to_loop);
		}

		void pause()
		{
			cs_music_pause();
		}

		void resume()
		{
			cs_music_resume();
		}

		void switch_to(AudioRef audio, float fade_out_time, float fade_in_time)
		{
			cs_music_switch_to((cs_audio_source_t*)audio->get_backend_handle(), fade_out_time, fade_in_time);
		}

		void crossfade(AudioRef audio, float cross_fade_time)
		{
			cs_music_crossfade((cs_audio_source_t*)audio->get_backend_handle(), cross_fade_time);
		}

		u64 get_sample_index()
		{
			return cs_music_get_sample_index();
		}

		void set_sample_index(u64 sample_index)
		{
			cs_music_set_sample_index(sample_index);
		}
	}
	
	Sound Sound::play(AudioRef audio, SoundParams params)
	{
		cs_sound_params_t csparams;
		csparams.paused = params.paused;
		csparams.looped = params.looped;
		csparams.volume = params.volume;
		csparams.pan = params.pan;
		csparams.delay = params.delay;
		cs_playing_sound_t cssound = cs_play_sound((cs_audio_source_t*)audio->get_backend_handle(), csparams);
		Sound sound;
		sound.id = cssound.id;
		return sound;
	}

	bool Sound::is_active()
	{
		cs_playing_sound_t cssound = { id };
		return cs_sound_is_active(cssound);
	}

	bool Sound::is_paused()
	{
		cs_playing_sound_t cssound = { id };
		return cs_sound_get_is_paused(cssound);
	}

	bool Sound::is_looped()
	{
		cs_playing_sound_t cssound = { id };
		return cs_sound_get_is_looped(cssound);
	}

	float Sound::get_volume()
	{
		cs_playing_sound_t cssound = { id };
		return cs_sound_get_volume(cssound);
	}

	u64 Sound::get_sample_index()
	{
		cs_playing_sound_t cssound = { id };
		return cs_sound_get_sample_index(cssound);
	}

	void Sound::set_is_paused(bool true_for_paused)
	{
		cs_playing_sound_t cssound = { id };
		cs_sound_set_is_paused(cssound, true_for_paused);
	}

	void Sound::set_is_looped(bool true_for_looped)
	{
		cs_playing_sound_t cssound = { id };
		cs_sound_set_is_looped(cssound, true_for_looped);
	}

	void Sound::set_volume(float volume)
	{
		cs_playing_sound_t cssound = { id };
		cs_sound_set_volume(cssound, volume);
	}

	void Sound::set_sample_index(u64 sample_index)
	{
		cs_playing_sound_t cssound = { id };
		cs_sound_set_sample_index(cssound, sample_index);
	}
}

#undef STB_VORBIS_HEADER_ONLY
#include "third_party/stb_vorbis.c"
