#pragma once
#include <blah_common.h>
#include <blah_calc.h>

namespace Blah
{
	using Easer = float (*)(float);

	enum class Easers
	{
		Linear,
		QuadIn,    QuadOut,    QuadInOut,
		CubeIn,    CubeOut,    CubeInOut,
		QuartIn,   QuartOut,   QuartInOut,
		QuintIn,   QuintOut,   QuintInOut,
		SineIn,    SineOut,    SineInOut,
		CircIn,    CircOut,    CircInOut,
		ExpIn,     ExpOut,     ExpInOut,
		ElasticIn, ElasticOut, ElasticInOut,
		BackIn,    BackOut,    BackInOut,
		BounceIn,  BounceOut,  BounceInOut,
		Count
	};

	namespace Ease
	{
		/*
			Adapted from functions here: https://github.com/warrenm/AHEasing/blob/master/AHEasing/easing.c
			For previews go here: https://easings.net/
		*/

		constexpr float linear(float t)
		{
			return t;
		}

		constexpr float quad_in(float t)
		{
			return t * t;
		}

		constexpr float quad_out(float t)
		{
			return -(t * (t - 2));
		}

		constexpr float quad_in_out(float t)
		{
			if (t < 0.5f)
				return 2 * t * t;
			else
				return (-2 * t * t) + (4 * t) - 1;
		}

		constexpr float cube_in(float t)
		{
			return t * t * t;
		}

		constexpr float cube_out(float t)
		{
			float f = (t - 1);
			return f * f * f + 1;
		}

		constexpr float cube_in_out(float t)
		{
			if (t < 0.5f)
				return 4 * t * t * t;
			else
			{
				float f = ((2 * t) - 2);
				return 0.5f * f * f * f + 1;
			}
		}

		constexpr float quart_in(float t)
		{
			return t * t * t * t;
		}

		constexpr float quart_out(float t)
		{
			float f = (t - 1);
			return f * f * f * (1 - t) + 1;
		}

		constexpr float quart_in_out(float t)
		{
			if (t < 0.5f)
				return 8 * t * t * t * t;
			else
			{
				float f = (t - 1);
				return -8 * f * f * f * f + 1;
			}
		}

		constexpr float quint_in(float t)
		{
			return t * t * t * t * t;
		}

		constexpr float quint_out(float t)
		{
			float f = (t - 1);
			return f * f * f * f * f + 1;
		}

		constexpr float quint_in_out(float t)
		{
			if (t < 0.5f)
				return 16 * t * t * t * t * t;
			else
			{
				float f = ((2 * t) - 2);
				return  0.5f * f * f * f * f * f + 1;
			}
		}

		inline float sine_in(float t)
		{
			return Calc::sin((t - 1) * Calc::PI * 0.5f) + 1;
		}

		inline float sine_out(float t)
		{
			return Calc::sin(t * (Calc::PI * 0.5f));
		}

		inline float sine_in_out(float t)
		{
			return 0.5f * (1 - Calc::cos(t * Calc::PI));
		}

		inline float circ_in(float t)
		{
			return 1 - Calc::sqrt(1 - (t * t));
		}

		inline float circ_out(float t)
		{
			return Calc::sqrt((2 - t) * t);
		}

		inline float circ_in_out(float t)
		{
			if (t < 0.5f)
				return 0.5f * (1 - Calc::sqrt(1 - 4 * (t * t)));
			else
				return 0.5f * (Calc::sqrt(-((2 * t) - 3) * ((2 * t) - 1)) + 1);
		}

		inline float exp_in(float t)
		{
			return (t == 0) ? 0 : Calc::pow(2, 10 * (t - 1));
		}

		inline float exp_out(float t)
		{
			return (t == 1) ? 1 : 1 - Calc::pow(2, -10 * t);
		}

		inline float exp_in_out(float t)
		{
			if (t == 0 || t == 1)
				return t;

			if (t < 0.5f)
				return 0.5f * Calc::pow(2, (20 * t) - 10);
			else
				return -0.5f * Calc::pow(2, (-20 * t) + 10) + 1;
		}

		inline float elastic_in(float t)
		{
			return Calc::sin(13 * (Calc::PI * 0.5f) * t) * Calc::pow(2, 10 * (t - 1));
		}

		inline float elastic_out(float t)
		{
			return Calc::sin(-13 * (Calc::PI * 0.5f) * (t + 1)) * Calc::pow(2, -10 * t) + 1;
		}

		inline float elastic_in_out(float t)
		{
			if (t < 0.5f)
				return 0.5f * Calc::sin(13 * (Calc::PI * 0.5f) * (2 * t)) * Calc::pow(2, 10 * ((2 * t) - 1));
			else
				return 0.5f * (Calc::sin(-13 * (Calc::PI * 0.5f) * ((2 * t - 1) + 1)) * Calc::pow(2, -10 * (2 * t - 1)) + 2);
		}

		inline float back_in(float t)
		{
			return t * t * t - t * Calc::sin(t * Calc::PI);
		}

