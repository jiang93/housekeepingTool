//  main.cpp
//  housekeepingTool
//  Created by Jiang on 12/7/21.

#include <stdlib.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <set>
#include <map>
#include <list>
#include <yaml-cpp/yaml.h>
#include <fstream>

using namespace std;

int manageFilesByDate(string directory, int days);
int manageFilesByNumber(string directory, int numuber);

int main(int argc, const char *argv[]) {
    //Check the number of parameters passed in main
    if (argc==2) {
        //Check the input configuration file
        string inputFile = argv[1];
        //std::string inputFile = "";
        ifstream openInputFile(inputFile);
        //If input file not exist, display error message
        if (!openInputFile.is_open()) {
            cout << "Failed to open YAML configuration file." << endl;
            return 1;
        }
        
        //Load the input configuration file
        YAML::Node config = YAML::LoadFile(inputFile);
        string filePath;
        string configuration;
        int limit;
        
        int count = 0;
        
        //Parse configuration data to variable
        for (YAML::const_iterator inst = config.begin(); inst != config.end(); ++inst) {
            if (count==0) {
                filePath = inst->second.as<string>();
                count = count + 1;
            }
            else if (count==1) {
                configuration = inst->second.as<string>();
                count = count + 1;
            }
            else {
                limit = inst->second.as<int>();
                //std::cout << filePath << " " << configuration << " " << limit << endl;
                        //If configuration is maximumNumberOfFiles
                        if (configuration=="maximumNumberOfFiles") {
                            manageFilesByNumber(filePath, limit);
                        }
                        //If configuration is maximumNumberOfDays
                        else if (configuration=="maximumNumberOfDays") {
                            manageFilesByDate(filePath, limit);
                        }
                        //If other configuration
                        else {
                            cout << "Please specify a valid configuration in the input file! Set the configuration field to maximumNumberOfFiles if you want to manage files according to the maximum number of files, or set the field to maximumNumberOfDays if you want to remove files older than a date." << endl;
                            return 1;
                        }
                count = 0;
            }
        }
    }
    else {
        cout << "Failed to run the task. Invalid parameter(s) passed to this program." << endl;
        return 1;
    }
    return 0;
}

int manageFilesByNumber(string directory, int number) {
    int noOfFilesExist = 0;
    std::set <time_t> sortFileDates;
    std::list <std::string> sortFileByDate;
    
    //If directory not exists
    if(!std::__fs::filesystem::exists(directory)) {
        cout << "Directory does not exist" << endl;
        return 1;
    }

    //Iterate over files in the given directory
    for (auto& fileName:std::__fs::filesystem::directory_iterator(directory)) {
        noOfFilesExist = noOfFilesExist + 1;
        auto lastModifiedDate = std::__fs::filesystem::last_write_time(fileName.path());
        std::time_t fileLastModifiedDate = decltype(lastModifiedDate)::clock::to_time_t(lastModifiedDate);
        sortFileDates.insert(fileLastModifiedDate);
    }

    //When number of files exceeds the maximum limit
    if (noOfFilesExist>number) {
        int noOfFilesToDelete = noOfFilesExist - number;
        //Iterate over last modified dates in the above set
        for (auto it=sortFileDates.begin(); it!=sortFileDates.end(); ++it) {
            //Iterate overs accoring to the last modified date
            for (auto& fileName:std::__fs::filesystem::directory_iterator(directory)) {
                auto lastModifiedDate = std::__fs::filesystem::last_write_time(fileName.path());
                std::time_t fileLastModifiedDate = decltype(lastModifiedDate)::clock::to_time_t(lastModifiedDate);
                // Delete file
                if (fileLastModifiedDate==*it && noOfFilesToDelete!=1) {
                    noOfFilesToDelete = noOfFilesToDelete - 1;
                    int fileDelete = std::__fs::filesystem::remove(fileName.path());
                    if (!fileDelete) {
                        cout << "Unable to delete " << fileName.path() << endl;
                        return 1;
                    }
                    else {
                        cout <<fileName.path() << " deleted" << endl;
                    }
                }
            }
        }
    }
    return 0;
}

int manageFilesByDate(string directory, int days) {
    int dayInSecond = 86400;
    time_t currentTimeInSeconds = time(0);
    time_t setTimeInSeconds = currentTimeInSeconds - (dayInSecond*days);
    
    //If directory not exists
    if(!std::__fs::filesystem::exists(directory)) {
        cout << "Directory does not exist" << endl;
        return 1;
    }

    //Iterate over files in the given directory
    for (auto& fileName:std::__fs::filesystem::directory_iterator(directory)) {
        auto lastModifiedDate = std::__fs::filesystem::last_write_time(fileName.path());
        time_t fileLastModifiedDate = decltype(lastModifiedDate)::clock::to_time_t(lastModifiedDate);
        //Delete files
        if (fileLastModifiedDate<setTimeInSeconds) {
            int fileDelete = std::__fs::filesystem::remove(fileName.path());
            if (!fileDelete) {
                cout << "Unable to delete " << fileName.path() << endl;
                return 1;
            }
            else {
                cout <<fileName.path() << " deleted" << endl;
            }
        }
    }
    return 0;
}
