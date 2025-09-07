#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <Arduino.h>

class HttpHandler
{

  String servername;

public:
  HttpHandler();
  void init(String _servername);
  String get(String boardID, String path);
  String post(String boardID, String path, String msg_json);
  String put(String boardID, String path, String msg_json);
};

#endif
