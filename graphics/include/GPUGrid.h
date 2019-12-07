#pragma once

#include <random>

namespace Compute
{
	struct Noise
	{
	private:
		uint octaves    = 1;
		float	lacunarity = 1.f; 
		float	gain       = 1.f;
		float	frequency  = 1.f;

		float fractalBounding;

		char m_perm[512], m_perm12[512];
		GL::Tex1D permTex, perm12Tex;
		GL::ProgRef program;
		IntVector3 workSize;

	public:
		Noise(uint seed = 42069)
		{
			program = GL::Programs.Load({ { "Noise.comp" } });
			glGetProgramiv(program->id, GL_COMPUTE_WORK_GROUP_SIZE, workSize.e);

			SetOctaves(octaves);
			SetLacunarity(lacunarity);
			SetGain(gain);
			SetFrequency(frequency);

			generateSeed(seed);
			generateSeedTexture();
		}

	private:
		void generateSeed(uint seed)
		{
			std::mt19937_64 gen(seed);

			for(int i = 0; i < 256; i++) { m_perm[i] = i; }

			for(int j = 0; j < 256; j++)
			{
				int rng = (int)(gen() % (256 - j));
				int k = rng + j;
				int l = m_perm[j];
				m_perm[j] = m_perm[j + 256] = m_perm[k];
				m_perm[k] = l;
				m_perm12[j] = m_perm12[j + 256] = m_perm[j] % 12;
			}
		}
		
		void generateSeedTexture()
		{
			permTex.Filter = GL::Nearest;
			perm12Tex.Filter = GL::Nearest;
			permTex.Setup(512, GL::R8UI, m_perm  );
			perm12Tex.Setup(512, GL::R8UI, m_perm12);
		}

		void bindSeedTexture()
		{
			glBindImageTexture(1, permTex.ID, 0, GL_FALSE, 0, GL_READ_ONLY, permTex.Format);
			glBindImageTexture(2, perm12Tex.ID, 0, GL_FALSE, 0, GL_READ_ONLY, perm12Tex.Format);
		}

	public:
		void GetNoise(const GL::Tex2DRef tex)
		{
			glActiveTexture(GL::Texture0);
			tex->Bind();
			bindSeedTexture();

			program->Use();
			glDispatchCompute(Math::NextPowerOfTwo(tex->Size.x / workSize.x), Math::NextPowerOfTwo(tex->Size.y / workSize.y), 1);
		}

		void WaitFinish()
		{ glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT); }

	private:
		void calculateFractialBounding()
		{
			float amp = _Gain;
			float ampFractal = 1.0f;
			for(int i = 1; i < _Octaves; i++)
			{
				ampFractal += amp;
				amp *= _Gain;
			}
			setFractalBounding(1.0f / ampFractal);
		}

		void setFractalBounding(float value)
		{
			fractalBounding = value;
			program->Use();
			program->Set("uFractalBounding", fractalBounding);
		}

	public:
		void SetOctaves(uint oct)
		{
			Octaves = oct;
			calculateFractialBounding();
			program->Set("uOctaves", Octaves);
		}

		void SetFrequency(float freq)
		{
			Frequency = freq;
			program->Use();
			program->Set("uFrequency", Frequency);
		}

		void SetLacunarity(float lac)
		{
			Lacunarity = lac;
			program->Use();
			program->Set("uLacunarity", Lacunarity);
		}

		void SetGain(float gain)
		{
			Gain = gain;
			calculateFractialBounding();
			program->Set("uGain", Gain);
		})
	};
}