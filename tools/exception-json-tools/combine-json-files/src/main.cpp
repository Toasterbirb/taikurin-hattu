#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <vector>

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

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " [path to json directory] [output file]\n";
		return 1;
	}

	std::string json_dir_path = argv[1];
	std::string output_path   = argv[2];

	/* Read all of the json files recursively */
	std::filesystem::recursive_directory_iterator dir_iterator(json_dir_path);
	nlohmann::json combined_json;

	for (const auto& p : dir_iterator)
	{
		nlohmann::json json_data = nlohmann::json::parse(read_file(p.path()));

		/* Add skipped files */
		for (size_t i = 0; i < json_data["skip"].size(); ++i)
			combined_json["skip"].push_back(json_data["skip"][i]);

		/* Add coordinates */
		for (size_t i = 0; i < json_data["floodfill"].size(); ++i)
		{
			nlohmann::json floodfill = json_data["floodfill"][i];
			combined_json["floodfill"].push_back(floodfill);
		}
	}

	/* Write the combined json to a file */
	std::ofstream file(output_path);
	file << std::setw(4) << combined_json << std::endl;

	return 0;
}
