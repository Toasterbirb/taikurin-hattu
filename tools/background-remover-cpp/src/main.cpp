#include <algorithm>
#include <execution>
#include <exception>
#include <filesystem>
#include "Magick++/Color.h"
#include "Magick++/Functions.h"
#include "Magick++/Geometry.h"
#include "Magick++/Image.h"
#include "MagickCore/image.h"
#include "MagickCore/option.h"
#include <Magick++.h>
#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

static const Magick::Color color_transparent(0, 0, 0, 0);
static const Magick::Color color_white(65535, 65535, 65535, 65535);

std::string read_file(const std::string& fpath)
{
	std::ifstream file;
	file.open(fpath, std::ios::in);

	std::string file_contents;
	char c;
	while (file.good())
	{
		c = file.get();
		file_contents += c;
	}

	/* Fix the null terminator */
	file_contents.erase(file_contents.size() - 1, 1);

	file.close();
	return file_contents;
}

void floodfill_white(Magick::Image& image, int x, int y)
{
	if (image.pixelColor(x, y) == color_white)
		image.floodFillColor(x, y, color_transparent, false);
}

void remove_background(const std::string& input, const std::string& output)
{
	Magick::Image image;
	image.read(input);

	size_t width = image.boundingBox().width();
	size_t height = image.boundingBox().height();

	image.colorFuzz(1);

	for (size_t i = 0; i < width; ++i)
	{
		/* Top */
		floodfill_white(image, i, 0);

		/* Bottom */
		floodfill_white(image, i, height - 1);
	}

	for (size_t i = 0; i < height; ++i)
	{
		/* Left */
		floodfill_white(image, 0, i);

		/* Right */
		floodfill_white(image, width - 1, i);
	}

	image.write(output);
}

void remove_background_custom(const std::string& file, int x, int y)
{
	Magick::Image image;
	image.read(file);

	/* If the x and y coords are both -1, don't do anything */
	if (x == -1 && y == -1)
		return;

	/* If the x and y coords are both 0, remove all white color
	 * from the image. Otherwise proceed as normal with floodfill */
	if (x == 0 && y == 0)
	{
		image.transparent(color_white);
	}
	else
	{
		image.colorFuzz(1);
		image.floodFillColor(x, y, color_transparent, false);
	}

	image.write(file);
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " [exception file] [asset directory]\n";
		return 1;
	}

	/* Get the exception file path */
	std::string exception_file_path = argv[1];

	/* Get the asset directory path */
	std::filesystem::path dir_path(argv[2]);

	/* Create output directory */
	const std::string output_dir_path = "./output";
	std::filesystem::create_directory(output_dir_path);

	/* Read in the exception json file */
	nlohmann::json json_data = nlohmann::json::parse(read_file(exception_file_path));

	/* Prepare a list of image file paths */
	std::vector<std::filesystem::path> file_paths;
	std::filesystem::recursive_directory_iterator dir_iterator(dir_path);

	for (const auto& p : dir_iterator)
	{
		/* Make sure that the filepath is a png file */
		if (!p.is_regular_file())
			continue;

		/* Check if the file shuold be skipped */
		bool skip = false;
		for (size_t i = 0; i < json_data["skip"].size(); ++i)
		{
			/* Pre-create the directories for file that are to be skipped */
			std::filesystem::create_directories(output_dir_path + "/" + p.path().parent_path().filename().string());

			if (p.path().parent_path().filename().string() + "/" + p.path().filename().string() == json_data["skip"][i])
			{
				skip = true;
				std::cout << "Skipping " << p.path().filename() << "\n";

				/* Copy the file manually to the result directory if its
				 * not there already*/
				std::string target_path = output_dir_path + "/" + p.path().parent_path().filename().string() + "/" + p.path().filename().string();

				if (!std::filesystem::exists(target_path))
					std::filesystem::copy_file(p.path(), target_path);
				break;
			}
		}

		if (skip)
			continue;

		file_paths.push_back(p);
	}

	std::for_each(std::execution::par, file_paths.begin(), file_paths.end(), [](auto&& p)
		{
			std::cout << "Processing " << p.filename() << "\n";

			try {
				std::string parent_dir = "./output/" + p.parent_path().filename().string();

				/* Check if the file has been processed already */
				if (!std::filesystem::exists(parent_dir + "/" + p.filename().string()))
				{
					std::filesystem::create_directory(parent_dir);
					remove_background(p.string(), parent_dir + "/" + p.filename().string());
				}
			} catch (std::exception e) {
				std::cout << e.what() << "\n";
			}
		});

	/* Go through the exception list */
	std::cout << "Processing exceptions...\n";
	for (int i = 0; i < json_data["floodfill"].size(); ++i)
	{
		remove_background_custom("./output/" + std::string(json_data["floodfill"][i]["file"]), json_data["floodfill"][i]["x"], json_data["floodfill"][i]["y"]);
	}

	return 0;
}
