
#pragma once

// note R"KEYWORD( html page code )KEYWORD"; 
// again I hate strings, so char is it and this method let's us write naturally

const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8" />
  <title>Line Chart by MCU data</title>
  <style>
    body {
      background-color: #121212;
      color: #e0e0e0;
      font-family: Arial, sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
    }

    .form-container {
      background-color: #1e1e1e;
      padding: 20px 30px;
      border-radius: 8px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.7);
      width: 640px;
    }

    .form-group {
      display: flex;
      align-items: center;
      margin-bottom: 15px;
    }

    .form-group label {
      flex: 1;
      margin-right: 10px;
      font-size: 14px;
    }

    .form-group input,
    .form-textarea {
      flex: 2;
      padding: 8px;
      border: 1px solid #444;
      border-radius: 4px;
      background-color: #2a2a2a;
      color: #f0f0f0;
      font-size: 14px;
      resize: none;
    }

    .form-group input:focus,
    .form-textarea:focus {
      border-color: #6200ea;
      outline: none;
    }

    .form-textarea {
      width: 100%;
      min-height: 80px;
      margin-bottom: 20px;
    }

    .form-header {
      margin-bottom: 20px;
      font-size: 16px;
      text-align: center;
      color: #bbbbbb;
    }


    button {
      width: 100%;
      padding: 10px;
      border: none;
      border-radius: 4px;
      background-color: #6200ea;
      color: #ffffff;
      font-size: 16px;
      cursor: pointer;
      box-shadow: 0 4px #3b0cad;
      transition: all 0.1s ease-in-out;
    }

    button:hover {
      background-color: #7a34f4;
    }

    button:active {
      transform: translateY(2px);
      box-shadow: 0 2px #3b0cad;
    }

    #myPlot {
      flex: 1;
      min-width: 100%;
      min-height: 100%;
      padding: 10px;
    }
  </style>
  <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
</head>

<body onload="process()">
  <div class="form-container">
    <!-- Just a label/title above -->
    <div class="form-header" id="header">Change the pin swtich status
      with the MCU and acquire data. The plot will show the zigzag pattern of the pin status.
    </div>

    <div id="myPlot"></div>

    <!-- Button -->
    <button type="button" id="submitBtn" onclick="submitForm()">Acquire Data</button>
  </div>

  <script>

    function drawPlot(vArray) {
      // Define the data for the line chart
      const trace1 = {
        y: vArray,
        mode: 'lines',
        type: 'scatter',
        name: 'Line 1'
      };

      const data = [trace1];

      const layout = {
        title: 'Line Chart',
        xaxis: {
          title: 'X Axis',
          showgrid: false,
          zeroline: false
        },
        yaxis: {
          title: 'Y Axis',
          showline: false
        }
      }
      // Render the plot inside the div with id 'myPlot'
      Plotly.newPlot('myPlot', data, layout);
    };

    var xmlHttp = new XMLHttpRequest();
    drawPlot([10, 15, 13, 17, 22]);

    function response() {
      var response = xmlHttp.responseXML;
      var vElements = response.getElementsByTagName("v");

      if (vElements.length > 0) {
        var vArray = [];
        for (var i = 0; i < vElements.length; i++) {
          vArray.push(Number(vElements[i].textContent));
        }
        console.log("Numeric array vArray =", vArray);
        drawPlot(vArray);
      } else {
        // No <v> elements found, do nothing or handle accordingly
        console.log("No <v> elements found in XML.");
      }

    }

    function submitForm() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("PUT", "submit?VALUE=" + "1", true);
      console.log("Submitting: " + "1");
      xhttp.send();
    }

    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to 
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process() {

      if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
        xmlHttp.open("PUT", "xml", true);
        xmlHttp.onreadystatechange = response;
        xmlHttp.send(null);
      }
      // you may have to play with this value, big pages need more porcessing time, and hence
      // a longer timeout
      setTimeout("process()", 200);
    }


  </script>

</html>
)=====";