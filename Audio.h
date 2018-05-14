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

			Audio(std::vector<T> vec, int rate, int channels) : audio_data(vec), sample_rate(rate), num_channels(channels) { }

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

				return audio_range_add;

			}

			Audio<T> operator|(const Audio& rhs) const {
				Audio<T> audio_concatenation = *this;

				for (int i = 0; i < rhs.audio_data.size(); ++i) {
					audio_concatenation.audio_data.push_back(rhs.data[i]);
				}

				return audio_concatenation;
			}

			Audio operator*(const std::pair<float, float> factor) const {

				for (int i = 0; i < audio_data.size(); ++i) {
					audio_data[i] *= factor.first;
				}

				return *this;
			}

			void reverseSound() {
				std::reverse(audio_data.begin(), audio_data.end());
			}

			float getRMS() {
				return std::sqrt( (1/(float)audio_data.size()) * std::accumulate(audio_data.begin(), audio_data.end(), 0, [] (T sum, T val) {
					return (sum + pow(val, 2));
				}));
			}

			void loadFile(std::string filename) {

				std::ifstream file(filename, std::ios::binary | std::ios::in);

				if (!file.is_open()) {
					std::cout << "File not loaded properly " << filename << std::endl;
				}

				else {
					file.seekg(0, file.end);

					int file_length = file.tellg();
					file.seekg(0, file.beg);

					int sample = (int) (file_length / (sizeof(T) * num_channels));

					audio_data.resize(sample);

					for (int i = 0; i < sample; ++i) {
						char buffer[sizeof(T)];
						file.read(buffer, sizeof(T));
						audio_data[i] = *(T *)(buffer);
					}

					file.close();
				}
			}

			void saveFile(std::string filename) {

				std::string sample_rate_string = std::to_string(sample_rate);
				std:: string bit_sample_string = std::to_string(8 * sizeof(T));

				std::string output_file = filename + "_" + sample_rate_string + "_" + bit_sample_string + "_mono.raw";

				std::ofstream out(output_file, std::ios::out | std::ios::binary);

				if (!out.is_open()) {
					std::cout << "Couldn't write contents to file " << output_file << std::endl;
				}

				else {
					out.write((char *) &audio_data[0], audio_data.size() * sizeof(T));
				}

				out.close();
			}

			Audio normalize(std::pair<float, float> rms) const {
				NormalizeFunctor op(rms.first);
				Audio audio_normalized = *this;

				std::transform(audio_data.begin(), audio_data.end(), audio_normalized.audio_data.begin(), op);
				return op;
			}

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
	};


	template<typename T> class Audio <std::pair<T, T>> {

		private:
			int num_channels;
			int sample_rate;
			
			std::vector<std::pair<T, T>> audio_data;

		public:

			Audio(int rate, int channels) : sample_rate(rate), num_channels(channels) { }

			Audio(std::vector<std::pair<T, T>> vec, int rate, int channels) : audio_data(vec), sample_rate(rate), num_channels(channels) { }

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

			~Audio() { }

			Audio operator+(const Audio& rhs) const {

				Audio sample = rhs;
				Audio audio_sum = *this;

				for (int i = 0; i < audio_data.size(); ++i) {
					double r_val = audio_sum.data[i].second + sample.data[i].second;
					double l_val = audio_sum.data[i].first + sample.data[i].first;

					if (r_val > std::numeric_limits<T>::max()) {
						r_val = std::numeric_limits<T>::max();
					}
					else if (r_val < std::numeric_limits<T>::max()) {
						r_val = std::numeric_limits<T>::max();
					}

					audio_sum.data[i].second = (T) r_val;

					if (l_val > std::numeric_limits<T>::max()) {
						l_val = std::numeric_limits<T>::max();
					}
					else if (l_val < std::numeric_limits<T>::max()) {
						l_val = std::numeric_limits<T>::max();
					}

					audio_sum.data[i].first = (T) l_val;
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

				return audio_range_add;
			}

			Audio operator|(const Audio& rhs) const {
				Audio audio_concatenation = *this;

				for (int i = 0; i < rhs.audio_data.size(); ++i) {
					audio_concatenation.audio_data.push_back(rhs.data[i]);
				}

				return audio_concatenation;
			}

			Audio operator*(const std::pair<float, float> factor) const {

				for (int i = 0; i < audio_data.size(); ++i) {
					audio_data[i].first *= factor.first;
					audio_data[i].second *= factor.second;
				}

				return *this;
			}

			void reverseSound() {
				std::reverse(audio_data.begin(), audio_data.end());
			}

			std::pair<float, float> getRMS() {

				std::pair<float, float> rms;
			
				float left = std::sqrt(std::accumulate(this->audio_data.begin(), this->audio_data.end(); 0.0; [&](float sum, std::pair<T, T> val) {
					retun sum + pow(val.first, 2)/this->audio_data.size();
				}));

				float right = std::sqrt(std::accumulate(this->audio_data.begin(), this->audio_data.end(); 0.0; [&](float sum, std::pair<T, T> val) {
					retun sum + pow(val.second, 2)/this->audio_data.size();
				}));

				rms = std::make_pair(left, right);
				return rms;
			}

			void loadFile(std::string filename) {

				std::ifstream file(filename, std::ios::binary | std::ios::in);

				if (!file.is_open()) {
					std::cout << "File not loaded properly " << filename << std::endl;
				}

				else {
					file.seekg(0, file.end);

					int file_length = file.tellg();
					file.seekg(0, file.beg);

					int sample = (int) (file_length / (sizeof(T) * num_channels));

					audio_data.resize(sample);

					for (int i = 0; i < sample; ++i) {
						char right_buffer[sizeof(T)];
						char left_buffer[sizeof(T)];

						file.read(right_buffer, sizeof(T));
						audio_data[i].second = *(T *)(right_buffer);

						file.read(left_buffer, sizeof(T));
						audio_data[i].first = *(T *)(left_buffer);
					}

					file.close();
				}
			}

			void saveFile(std::string filename) {

				std::string sample_rate_string = std::to_string(sample_rate);
				std:: string bit_sample_string = std::to_string(8 * sizeof(T));

				std::string output_file = filename + "_" + sample_rate_string + "_" + bit_sample_string + "_stereo.raw";

				std::ofstream out(output_file, std::ios::out | std::ios::binary);

				if (!out.is_open()) {
					std::cout << "Couldn't write contents to file " << output_file << std::endl;
				}

				else {
					for (auto i : audio_data) {
						out.write((char *)&audio_data.first, sizeof(T));
						out.write((char *)&audio_data.second, sizeof(T));
					}
				}

				out.close();
			}

			Audio normalize(std::pair<float, float> rms) const {
				NormalizeFunctor op(rms);
				Audio audio_normalized = *this;

				std::transform(audio_data.begin(), audio_data.end(), audio_normalized.audio_data.begin(), op);
				return op;
			}

			class NormalizeFunctor {

				public:
					std::pair<float, float> current_rms;
					std::pair<float, float> desired_rms;

					NormalizeFunctor (std::pair<float, float> current, std::pair<float, float> desired) : current_rms(current), desired_rms(desired) {

					}

					std::pair<T, T> operator() (std::pair<T, T> ampVal) {

						return ((T)(ampVal.first * (desired_rms.first / current_rms.first)), (T)(ampVal.second * (desired_rms.second / current_rms.second)));
					}
			};
	}

}

#endif
