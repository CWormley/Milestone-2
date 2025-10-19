/**
*
* milestone2.cpp : This file contains the 'main' function. Program execution begins and ends there.
*
* 09/23/24 - Created by ChatGPT with prompt "write C++ program reads and parses the file: milestone2_config.json"
*            The file: "milestone2_config.json" is in the following format:
*
                {
                    "Milestone2": [
                        {
                            "files": [
                                {
                                    "testcaseFile": "milestone2.json",
                                    "outputFile": "generatedOutputFile.txt",
                                    "errorLogFile":"logFile.txt"
                                }
                            ],
                            "defaultVariables": [
                                {
                                    "numberOfIterations": 10000,
                                    "readSize": 100
                                }
                            ]
                        }
                    ]
                }
            and where the testcaseFile has the following format:
                {
                    "FileReadBenchmark": [
                        {
                            "testCase1": [
                                {
                                    "readTest": {"inputFileName": "top-output.txt", "numberOfIterations": 100}
                                }
                                ...
                            ]
                        }
                    ]
                }

9/15/2025 - create by Joseph Hui;

*/
#define _CRT_SECURE_NO_WARNINGS
#define CONFIG_FILE "milestone2_config.json"

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "json.hpp"

using json = nlohmann::json;

// Global variable to be used for logging output
std::ofstream _outFile;

/**
*
* getOutFile
*
* @brief function to return pointer to outFile
*
* @param        none
*
* @return       pointer to output file
*/
std::ofstream& getOutFile() {
    return _outFile;
}


/**
*
* setOutFile
*
* @brief function to set path and open output file
*
* @param filePath       the path to output file
*
* @return               nothing
*/
void setOutFile(const std::string& filePath) {
    // Close the current file if it's already open
    if (_outFile.is_open()) {
        _outFile.close();
    }

    // Open the new file
    _outFile.open(filePath);
    if (!_outFile.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
    }
}

/**
 * logToFileAndConsole
 *
 * @brief Logs a message to both the console and the output file.
 *
 * This helper function prints a message to the console and writes the same
 * message to the output file.
 *
 * @param message The message to log
 */
void logToFileAndConsole(std::string message) {
    // Get the output file
    std::ofstream& outFile = getOutFile();

    std::cout << message << std::endl;  // Print to console 
    outFile << message << std::endl;  // Write to file
}

/**
*
* readTest
*
* @brief Simple file readtest to use as a template for benchmark
*
* @param    inputFileName       input file name
*
* @return   nothing, but output is sent to console and written to output file
*/
void readTest(std::string inputFileName) {
    FILE* fp;
    char buffer[100];
    int counter = 0;

    fp = fopen(inputFileName.c_str(), "rb");
    while (!feof(fp)) {
        fread(buffer, sizeof(buffer), 1, fp);
        counter++;
    }

    fclose(fp);

    return;
}


/**
*
* timeWrapper
*
* @brief Simple wrapper function that times a limited number of functions (hardcoded for now)
*
* @param    functionName        name of function to all
* @param    inputFileName       input file name
* @param    numberOfIterations  number of times to run the read
*
* @return   nothing, but output is sent to console and written to output file
*/
void timeWrapper(std::string functionName, std::string inputFileName, int numberOfIterations) {
    // need to write out the data for each timed iteration in the following format:
    // 
    // end time    iter#   avg     min     max     
    // 
    // <time1>         1   1.2     0.9     1.4
    // <time2>         2   1.1     0.7     1.2
    // ...
    
    // set the start time
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);

    // output some helpful comments to the console
    std::cout << "Starting computation at " << std::ctime(&start_time);

    // write out the head line for file
    logToFileAndConsole("end time\titer #\t\tavg\t\tmin\t\tmax\t\t");

    // call the specific function to time
    for (int i = 0; i < numberOfIterations; i++) {
        /*
        **  Note that none of the values are currently calculated.  This is part of the homework!
        */
        float average = 0.0;
        float min = 0.0;
        float max = 0.0;

        // add more functions here
        if (functionName == "readTest") {
            readTest(inputFileName);
        }

        // write out the current values for this iteration
        auto curIterEnd = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = curIterEnd - start;
        std::time_t iterEndTime = std::chrono::system_clock::to_time_t(curIterEnd);
        std::string timeString = "00:00:00";

        logToFileAndConsole(timeString + "\t" + std::to_string(i) + "\t\t" + std::to_string(average) + "\t" + std::to_string(min) + "\t" + std::to_string(max));
    }

    logToFileAndConsole("\n\n");

    // set the end time
    auto finalEnd = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = finalEnd - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(finalEnd);

    std::cout << "Finished computation at " << std::ctime(&end_time)
        << "Elapsed time: " << elapsed_seconds.count() << "s"
        << std::endl << std::endl;

    return;
}


