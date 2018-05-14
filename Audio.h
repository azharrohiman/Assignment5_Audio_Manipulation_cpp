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
			Audio(int rate, int channels) : sample_rate(rate), num_channels(channels) { }

			Audio(std::vector<T> vec, int rate, int channels) : audio_data(data), sample_rate(rate), num_channels(channels) { }

			Audio(std::string filename, int rate, int channels) : sample_rate(rate), num_channels(channels) {
				// readfile method(filename)
			}

			Audio(const Audio& rhs) {
				sample_rate = rhs.sample_rate;
				num_channels = rhs.num_channels;
				audio_data = rhs.audio_data;
			}

			Audio& operator=(const Audio & rhs) {
				sample_rate = rhs.sample_rate;
				num_channels = rhs.num_channels;
				audio_data = rhs.audio_data;

				return *this;
			}

			Audio(Audio&& other) {
				sample_rate = rhs.sample_rate;
				num_channels = rhs.num_channels;
				audio_data = std::move(rhs.audio_data);
				rhs.audio_data.resize(0);
			}

			Audio& operator=(Audio&& rhs) {
				sample_rate = rhs.sample_rate;
				num_channels = rhs.num_channels
				audio_data = std::move(rhs.audio_data);
				rhs.audio_data.resize(0);

				return *this;
			}

			~Audio();

			Audio operator+(const Audio& rhs) const {

				Audio sample = rhs;
				Audio audio_sum = *this;

				for (int i = 0; i < audio_data.size(); ++i) {
					T val = audio_sum.data[i] + sample.data[i];

					if (val > std::numeric_limits<T>::max()) {
						val = std::numeric_limits<T>::max();
					}
					else if (val < std::numeric_limits<T>::max()) {
						val = std::numeric_limits<T>::max();
					}

					audio_sum.data[i] = val;
				}
				return audio_sum;
			}

			Audio operator^(const std::par<int, int> range) const {

				Audio audio_cut;

				for (int i = 0; i < audio_data.size(); ++i) {
					if (i < range.first) {
						audio_cut.audio_data.push_back(this->audio_data[i]);
					}
					else if (i > range.second) {
						audio_cut.audio_data.push_back(this->audio_data[i]);
					}
				}
				return audio_cut;
			}

			Audio rangeAdd(const Audio& rhs, const std::pair<int, int> range1, const std::pair<int, int> range2) {

				Audio sample_1;
				sample_1.audio_data.resize(range1.second - range1.first);

				Audio sample_2;
				sample_2.audio_data.resize(range2.second - range2.first);

				T start_1 = this->audio_data.begin() + range1.first;
				T end_1 = this->audio_data.begin() + range1.second;

				std::copy(start_1, end_1, sample_1.audio_data.begin());

				T start_2 = this->audio_data.begin() + range2.first;
				T end_2 = this->audio_data.begin() + range2.second;

				std::copy(start_2, end_2, sample_2.audio_data.begin());

				Audio audio_range_add = sample_1 + sample_2;

			}

			Audio<T> operator|(const Audio& rhs) const {
				Audio<T> audio_concatenation = *this;

				for (int i = 0; i < rhs.audio_data.size(); ++i) {
					audio_concatenation.audio_data.push_back(rhs.data[i]);
				}

				return audio_concatenation;
			}

			Audio operator*(const std::pair<float, float> factor) const;

			void reverseSound();

			float getRMS();

			Audio normalize(std::pair<float, float> rms) const;

			class NormalizeFunctor {

				public:
					float current_rms;
					float desired_rms;

					NormalizeFunctor (float current, float desired) : current_rms(current), desired_rms(desired) {

					}

					T operator() (T ampVal) {

						double rms_val = desired_rms/current_rms;
						double out_amp = rms_val * ampVal;

						if (out_amp > std::numeric_limits<T>::max()) {
							out_amp = std::numeric_limits<T>::max();
						}
						else if (out_amp < std::numeric_limits<T>::min()) {
							out_amp = std::numeric_limits<T>::mins();
						}

						return (T)(out_amp);

					}

			}

	}

}

#endif
