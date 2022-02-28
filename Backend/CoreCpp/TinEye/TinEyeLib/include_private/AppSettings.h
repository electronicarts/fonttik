#pragma once
#include <filesystem>
namespace fs = std::filesystem;

class AppSettings {
	bool dbgSaveLuminanceMap, 
		dbgSaveTexboxOutline, 
		dbgSaveSeparateTextboxes, 
		dbgSaveHistograms,
		dbgSaveRawTextboxOutline;
	fs::path resultsPath, debugInfoPath;

public:
	AppSettings(bool saveLum, bool saveTextbox, bool saveSeparateTexbox, bool saveHist, bool saveRawTextbox,
		fs::path resultsPath, fs::path dbgInfoPath) :dbgSaveLuminanceMap(saveLum),
		dbgSaveTexboxOutline(dbgSaveTexboxOutline),dbgSaveRawTextboxOutline(saveRawTextbox),
		dbgSaveSeparateTextboxes(saveSeparateTexbox), dbgSaveHistograms(saveHist),
		resultsPath(resultsPath), debugInfoPath(dbgInfoPath) {}

	//AppSettigs
	bool saveLuminanceMap() const { return dbgSaveLuminanceMap; }
	bool saveTexboxOutline() const { return dbgSaveTexboxOutline; }
	bool saveSeparateTextboxes() const { return dbgSaveSeparateTextboxes; }
	bool saveHistograms() const { return dbgSaveHistograms; }
	bool saveRawTexboxOutline() const { return dbgSaveRawTextboxOutline; }
	fs::path getResultsPath() const { return resultsPath; }
	fs::path getDebugInfoPath() const { return debugInfoPath; }
};