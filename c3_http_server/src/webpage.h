#pragma once

#include <queue>
#include <WebServer.h> // Include for the WebServer type

namespace WebPage
{

    // Size of the XML buffer
    constexpr size_t XmlBufferSize = 2048;

    // Declare the XML buffer (optional: extern if you want to access it outside)
    extern char XML[XmlBufferSize];

    extern int webSubmittedValue;
    // bool XMLSent;
    // std::queue<int> key_buffer;
    // 如果不是extern, 则会有重复定义的错误，不需要在这里定义

    void update_XML(int val);
    void newKeyInput(int val);

    // Declare the WebServer instance accessible externally (optional)
    extern WebServer server;

    // Function to send the main HTML page
    void SendHtml();

    // Initialize HTTP server and handlers
    void init_html_pages();

}
