/*
The code was referrenced to https://github.com/KrisKasprzak/ESP32_WebPage/
It is based XML to communicate between web page and ESP32. The web page is controlled by JavaScript.
No additional library is required on the web page side.
*/

#include <Arduino.h>
#include <WebServer.h>
// #include <string>
#include "example_funcs.h"
#include "index_src.h"
/*
   #include "../.pio/libdeps/seeed_xiao_esp32s3/lvgl/examples/lv_examples.h"
   Here we should include the lv_examples.h file from the lvgl library. However,
   to make it neater, the include is done in the platformio.ini file using
   build_flags
   */

#define PAGE_SIZE 11234

namespace WebPage
{
    // constexpr size_t XmlBufferSize = 12048;
    // char XML[XmlBufferSize];
    int webSubmittedValue = -1;
    // std::queue<int> key_buffer;

    WebServer server(80);

    void SendHtml()
    {
        // server.send(200, "text/html",  (const char *)PAGE_MAIN);
        std::string optionsString;
        for (size_t i = 0; i < func_count; ++i)
        {
            optionsString += "<option>";
            optionsString += func_names[i];
            optionsString += "</option>";
        }

        std::string page(PAGE_MAIN);
        // Replace the placeholder option with the generated optionsString
        const std::string placeholder = "<option>Placeholder Option 1</option>";
        size_t pos = page.find(placeholder);
        if (pos != std::string::npos)
        {
            page.replace(pos, placeholder.length(), optionsString);
        }
        printf("%s\n", page.c_str());
        server.send(200, "text/html", (const char *)page.c_str());
        // Have to have the (const char *) here otherwise not working
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
        // server.on("/xml", SendXML);
        // this is the handler for the PUT from the web page
        server.on("/submit", getSubmit);

        Serial.println("Starting HTTP server...");
        server.begin();
        Serial.println("HTTP server started (http://localhost:8180)");
    }

};
