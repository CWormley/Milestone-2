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
#include <vector>
#include <cfloat>
#include <iomanip> 
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
void readTest(std::string inputFileName, int readSize) {
    FILE* fp;
    std::vector<char> buffer(readSize);
    int counter = 0;

    fp = fopen(inputFileName.c_str(), "rb");
    if (!fp) {
        std::cerr << "Error opening file: " << inputFileName << std::endl;
        return;
    }

    while (!feof(fp)) {
        fread(buffer.data(), readSize, 1, fp);
        counter++;
    }

    fclose(fp);
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
void timeWrapper(std::string functionName, std::string inputFileName, int numberOfIterations, int readSize) {
    using namespace std::chrono;

    const int groupSize = 1000;  // print results every 1000 iterations
    double totalTime = 0.0;
    double minTime = DBL_MAX;
    double maxTime = 0.0;

    auto testStart = steady_clock::now();
    auto startTime = system_clock::now();
    std::time_t start_time = system_clock::to_time_t(startTime);

    std::cout << "Starting computation at " << std::ctime(&start_time);
    logToFileAndConsole("end time\titer #\tavg (µs)\tmin (µs)\tmax (µs)");

    for (int i = 1; i <= numberOfIterations; ++i) {
        auto t1 = high_resolution_clock::now();

        if (functionName == "readTest") {
            readTest(inputFileName, readSize);
        }

        auto t2 = high_resolution_clock::now();
        double iterTime = duration<double, std::micro>(t2 - t1).count();

        totalTime += iterTime;
        minTime = std::min(minTime, iterTime);
        maxTime = std::max(maxTime, iterTime);

        if (i % groupSize == 0) {
            auto now = system_clock::now();
            std::time_t now_c = system_clock::to_time_t(now);
            std::tm* local = std::localtime(&now_c);
            char timeStr[10];
            std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", local);

            double avg = totalTime / groupSize;

            logToFileAndConsole(std::string(timeStr) + "\t" +
                std::to_string(i) + "\t" +
                std::to_string(avg) + "\t" +
                std::to_string(minTime) + "\t" +
                std::to_string(maxTime));

            // reset for next group
            totalTime = 0.0;
            minTime = DBL_MAX;
            maxTime = 0.0;
        }
    }

    auto testEnd = steady_clock::now();
    double totalElapsed = duration<double>(testEnd - testStart).count();

    std::time_t end_time = system_clock::to_time_t(system_clock::now());
    std::cout << "Finished computation at " << std::ctime(&end_time)
              << "Total elapsed time: " << totalElapsed << " seconds\n";
}

void timeWrapperTimed(const std::string& functionName, const std::string& inputFileName, int readSize) {
    using namespace std::chrono;

    const int groupSize = 1000;
    double totalTime = 0.0;
    double minTime = DBL_MAX;
    double maxTime = 0.0;
    int iter = 0;

    auto start = steady_clock::now();

    // Run for ~10 seconds
    while (duration_cast<seconds>(steady_clock::now() - start).count() < 10) {
        auto t1 = high_resolution_clock::now();
        readTest(inputFileName, readSize);
        auto t2 = high_resolution_clock::now();

        double elapsed = duration<double, std::micro>(t2 - t1).count();  // microseconds
        totalTime += elapsed;
        minTime = std::min(minTime, elapsed);
        maxTime = std::max(maxTime, elapsed);
        iter++;

        if (iter % groupSize == 0) {
            // Get current time for logging
            auto now = system_clock::now();
            std::time_t now_c = system_clock::to_time_t(now);
            char buf[10];
            std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now_c));

            // Compute average per group
            double avg = totalTime / groupSize;

            logToFileAndConsole(
                std::string(buf) + "\t" +
                std::to_string(iter) + "\t" +
                std::to_string(avg) + "\t" +
                std::to_string(minTime) + "\t" +
                std::to_string(maxTime)
            );

            // Reset counters for next batch
            totalTime = 0.0;
            minTime = DBL_MAX;
            maxTime = 0.0;
        }
    }

    logToFileAndConsole(
        "Finished 10-second timed run. Total iterations: " + std::to_string(iter)
    );
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
void processTestCase(const std::string& testCaseName, const json& testCaseArray, int readSize) {
    logToFileAndConsole("\nProcessing " + testCaseName + " commands:\n\n");

    for (size_t i = 0; i < testCaseArray.size(); ++i) {
        const json& entry = testCaseArray[i];

        for (json::const_iterator it = entry.begin(); it != entry.end(); ++it) {
            const std::string& actionName = it.key();
            const json& details = it.value();

            if (actionName == "readTestTimed") {
                const std::string& inputFileName = details["inputFileName"];
                logToFileAndConsole("Calling: timeWrapperTimed(" + actionName + ", " + inputFileName + ")");
                timeWrapperTimed(actionName, inputFileName, readSize);
                logToFileAndConsole("Finished " + actionName + ".");
            } 
            else if (actionName == "readTest") {
                const std::string& inputFileName = details["inputFileName"];
                int numberOfIterations = details["numberOfIterations"];
                logToFileAndConsole("Calling: timeWrapper(" + actionName + ", " + inputFileName + ", " + std::to_string(numberOfIterations) + ")");
                timeWrapper(actionName, inputFileName, numberOfIterations, readSize);
                logToFileAndConsole("Finished " + actionName + ".");
            } 
            else {
                logToFileAndConsole("Encountered unknown command: " + actionName + ". Check commands in JSON file.");
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
    int numberOfIterations = config["Milestone2"][0]["defaultVariables"][0]["numberOfIterations"];

    // Retrieve default readsize
    int readSize = config["Milestone2"][0]["defaultVariables"][0]["readSize"];


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
            processTestCase(testCaseName, testCaseArray, readSize);

        }
    }

    logToFileAndConsole("\n\nEnd of unit tests");

    // Close files
    configFile.close();
    testcaseFile.close();
    outFile.close();

    return 0;
}
