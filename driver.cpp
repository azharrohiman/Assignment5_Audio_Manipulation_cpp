#include <string>
#include <cstdint>
#include <iostream>

#include "Audio.h"

template <typename T> int bitOperations(int argc, char* argv[], int sample_rate, int num_channels, std::string filename) {

	if (std::string(argv[9]) == "-rms" || std::string(argv[9]) == "-rev") {
		if (argc < 11) return 1;

		RHMMUH005::Audio<T> sample(argv[10], sample_rate, num_channels);

		if (std::string(argv[9]) == "-rms") {
			std::pair<float, float> rms = sample.getRMS();

			if (num_channels == 2) {
				std::cout << "Left RMS: " << rms.first << std::endl;
				std::cout << "Right RMS: " << rms.second << std::endl;
			}
			else {
				std::cout << "RMS: " << rms.first << std::endl;
			}
		}

		else if (std::string(argv[9]) == "-rev") {
			RHMMUH005::Audio<T> reverse_audio = sample.reverseSound();
			std::cout << "Audio reversed" << std::endl;
		}
		return 0;
	}

	else if (std::string(argv[9]) == "-cat" || std::string(argv[9]) == "-add") {
		if (argc < 12) return 1;

		RHMMUH005::Audio<T> sample_1(argv[10], sample_rate, num_channels);
		RHMMUH005::Audio<T> sample_2(argv[11], sample_rate, num_channels);

		if (std::string(argv[9]) == "-cat") {
			RHMMUH005::Audio<T> audio_concatenation = sample_1 | sample_2;
			std::cout << argv[10] << " and " << argv[11] << " concatenated" << std::endl;
			audio_concatenation.saveFile(filename);
		}

		else if (std::string(argv[9]) == "-add") {
			RHMMUH005::Audio<T> audio_sum = sample_1 + sample_2;
			std::cout << argv[10] << " and " << argv[11] << " added" << std::endl;
			audio_sum.saveFile(filename);
		}
		return 0;
	}

	else if (std::string(argv[9]) == "-norm" || std::string(argv[9]) == "-cut" || std::string(argv[9]) == "-v") {
		if (argc < 13) return 1;

		RHMMUH005::Audio<T> sample(argv[12], sample_rate, num_channels);
		int range1 = std::stoi(argv[10]);
		int range2 = std::stoi(argv[11]);

		if (std::string(argv[9]) == "-norm") {
			RHMMUH005::Audio<T> audio_normalized = sample.normalize(std::pair<float, float>(range1, range2));
			audio_normalized.saveFile(filename);
			std::cout << "Audio normalized" << std::endl;
		}

		else if (std::string(argv[9]) == "-cut") {
			RHMMUH005::Audio<T> audio_cut = sample ^ std::pair<int, int>(range1, range2);
			audio_cut.saveFile(filename);
			std::cout << "Audio cut" << std::endl;
		}

		else if (std::string(argv[9]) == "-v") {
			RHMMUH005::Audio<T> audio_factor = sample * std::pair<float, float>(range1, range2);
			audio_factor.saveFile(filename);
			std::cout << "Audio volume factored" << std::endl;
		}

		return 0;
	}

	else {
		return -1;
	}
}

int main(int argc, char* argv[]) {

	using namespace std;

	if (argc < 8) {
		cout << "Not enough arguments" << endl;
		return 1;
	}

	else {
		int sample_rate = std::stoi(argv[2]);
		int bit_number = std::stoi(argv[4]);
		int num_channels = std::stoi(argv[6]);

		string filename = "out";

		if (string(argv[7]) == "-o") {
			filename = string(argv[8]);
		}

		int val = 0;

		switch (num_channels) {
			case 1:
				switch (bit_number) {
					case 8:
						val = bitOperations<int8_t>(argc, argv, sample_rate, num_channels, filename);
						break;
					case 16:
						val = bitOperations<int16_t>(argc, argv, sample_rate, num_channels, filename);
						break;
				}
				break;

			case 2:
				switch (bit_number) {
					case 8:
						val = bitOperations<std::pair<int8_t, int8_t>>(argc, argv, sample_rate, num_channels, filename);
						break;
					case 16:
						val = bitOperations<std::pair<int16_t, int16_t>>(argc, argv, sample_rate, num_channels, filename);
						break;
				}
				break;
		}

		if (val !=0 ) {
			std::cout << "Arguments incorrectly given" << std::endl;
			return 1;
		}
	}

	return 0;
}
