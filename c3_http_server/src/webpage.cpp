/*
The code was referrenced to https://github.com/KrisKasprzak/ESP32_WebPage/
It is based XML to communicate between web page and ESP32. The web page is controlled by JavaScript.
No additional library is required on the web page side.
*/

#include <Arduino.h>
#include <queue>
#include <WebServer.h>
#include "index_src.h"

namespace WebPage
{
    constexpr size_t XmlBufferSize = 2048;
    char XML[XmlBufferSize];
    int webSubmittedValue = -1;
    // bool XMLSent = true;
    std::queue<int> key_buffer;

    WebServer server(80);

    void update_XML(int val)
    {
        /*
        The size of the XML string should be smaller than XmlBufferSize
        */
        XML[0] = 0; // Clear the XML buffer
        snprintf(XML, XmlBufferSize,
                 "<data>"
                 "<v>%d</v>"
                 "</data>",
                 val);
    }

    void newKeyInput(int val)
    {
        key_buffer.push(val);
    }

    void SendHtml()
    {
        /* 
        The main web page is stored in index_src.h as a string constant PAGE_MAIN
        The HTML file can be edited seperately and then converted to the header file using the
        script (Linux): conv
        */
        server.send(200, "text/html", (const char *)PAGE_MAIN);
    }

    void SendXML()
    {
        /*
        This function is called when the web page requests /xml. So it will be called repeatedly.
        If setting a condition to not send XML unless there is a new key input, the web page will not get any response
        and apears error in the browser console "PUT xxxxxx"
        */
        if (key_buffer.empty())
            update_XML(0);
        else
        {
            update_XML(key_buffer.front());
            key_buffer.pop();
        }
        XMLSent = false;
        Serial.println("Sending updated XML");
        server.send(200, "text/xml", XML); // a longer timeout that 200 ms
        XMLSent = true;
    }

    void getSubmit()
    {
        if (server.hasArg("VALUE"))
        /*
        This is the key part to transmit data from webpage to server.
        On the webpage client end the code is something like this:

          function submitForm() {
                var xhttp = new XMLHttpRequest();
                xhttp.open("PUT", "submit?VALUE=" + sumInputs(), true);
                console.log("Submitting: " + sumInputs());
                xhttp.send();
            }
                The VALUE is the name of the argument, it can be anything.
                The value being sent by is a string. 
        */
        {
            String value = server.arg("VALUE");
            Serial.print("Received VALUE: ");
            Serial.println(value);
            webSubmittedValue = value.toInt();
            // Here you can add code to handle the received value
        }
        else
        {
            Serial.println("No VALUE argument received");
        }
        server.send(200, "text/plain", "OK");
    }

    void init_html_pages()
    {
        server.on("/", SendHtml);
        // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
        // just parts of the web page
        server.on("/xml", SendXML);
        // this is the handler for the PUT from the web page
        server.on("/submit", getSubmit);

        Serial.println("Starting HTTP server...");
        server.begin();
        Serial.println("HTTP server started (http://localhost:8180)");
    }

};
