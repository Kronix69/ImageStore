#include "file.h"
#include <png++/png.hpp>
#include <fstream>
#include <iterator>
#include <iostream>
#include <math.h>

uint8_t getByteFromInt(uint32_t integer, uint8_t byte)
{
	if (byte > 3)
		return 0;

	return (integer >> (byte * 8)) & 0xFF;
}

uint8_t getBitFromByte(uint8_t Byte, uint8_t bit)
{
	if (bit > 7)
		return 0;

	return (Byte >> (bit)) & 1;
}

uint8_t File::getBit(size_t byte, uint8_t bitnum)
{
	if (byte < data.size())
		return (data.at(byte) >> (bitnum)) & 1;
	else
		return 0;
}

void File::readFile(std::string file)
{
	std::cout << file << "\n";
	std::ifstream input;
	input.open(file.c_str(), std::ios::binary);

	if (!input.good())
		std::cout << "its not good\n";

	data = std::vector<uint8_t>(std::istreambuf_iterator<char>(input), {});



}

void File::exportFile(std::string file)
{
	std::fstream output;
	output.open(file.c_str(), std::ios::out | std::ios::binary);

	output.write(reinterpret_cast<const char*>(&data[0]), data.size());
}

void File::readPNG(std::string img)
{
	std::cout << "\n\n READING \n\n";

	png::image<png::rgb_pixel> image(img);
	size_t width = image.get_width(), height = image.get_height();
	std::vector<png::rgb_pixel> imgpixels;

	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			auto pix = image.get_pixel(x, y);
			imgpixels.push_back(png::rgb_pixel(pix.red % 2, pix.green % 2, pix.blue % 2));
		}
	}

	size_t FileSize = 0;

	for (size_t i = 0; i < 11; i++)
	{
		auto pixel = imgpixels.at(i);

		uint8_t shift = (i * 3);
		FileSize |= ((pixel.red % 2) << shift);
		FileSize |= ((pixel.green % 2) << (shift + 1));
		FileSize |= ((pixel.blue % 2) << (shift + 2));

	}

	std::cout << "File size: " << FileSize << "\n";

	imgpixels.erase(imgpixels.begin(), imgpixels.begin() + 11);


	data.clear();
	while (data.size() != FileSize + 1)
		data.push_back(0);

	std::cout << "new size: " << data.size() << "\n";

	std::cout << ceil((FileSize * 8) / 3.00) << "\n";

	for (size_t i = 0; i < ceil((FileSize * 8) / 3.00); i++)
	{
		auto pixel = imgpixels.at(i);

		uint8_t shift = (i * 3);

		data.at((i * 3) / 8) |= ((pixel.red % 2) << (shift % 8));
		data.at((i * 3 + 1) / 8) |= ((pixel.green % 2) << ((shift + 1) % 8));
		data.at((i * 3 + 2) / 8) |= ((pixel.blue % 2) << ((shift + 2) % 8));

	}


	data.pop_back();


}

void File::exportPNG(std::string originalimage, std::string newimage)
{
	std::cout << "\n\n EXPORTING \n\n";
	png::image<png::rgb_pixel> origimage(originalimage);

	size_t width = origimage.get_width(), height = origimage.get_height();

	auto FileSize = data.size();
	std::cout << "> File Size: " << FileSize << "\n";

	std::vector<png::rgb_pixel> imgfilter;

	for (size_t bit = 0; bit < (4 * 8); bit += 3)
	{
		png::rgb_pixel pix;
		pix.red = getBitFromByte(getByteFromInt(FileSize, bit / 8), bit % 8);
		pix.green = getBitFromByte(getByteFromInt(FileSize, (bit + 1) / 8), (bit + 1) % 8);
		pix.blue = getBitFromByte(getByteFromInt(FileSize, (bit + 2) / 8), (bit + 2) % 8);


		imgfilter.push_back(pix);
	}

	for (size_t bit = 0; bit < FileSize * 8; bit += 3)
	{
		png::rgb_pixel pix;

		pix.red = getBit(bit / 8, bit % 8);
		pix.green = getBit((bit + 1) / 8, (bit + 1) % 8);
		pix.blue = getBit((bit + 2) / 8, (bit + 2) % 8);


		imgfilter.push_back(pix);
	}

	for (size_t pix = 0; pix < imgfilter.size(); pix++)
	{
		size_t x = pix % width, y = pix / width;

		auto pixel = origimage.get_pixel(x, y);
		auto filterpixel = imgfilter.at(pix);

 
		pixel.red += (!(pixel.red % 2 == filterpixel.red)) * ((pixel.red == 255) ? -1 : 1);
		pixel.green += (!(pixel.green % 2 == filterpixel.green)) * ((pixel.green == 255) ? -1 : 1);
		pixel.blue += (!(pixel.blue % 2 == filterpixel.blue)) * ((pixel.blue == 255) ? -1 : 1);


		origimage.set_pixel(x, y, pixel);
	}

	origimage.write(newimage);
}