		inline float back_out(float t)
		{
			float f = (1 - t);
			return 1 - (f * f * f - f * Calc::sin(f * Calc::PI));
		}

		inline float back_in_out(float t)
		{
			if (t < 0.5f)
			{
				float f = 2 * t;
				return 0.5f * (f * f * f - f * Calc::sin(f * Calc::PI));
			}
			else
			{
				float f = (1 - (2 * t - 1));
				return 0.5f * (1 - (f * f * f - f * Calc::sin(f * Calc::PI))) + 0.5f;
			}
		}

		constexpr float bounce_out(float t)
		{
			if (t < 4 / 11.0f)
				return (121 * t * t) / 16.0f;
			else if (t < 8 / 11.0f)
				return (363 / 40.0f * t * t) - (99 / 10.0f * t) + 17 / 5.0f;
			else if (t < 9 / 10.0f)
				return (4356 / 361.0f * t * t) - (35442 / 1805.0f * t) + 16061 / 1805.0f;
			else
				return (54 / 5.0f * t * t) - (513 / 25.0f * t) + 268 / 25.0f;
		}

		constexpr float bounce_in(float t)
		{
			return 1 - bounce_out(1 - t);
		}

		constexpr float bounce_in_out(float t)
		{
			if (t < 0.5f)
				return 0.5f * bounce_in(t * 2);
			else
				return 0.5f * bounce_out(t * 2 - 1) + 0.5f;
		}

		inline Easer get(Easers e)
		{
			switch (e)
			{
			case Easers::Linear: return &linear;

			case Easers::CubeIn: return &cube_in;
			case Easers::CubeOut: return &cube_out;
			case Easers::CubeInOut: return &cube_in_out;

			case Easers::QuadIn: return &quad_in;
			case Easers::QuadOut: return &quad_out;
			case Easers::QuadInOut: return &quad_in_out;

			case Easers::QuartIn: return &quart_in;
			case Easers::QuartOut: return &quart_out;
			case Easers::QuartInOut: return &quart_in_out;

			case Easers::QuintIn: return &quint_in;
			case Easers::QuintOut: return &quint_out;
			case Easers::QuintInOut: return &quint_in_out;

			case Easers::SineIn: return &sine_in;
			case Easers::SineOut: return &sine_out;
			case Easers::SineInOut: return &sine_in_out;

			case Easers::CircIn: return &circ_in;
			case Easers::CircOut: return &circ_out;
			case Easers::CircInOut: return &circ_in_out;

			case Easers::ExpIn: return &exp_in;
			case Easers::ExpOut: return &exp_out;
			case Easers::ExpInOut: return &exp_in_out;

			case Easers::ElasticIn: return &elastic_in;
			case Easers::ElasticOut: return &elastic_out;
			case Easers::ElasticInOut: return &elastic_in_out;

			case Easers::BackIn: return &back_in;
			case Easers::BackOut: return &back_out;
			case Easers::BackInOut: return &back_in_out;

			case Easers::BounceIn: return &bounce_in;
			case Easers::BounceOut: return &bounce_out;
			case Easers::BounceInOut: return &bounce_in_out;
			case Easers::Count:
				break;
			}

			BLAH_ASSERT(false, "Invalid Easer Type");
			return nullptr;
		}

		inline const char* name(Easers e)
		{
			switch (e)
			{
			case Easers::Linear: return "Linear";
			case Easers::CubeIn: return "CubeIn";
			case Easers::CubeOut: return "CubeOut";
			case Easers::CubeInOut: return "CubeInOut";
			case Easers::QuadIn: return "QuadIn";
			case Easers::QuadOut: return "QuadOut";
			case Easers::QuadInOut: return "QuadInOut";
			case Easers::QuartIn: return "QuartIn";
			case Easers::QuartOut: return "QuartOut";
			case Easers::QuartInOut: return "QuartInOut";
			case Easers::QuintIn: return "QuintIn";
			case Easers::QuintOut: return "QuintOut";
			case Easers::QuintInOut: return "QuintInOut";
			case Easers::SineIn: return "SineIn";
			case Easers::SineOut: return "SineOut";
			case Easers::SineInOut: return "SineInOut";
			case Easers::CircIn: return "CircIn";
			case Easers::CircOut: return "CircOut";
			case Easers::CircInOut: return "CircInOut";
			case Easers::ExpIn: return "ExpIn";
			case Easers::ExpOut: return "ExpOut";
			case Easers::ExpInOut: return "ExpInOut";
			case Easers::ElasticIn: return "ElasticIn";
			case Easers::ElasticOut: return "ElasticOut";
			case Easers::ElasticInOut: return "ElasticInOut";
			case Easers::BackIn: return "BackIn";
			case Easers::BackOut: return "BackOut";
			case Easers::BackInOut: return "BackInOut";
			case Easers::BounceIn: return "BounceIn";
			case Easers::BounceOut: return "BounceOut";
			case Easers::BounceInOut: return "BounceInOut";
			case Easers::Count:
				break;
			}

			return "<Invalid>";
		}
	}
}
