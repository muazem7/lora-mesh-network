#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <Arduino.h>
#include <SD.h>

class FileHandler
{

private:
  String filename;

public:
  bool isInitialised;
  FileHandler();
  bool init();
  bool appendLine(String _filename, String text);
  String readLine(String _filename, uint32_t line_num);
  void listFiles();
  void printDirectory(File dir, int numTabs);
  void readFile(String _filename);
  bool createFile(String _filename);
};
#endif