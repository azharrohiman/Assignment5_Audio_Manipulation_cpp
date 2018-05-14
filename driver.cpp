#include <string>
#include <cstdint>
#include <iostream>

#include "Audio.h"

template <typename T> int bitOperations(int argc, char* argv[], int sample_rate, int num_channels, std::string filename) {

	//if (std::string(argv[7]) != "-o") {
		
	//}
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
	}

	return 0;
}
