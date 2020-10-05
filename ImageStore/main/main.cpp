#include "main.h"
#include <iostream>
#include <file/file.h>
#include <Windows.h>

int main()
{
	File file;

	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);

	std::string Path(path);

	//file.readFile(Path + "\\test.mp4");


	//file.exportPNG(Path + "\\mario.png", Path + "\\coolmario.png");
	file.readPNG(Path + "\\coolmario.png");
	file.exportFile(Path + "\\test.mp4");

	std::cout << "Kill them\n";
	system("pause");
}