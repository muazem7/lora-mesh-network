#include "FileHandler.h"
#include "board_pins.h"
#include <SD.h>
#include <SPI.h>

FileHandler::FileHandler() {}

/**
 * Initializes SPI and prints SD card information
 */
bool FileHandler::init()
{
    bool result = false;

#if defined(SD_SCLK) && (SD_MISO) && (SD_MOSI) && (SD_CS)
    // Specify pin to initialize SPI
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS))
    {
        Serial.println("SDCard MOUNT FAIL");
    }
    else
    {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        String str = "SDCard Size: " + String(cardSize) + "MB";
        Serial.println(str);
        result = true;
    }
#endif

    isInitialised = result;
    return result;
}

/**
 * Creates a new file
 * @param _filename The name of the file to create
 */
bool FileHandler::createFile(String _filename)
{
    if (!isInitialised)
    {
        Serial.println("createFile Error SD is not initialised");
        return false;
    }

    filename = String("/" + _filename);
    bool result = false;
    File file = SD.open(filename, FILE_WRITE);

    // If the file opened successfully, write to it:
    if (file)
    {
        // Close the file:
        file.close();
        Serial.println("done.");
        result = true;
    }
    else
    {
        // If the file didn't open, print an error:
        Serial.println("error opening " + filename);
    }
    return result;
}

/**
 * Appends a line to an existing file
 * @param _filename The name of the file
 * @param text The text to append
 */
bool FileHandler::appendLine(String _filename, String text)
{
    if (!isInitialised)
    {
        Serial.println("writeLine Error SD is not initialised");
        return false;
    }

    filename = String("/" + _filename);
    bool result = false;
    File file = SD.open(filename, FILE_APPEND);

    // If the file opened successfully, write to it:
    if (file)
    {
        Serial.println("Writing " + text + " to " + filename);
        file.print(text + "\n");
        // Close the file:
        file.close();
        Serial.println("write line done.");
        result = true;
    }
    else
    {
        // If the file didn't open, print an error:
        Serial.println("error opening " + filename);
    }
    return result;
}

/**
 * Reads a line from a file
 * @param _filename The name of the file
 * @param line_num Line number
 */
String FileHandler::readLine(String _filename, uint32_t line_num)
{
    if (!isInitialised)
    {
        Serial.println("readLine Error SD is not initialised");
        return "null";
    }

    if (line_num == 0)
    {
        Serial.println("Line number must be > 0");
        return "null";
    }
    filename = String("/" + _filename);

    File file = SD.open(filename);
    String line;
    uint32_t l_num = 0;
    // If the file opened successfully, read from it:
    if (file)
    {
        Serial.printf("Reading line %d in file %s\n", line_num, filename.c_str());

        while (file.available() && (line_num > l_num))
        {
            line = file.readStringUntil('\n');
            ++l_num;
        }
        if (line_num > l_num)
            Serial.printf("File does not have more than %d line/s\n", l_num);
        else
            Serial.println("line num " + String(l_num) + " : " + line);

        file.close();
        Serial.println("read line done.");
    }
    else
    {
        // If the file didn't open, print an error:
        Serial.println("error opening " + filename);
    }
    return line;
}

/**
 * Reads the entire file
 * @param _filename The name of the file
 */
void FileHandler::readFile(String _filename)
{
    if (!isInitialised)
    {
        Serial.println("readLine Error SD is not initialised");
        return;
    }

    filename = String("/" + _filename);

    File file = SD.open(filename);
    String line;
    uint32_t l_num = 1;
    // If the file opened successfully, read from it:
    if (file)
    {
        Serial.println("Reading file " + filename);
        while (file.available())
        {
            line = file.readStringUntil('\n');
            Serial.println("line num " + String(l_num) + " : " + line);
            ++l_num;
        }

        file.close();
        Serial.println("read file done.");
    }
    else
    {
        // If the file didn't open, print an error:
        Serial.println("error opening " + filename);
    }
    return;
}

/** Test method
 * Lists all files on the SD card
 */
void FileHandler::listFiles()
{
    if (!isInitialised)
    {
        Serial.println("readLine Error SD is not initialised");
        return;
    }

    File root = SD.open("/");

    printDirectory(root, 0);
}

/** Test method
 * Lists all directories on the SD card
 */
void FileHandler::printDirectory(File dir, int numTabs)
{
    while (true)
    {
        File entry = dir.openNextFile();

        if (!entry)
        {
            // No more files
            break;
        }

        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }

        Serial.print(entry.name());

        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            // Files have sizes, directories do not
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }

        entry.close();
    }
}
