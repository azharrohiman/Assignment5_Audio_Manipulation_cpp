#ifndef AUDIO_H
#define AUDIO_H

#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <memory>
#include <string>

namespace RHMMUH005 {

	template <typename T> class Audio {

		private:
			int num_channels;
			int sample_rate;
			
			std::vector<T> audio_data;

		public:
			Audio(int rate, int channels);
			Audio(std::vector<T> vec, int rate, int channels);
			Audio(std::string filename, int rate, int channels);

			Audio(const Audio& rhs);
			Audio& operator=(const Audio & rhs);

			Audio(Audio&& other);
			Audio& operator=(Audio&& rhs);

			~Audio();

			Audio operator+(const Audio& rhs) const;

			Audio operator^(const std::par<int, int> range) const;

			Audio rangeAdd(Audio first, const std::pair<int, int> range1, Audio second, const std::pair<int, int> range2);

			Audio<T> operator|(const Audio& rhs) const;

			Audio operator*(const std::pair<float, float> factor) const;

			void reverseSound();

			float getRMS();

			Audio normalize(std::pair<float, float> rms) const;

	}

}

#endif
