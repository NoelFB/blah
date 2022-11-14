#pragma once

#include <blah_common.h>
#include <blah_stream.h>
#include <blah_filesystem.h>

namespace Blah
{
	class Audio;
	class Sound;
	using AudioRef = Ref<Audio>;

	class Audio
	{
	public:

		// Load audio from a .wav file or .ogg file.
		static AudioRef create(const FilePath& path);

		// Load audio from a .ogg Stream.
		static AudioRef create_from_ogg(Stream& stream);

		// Load audio from a .wav Stream.
		static AudioRef create_from_wav(Stream& stream);

		~Audio();
		inline void* get_backend_handle() { return m_ptr; }
		u64 get_sample_count();

	private:
		Audio(void* audio);
		void* m_ptr;
	};

	// Set the global panning control for left/right stereo, from 0.0f (left) to 1.0f (right).
	// 0.5f is the default for playing both speakers equal.
	void set_global_pan(float pan);

	// Set the global volume control for all audio, sounds + music included.
	void set_global_volume(float volume);

	// Set the global pause control for all audio, sounds + music included.
	void set_global_pause(bool true_for_paused);

	// Set the volume control for *just sounds*, not music.
	void set_sound_volume(float volume);

	// Set the volume control for *just music*, not sounds.
	void set_music_volume(float volume);

	namespace Music
	{
		// Play audio track using the music controls.
		// Can optionally fade the volume in over a number of seconds.
		void play(AudioRef audio, float fade_in_time = 0);

		// Stop any music playing.
		// Can optionally fade out the music volume over a number of seconds.
		void stop(float fade_out_time = 0);

		// Sets the volume for music.
		void set_volume(float volume);

		// Turns on or off whether or not to loop music.
		void set_loop(bool true_to_loop);

		// Pauses the music.
		void pause();

		// Resumes the music from a pause.
		void resume();

		// Switches to another audio track.
		// Can optionally fade out over a number of seconds.
		// Then can optinoally fade in the next track over a number of seconds.
		void switch_to(AudioRef audio, float fade_out_time = 0, float fade_in_time = 0);

		// Crossfades from the current music to another track over a number of seconds.
		void crossfade(AudioRef audio, float cross_fade_time = 0);

		// Get the index of the music sample currently playing.
		u64 get_sample_index();

		// Set the index for next sample of music to play.
		// You can use this to manually sync to a specific part in the audio.
		void set_sample_index(u64 sample_index);
	}

	struct SoundParams
	{
		bool paused = false;
		bool looped = false;
		float volume = 1.0f;
		float pan = 0.5f; // 0 means left, 1 means right, 0.5f means balanced.
		float delay = 0; // A number of seconds before playing the sound.
	};

	class Sound
	{
	public:

		// Play a sound, returns a new `Sound` instance.
		// Returns a sound instance. The instance will destroy itself internally when
		// it finishes playing, unless `looped` is set to true.
		// Once destroyed other functions like `set_volume` will be ignored.
		static Sound play(AudioRef audio, SoundParams params = SoundParams());

		// Tells whether or not this sound instance is still alive.
		bool is_active();

		// Returns if this sound instance is alive and paused.
		bool is_paused();

		// Returns if this sound instance is alive and looped.
		bool is_looped();

		// Gets the volume of the sound instance.
		float get_volume();

		// Gets the sample index currently playing.
		u64 get_sample_index();

		// Pauses the sound.
		void set_is_paused(bool true_for_paused);

		// Sets whether or not this sound will continually playing.
		void set_is_looped(bool true_for_looped);

		// Sets the volume for this sound.
		void set_volume(float volume);

		// Sets the sample index for this sound to play next.
		// You can use this to manually sync to a specific part in the audio.
		void set_sample_index(u64 sample_index);

		u64 id = 0;
	};

}
