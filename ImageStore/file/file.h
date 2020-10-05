#pragma once
#include <vector>
#include <string>

class File
{
private:
	std::vector<uint8_t> data;

	uint8_t getBit(size_t byte, uint8_t bitnum);

public:
	void readPNG(std::string image);
	void readFile(std::string file);

	void exportPNG(std::string originalimage, std::string newimage);
	void exportFile(std::string file);
};

