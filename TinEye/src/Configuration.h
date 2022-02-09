#include <fstream>
#include <nlohmann/json.hpp>
#include <map>


using json = nlohmann::json;

struct ResolutionGuidelines {
	size_t width;
	size_t height;
	ResolutionGuidelines() :width(0), height(0) {}
	ResolutionGuidelines(size_t w, size_t h)
		:width(w), height(h) {}
};

class Configuration {
private:
	float contrastRatio;
	std::string language;
	std::unordered_map<int, ResolutionGuidelines> resolutionGuidelines;
	ResolutionGuidelines* activeResolution = nullptr;
public:
	Configuration(std::string configPath) {
		json config;
		std::ifstream configFile(configPath);
		if (configFile) {
			configFile >> config;
			contrastRatio = config["contrast"];
			language = config["language"];

			for (auto it = config["resolutions"].begin(); it != config["resolutions"].end(); ++it)
			{
				ResolutionGuidelines a(it.value()["width"], it.value()["height"]);
				resolutionGuidelines[atoi(it.key().c_str())] = a;
			}
		}
		else {
			//If file is not found, error and set default values
			std::cerr << "Configuration file not found, falling back to default configuration\n";
			std::cerr << "Contrast ratio: 4.5, language: eng" << std::endl;

			contrastRatio = 4.5f;
			language = "eng";
		}

	}

	void setActiveResolution(int resolution) {
		auto foundRes = resolutionGuidelines.find(resolution);
		//Check if specified resolution was added during file load
		if (foundRes == resolutionGuidelines.end()) {
			//If not found, error and create default for 1080
			std::cerr << "Specified resolution not found, using 1080p, 28x4px as baseline" << std::endl;
			resolutionGuidelines[1080] = ResolutionGuidelines(4, 28);
			activeResolution = &resolutionGuidelines.find(1080)->second;
		}
		else {
			activeResolution = &foundRes->second;
		}
	};

	size_t getHeightRequirement() const { return activeResolution->height; }
	size_t getWidthRequirement() const { return activeResolution->width; }
	float getContrastRequirement() const { return contrastRatio; }
	std::string getLanguage() const { return language; }
};