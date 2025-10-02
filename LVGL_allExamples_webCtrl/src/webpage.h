#pragma once

#include <Arduino.h>
#include <WebServer.h>
// #include <string>
#include "example_funcs.h"
#include "index_src.h"

namespace WebPage
{ // Declare the WebServer instance accessible externally (optional)
    extern WebServer server;
    extern int webSubmittedValue;
    void SendHtml();
    void getSubmit();
    void init_html_pages();
}
