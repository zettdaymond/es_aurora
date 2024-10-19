#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <thread>

#include <filesystem>
#include <vector>

#include <SDL2/SDL_filesystem.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_log.h>

namespace fs = std::filesystem;

extern "C" {
int renpython_main(int argc, char** argv);
}

class ChronoTimer {
    using Clock = std::chrono::high_resolution_clock;

public:
    ChronoTimer(std::string entryName)
       : entryName(entryName)
    {
        startTime = Clock::now();
    }

    ~ChronoTimer()
    {
        auto endTime = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        auto durationMs = duration.count();

        SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "PROFILE: %s took %lli ms", entryName.c_str(), durationMs);
    }

private:
    std::string entryName;
    Clock::time_point startTime;
};

bool Contains(std::string_view s1, std::string_view s2)
{
    if (s1.find(s2) != std::string::npos) {
        return true;
    }
    return false;
}

bool CopyData(fs::path const& copy, fs::path const& original, std::vector<std::string> ignorePatterns = {})
{
    SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Started copy from : %s to %s", original.c_str(), copy.c_str());

    ChronoTimer timer("Coping resources");

    if (!fs::exists(original)) {
        return false;
    }

    if (fs::is_directory(original)) {
        fs::create_directories(copy);

        for (auto const& dirEntry : std::filesystem::directory_iterator{original}) {
            auto dirPath = dirEntry.path();
            bool needToBeExcluded = std::any_of(ignorePatterns.begin(),
                                                ignorePatterns.end(),
                                                [&dirPath](const auto& pattern) {
                                                    return Contains(dirPath.string(), pattern);
                                                });

            if (needToBeExcluded) {
                continue;
            }

            auto leaf = dirPath.filename();
            auto copyTo = copy / leaf;

            SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION,
                           "Copying from : %s to %s (Extracted leaf : %s)",
                           dirPath.c_str(),
                           copyTo.c_str(),
                           leaf.c_str());

            fs::copy(dirPath, copyTo, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        }
    } else {
        if (!fs::exists(copy.parent_path())) {
            fs::create_directories(copy.parent_path());
        }
        fs::copy(original, copy);
    }

    return true;
}

bool ResourcesValid(fs::path gameDir, uint32_t resourceVersion)
{
    const auto versionFilePath = gameDir / "resources_version.txt";

    try {
        if (!fs::exists(versionFilePath)) {
            return false;
        }

        std::ifstream fileVersionStream(versionFilePath.string());

        std::string fileVersionString;
        fileVersionStream >> fileVersionString;

        return (std::to_string(resourceVersion) == fileVersionString);

    } catch (std::filesystem::filesystem_error const& ex) {
        throw;
    } catch (std::ios_base::failure const& e) {
        return false;
    } catch (std::exception const& e) {
        throw;
    }

    return true;
}

bool MarkResourcesAsValid(fs::path gameDir, uint32_t resourceVersion)
{
    const auto versionFilePath = gameDir / "resources_version.txt";

    try {
        if (!fs::exists(versionFilePath)) {
            fs::remove(versionFilePath);
        }

        std::ofstream fileVersionStream(versionFilePath.string());

        std::string fileVersionString = std::to_string(resourceVersion);
        fileVersionStream << fileVersionString;

        return true;

    } catch (std::filesystem::filesystem_error const& ex) {
        throw;

    } catch (std::ios_base::failure const& e) {
        return false;
    } catch (std::exception const& e) {
        throw;
    }

    return false;
}

bool MakeSymlink(fs::path symlink, fs::path original)
{
    if (!fs::exists(original)) {
        return false;
    }

    try {
        if (fs::exists(symlink)) {
            fs::remove(symlink);
        }

        if (fs::is_directory(original)) {
            fs::create_directory_symlink(original, symlink);
        } else {
            fs::create_symlink(original, symlink);
        }
    } catch (std::exception& e) {
        throw;
    }

    return true;
}

extern "C" __attribute__((visibility("default")))
int main(int argc, char** argv)
{
    constexpr static uint32_t kResourcesVersion = 10;
    constexpr static bool kDeveloperMode = false;

    if(kDeveloperMode) {
        SDL_SetHint("SDL_LOGGING", "*=verbose");
        putenv("RENPY_LOG_TO_STDOUT=1");
        putenv("RENPY_LOG_EVENTS=1");
    }

    fs::path packageFilesPath = "/usr/share/su.sovietgames.everlasting_summer/";
    fs::path targetDirectoryPath = fs::path(SDL_GetPrefPath("su.sovietgames", "everlasting_summer")) / "files";

    try {
        if (!fs::exists(packageFilesPath)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                         "Package files could not be located at path : %s",
                         packageFilesPath.c_str());
            return -1;
        }

        if (!fs::exists(targetDirectoryPath)) {
            fs::create_directories(targetDirectoryPath);
        }

        if (!ResourcesValid(targetDirectoryPath, kResourcesVersion)) {
            fs::remove_all(targetDirectoryPath);
            fs::create_directories(targetDirectoryPath);

            CopyData(targetDirectoryPath / "lib" / "python2.7", packageFilesPath / "lib" / "python2.7");
            CopyData(targetDirectoryPath / "renpy", packageFilesPath / "renpy");
            CopyData(targetDirectoryPath / "main.py", packageFilesPath / "main.py");

            CopyData(targetDirectoryPath / "game", packageFilesPath / "game", {"assets.rpa"});
            MakeSymlink(targetDirectoryPath / "game" / "assets.rpa", packageFilesPath / "game" / "assets.rpa");

            MarkResourcesAsValid(targetDirectoryPath, kResourcesVersion);
        }

    } catch (const std::exception& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "exception occured : %s", e.what());
        return -1;
    }

    auto pythonPath = targetDirectoryPath / "lib" / "python";
    auto mainPath = targetDirectoryPath / "main.py";

    int argcToPython = 2;
    char* argvToPython[] = {
       const_cast<char*>(pythonPath.c_str()),
       const_cast<char*>(mainPath.c_str()),
       nullptr,
    };

    return renpython_main(argcToPython, argvToPython);
}