/**
*
* processTestCase
*
* @brief function to proocess test cases, based upon test data in json format
*
* @param    testCaseName    test name
* @param    testCaseArray   test array
*
* @return                   nothing, but output is sent to console and written to output file
*/
void processTestCase(const std::string& testCaseName, const json& testCaseArray) {
    logToFileAndConsole("\nProcessing " + testCaseName + " commands:\n\n");

    for (size_t i = 0; i < testCaseArray.size(); ++i) {
        const json& entry = testCaseArray[i];

        for (json::const_iterator it = entry.begin(); it != entry.end(); ++it) {
            const std::string& actionName = it.key();
            const json& details = it.value();

            if (actionName == "readTest") {
                // this is where additional parameters can be read from the json file (i.e., specific readsize)
                const std::string& inputFileName = details["inputFileName"];
                int numberOfIterations = details["numberOfIterations"];

                logToFileAndConsole("Calling: timeWrapper(" + actionName + ", " + inputFileName + ", " + std::to_string(numberOfIterations) + ")\n");
                timeWrapper(actionName, inputFileName, numberOfIterations);
                logToFileAndConsole("Finished " + actionName + ".");
            }
            else {
                // default - don't know this command
                logToFileAndConsole("Encountered unknown command:" + actionName + ".  Check commands in json file");
            }
        }
    }

    logToFileAndConsole("\n\nFinished processing testcase, " + testCaseName + ".\n\n");
}

/**
*
* main
*
* main function which does the following:
*   read config file for input file, output file, error file, hash table size and FIFO size
*   create a hash table
*   for each of the test case
*       process test cases - display results to console and write to output file
*       print out the hash table
*       clear out hash table
*
* @param    none
*
* @return   nothing, but output is written to console and files
*/

int main() {
    // Load the config file

    std::ifstream configFile(CONFIG_FILE);
    if (!configFile.is_open()) {
        std::cerr << "Error opening config file!" << std::endl;
        return 1;
    }

    json config;
    configFile >> config;

    // Retrieve file paths from the config
    std::string testcaseFilePath = config["Milestone2"][0]["files"][0]["testcaseFile"];
    std::string outputFilePath = config["Milestone2"][0]["files"][0]["outputFile"];
    std::string errorFilePath = config["Milestone2"][0]["files"][0]["errorLogFile"];

    // Retrieve default numberOfIterations
    //int numberOfIterations = config["Milestone2"][0]["defaultVariables"][0]["numberOfIterations"];

    // Retrieve default readsize
    //int readSize = config["Milestone2"][0]["defaultVariables"][0]["readSize"];


    // Load the JSON testcase file
    std::ifstream testcaseFile(testcaseFilePath);
    if (!testcaseFile.is_open()) {
        std::cerr << "Failed to open the file: " << testcaseFilePath << ".\n";
        return 1;
    }

    // Open up the outfile and set the output file path using the setter
    //
    // Treating output file differently than input and config files because it's used in other files
    setOutFile(outputFilePath);

    // Get the output file
    std::ofstream& outFile = getOutFile();

    json data;
    testcaseFile >> data;

    // Process the test cases in the json file
    for (size_t i = 0; i < data["FileReadBenchmark"].size(); ++i) {
        const json& testCase = data["FileReadBenchmark"][i];
        for (json::const_iterator it = testCase.begin(); it != testCase.end(); ++it) {
            const std::string& testCaseName = it.key();
            const json& testCaseArray = it.value();
            processTestCase(testCaseName, testCaseArray);

        }
    }

    logToFileAndConsole("\n\nEnd of unit tests");

    // Close files
    configFile.close();
    testcaseFile.close();
    outFile.close();

    return 0;
}
