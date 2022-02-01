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
	std::unordered_map<int,ResolutionGuidelines> resolutionGuidelines;
	ResolutionGuidelines* activeResolution = nullptr;
public:
	Configuration(std::string configPath) {
		json config;
		std::ifstream configFile(configPath);
		configFile >> config;
		contrastRatio = config["contrast"];
		language = config["language"];

		for (auto it = config["resolutions"].begin(); it != config["resolutions"].end(); ++it)
		{
			ResolutionGuidelines a(it.value()["width"], it.value()["height"]);
			resolutionGuidelines[atoi(it.key().c_str())] = a;
		}
	}

	void setActiveResolution(int resolution) { activeResolution = &resolutionGuidelines.find(resolution)->second; };

	size_t getHeightRequirement() const { return activeResolution->height; }
	size_t getWidthRequirement() const { return activeResolution->width; }
	float getContrastRequirement() const { return contrastRatio; }
	std::string getLanguage() const { return language; }
};