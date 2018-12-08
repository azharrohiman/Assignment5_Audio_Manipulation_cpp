#ifndef AUDIO_H
#define AUDIO_H

#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <memory>
#include <string>
#include <limits>
#include <algorithm>
#include <cmath>
#include <numeric>

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
				loadFile(filename);
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

			Audio(Audio&& rhs) {
				sample_rate = rhs.sample_rate;
				num_channels = rhs.num_channels;
				audio_data = std::move(rhs.audio_data);
				rhs.audio_data.resize(0);
			}

			Audio& operator=(Audio&& rhs) {
				sample_rate = rhs.sample_rate;
				num_channels = rhs.num_channels;
				audio_data = std::move(rhs.audio_data);
				rhs.audio_data.resize(0);

				return *this;
			}

			~Audio() { }

			Audio operator+(const Audio& rhs) const {

				Audio sample = rhs;
				Audio audio_sum = *this;

				for (int i = 0; i < audio_data.size(); ++i) {
					T val = audio_sum.audio_data[i] + sample.audio_data[i];

					if (val > std::numeric_limits<T>::max()) {
						val = std::numeric_limits<T>::max();
					}
					else if (val < std::numeric_limits<T>::max()) {
						val = std::numeric_limits<T>::max();
					}

					audio_sum.audio_data[i] = val;
				}
				return audio_sum;
			}

			Audio operator^(const std::pair<int, int> range) const {

				Audio audio_cut = *this;

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

			Audio<T> operator|(const Audio& rhs) const {
				Audio<T> audio_concatenation = *this;

				for (int i = 0; i < rhs.audio_data.size(); ++i) {
					audio_concatenation.audio_data.push_back(rhs.audio_data[i]);
				}

				return audio_concatenation;
			}

			Audio operator*(const std::pair<float, float> factor) const {

				Audio val = *this;
				for (int i = 0; i < audio_data.size(); ++i) {
					val.audio_data[i] *= factor.first;
				}

				return *this;
			}

			Audio reverseSound() {
				Audio rev = *this;
				std::reverse(rev.audio_data.begin(), rev.audio_data.end());
				return rev;
			}

			std::pair<float, float> getRMS() {
				float x = 0;
				int val = 0;

				x = std::accumulate(audio_data.begin(), audio_data.end(), x, [&val](float x, T y) {
					val++;
					return x + (y * y);
				});

				float sqrt_val = std::sqrt(x/val);

				std::pair<float, float> final_rms(sqrt_val, 0.0f);
				return final_rms;
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
				Audio op = *this;
				float current_rms = op.getRMS().first;
				op.audio_data.resize(0);

				std::transform(audio_data.begin(), audio_data.end(), std::back_inserter(op.audio_data), NormalizeFunctor(rms.first, current_rms));
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
							out_amp = std::numeric_limits<T>::min();
						}

						return (T)(out_amp);
					}
			};
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
				loadFile(filename);
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

			Audio(Audio&& rhs) {
				sample_rate = rhs.sample_rate;
				num_channels = rhs.num_channels;
				audio_data = std::move(rhs.audio_data);
				rhs.audio_data.resize(0);
			}

			Audio& operator=(Audio&& rhs) {
				sample_rate = rhs.sample_rate;
				num_channels = rhs.num_channels;
				audio_data = std::move(rhs.audio_data);
				rhs.audio_data.resize(0);

				return *this;
			}

			~Audio() { }

			Audio operator+(const Audio& rhs) const {

				Audio sample = rhs;
				Audio audio_sum = *this;

				for (int i = 0; i < audio_data.size(); ++i) {
					double r_val = audio_sum.audio_data[i].second + sample.audio_data[i].second;
					double l_val = audio_sum.audio_data[i].first + sample.audio_data[i].first;

					if (r_val > std::numeric_limits<T>::max()) {
						r_val = std::numeric_limits<T>::max();
					}
					else if (r_val < std::numeric_limits<T>::max()) {
						r_val = std::numeric_limits<T>::max();
					}

					audio_sum.audio_data[i].second = (T) r_val;

					if (l_val > std::numeric_limits<T>::max()) {
						l_val = std::numeric_limits<T>::max();
					}
					else if (l_val < std::numeric_limits<T>::max()) {
						l_val = std::numeric_limits<T>::max();
					}

					audio_sum.audio_data[i].first = (T) l_val;
				}

				return audio_sum;
			}

			Audio operator^(const std::pair<int, int> range) const {

				Audio audio_cut = *this;

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

			Audio operator|(const Audio& rhs) const {
				Audio audio_concatenation = *this;

				for (int i = 0; i < rhs.audio_data.size(); ++i) {
					audio_concatenation.audio_data.push_back(rhs.audio_data[i]);
				}

				return audio_concatenation;
			}

			Audio operator*(const std::pair<float, float> factor) const {

				Audio val = *this;
				for (int i = 0; i < audio_data.size(); ++i) {
					val.audio_data[i].first = val.audio_data[i].first * factor.first;
					val.audio_data[i].second = val.audio_data[i].second * factor.second;
				}

				return val;
			}

			Audio reverseSound() {
				Audio rev = *this;
				std::reverse(rev.audio_data.begin(), rev.audio_data.end());
				return rev;
			}

			std::pair<float, float> getRMS() {

				std::pair<float, float> rms;
			
				float left = std::sqrt(std::accumulate(this->audio_data.begin(), this->audio_data.end(), 0.0, [&](float sum, std::pair<T, T> val) {
					return sum + pow(val.first, 2)/this->audio_data.size();
				}));

				float right = std::sqrt(std::accumulate(this->audio_data.begin(), this->audio_data.end(), 0.0, [&](float sum, std::pair<T, T> val) {
					return sum + pow(val.second, 2)/this->audio_data.size();
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
						out.write((char *)&i.first, sizeof(T));
						out.write((char *)&i.second, sizeof(T));
					}
				}

				out.close();
			}

			Audio normalize(std::pair<float, float> rms) const {
				Audio op = *this;
				std::pair<float, float> current_rms = op.getRMS();
				op.audio_data.resize(0);

				std::transform(audio_data.begin(), audio_data.end(), std::back_inserter(op.audio_data), NormalizeFunctor(rms, current_rms));
				return op;
			}

			class NormalizeFunctor {

				public:
					std::pair<double, double> rms;
					std::pair<float, float> current_rms;
					std::pair<float, float> desired_rms;

					NormalizeFunctor (std::pair<float, float> current, std::pair<float, float> desired) {
						float left_rms = desired_rms.first/current_rms.first;
						float right_rms = desired_rms.second/current_rms.second;
						rms = std::pair<float, float>(left_rms, right_rms);
					}

					std::pair<T, T> operator() (std::pair<T, T> ampVal) {

						double amp_r = ampVal.second * rms.second;

						if (amp_r > std::numeric_limits<T>::max()) {
							amp_r = std::numeric_limits<T>::max();
						}

						else if (amp_r < std::numeric_limits<T>::min()) {
							amp_r = std::numeric_limits<T>::min();
						}

						double amp_l = ampVal.first * rms.first;

						if (amp_l > std::numeric_limits<T>::max()) {
							amp_l = std::numeric_limits<T>::max();
						}

						else if (amp_l < std::numeric_limits<T>::min()) {
							amp_l = std::numeric_limits<T>::min();
						}

						return std::pair<T, T>((T)amp_l, (T)amp_r);
					}
			};
	};

}

#endif
