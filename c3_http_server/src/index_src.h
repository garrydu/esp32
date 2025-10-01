
#pragma once

// note R"KEYWORD( html page code )KEYWORD"; 
// again I hate strings, so char is it and this method let's us write naturally

const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Dark Mode Form</title>
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
      width: 320px;
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
  </style>
</head>

<body onload="process()">
  <div class="form-container">
    <!-- Just a label/title above -->
    <div class="form-header" id="header">Please fill in your details below</div>


    <!-- Three labeled text inputs -->
    <div class="form-group">
      <label for="input1" id="label1">Field 1: 0</label>
      <input type="range" id="slider1" min="0" max="255" value="0" width="0%"
        oninput="UpdateSlider(this.value, 'label1')" />
    </div>
    <div class="form-group">
      <label for="input2" id="label2">Field 2: 0</label>
      <input type="range" id="slider2" min="0" max="255" value="0" width="0%"
        oninput="UpdateSlider(this.value, 'label2')" />
    </div>
    <div class="form-group">
      <label for="input3" id="label3">Field 3: 0</label>
      <input type="range" id="slider3" min="0" max="255" value="0" width="0%"
        oninput="UpdateSlider(this.value, 'label3')" />
    </div>

    <!-- Button -->
    <button type="button" id="submitBtn" onclick="submitForm()">Submit Sum</button>
  </div>
</body>
<script type="text/javascript">
  var xmlHttp = new XMLHttpRequest();
  var currentFocus = -1;

  function response() {
    var sliders = ["slider1", "slider2", "slider3"];
    if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
      var response = xmlHttp.responseXML;
      // You can process the response here if needed
      console.log("Response from server: " + response);
      const v = response.getElementsByTagName("v")[0].childNodes[0].nodeValue;
      console.log("v=" + v);
      if (v & 1) {
        submitForm();
      } else if (v & 2) {
        if (currentFocus > -1) {
          const currentSliderValue = document.getElementById(sliders[currentFocus]).value;
          document.getElementById(sliders[currentFocus]).value = (parseInt(currentSliderValue) + 10) % 256;
          UpdateSlider(document.getElementById(sliders[currentFocus]).value,
            sliders[currentFocus].replace("slider", "label"));
        }
      } else if (v & 4) {
        if (currentFocus > -1) {
          const currentSliderValue = document.getElementById(sliders[currentFocus]).value;
          document.getElementById(sliders[currentFocus]).value = (parseInt(currentSliderValue) + 246) % 256;
          UpdateSlider(document.getElementById(sliders[currentFocus]).value,
            sliders[currentFocus].replace("slider", "label"));
        }
      } else if (v & 8) {
        if (currentFocus == -1) currentFocus = 0;
        else {
          document.getElementById(sliders[currentFocus]).style.outline = "none"; // remove outline
          currentFocus = (currentFocus + 1) % sliders.length;
        }
        const slider = document.getElementById(sliders[currentFocus]);
        slider.style.outline = "3px solid #6200ea"; // purple outline
      } else if (v & 16) {
        if (currentFocus == -1) currentFocus = 2;
        else {
          document.getElementById(sliders[currentFocus]).style.outline = "none";
          currentFocus = (currentFocus + 2) % sliders.length;
        }
        const slider = document.getElementById(sliders[currentFocus]);
        slider.style.outline = "3px solid #6200ea"; // purple outline
      }
    }
  }

  function UpdateSlider(val, labelid) {
    document.getElementById(labelid).innerHTML = "Field " + labelid.charAt(labelid.length - 1) + ": " + val;
  }

  function sumInputs() {
    // Get input elements by ID
    const input1 = document.getElementById("slider1").value.trim();
    const input2 = document.getElementById("slider2").value.trim();
    const input3 = document.getElementById("slider3").value.trim();

    // Convert to numbers
    const num1 = parseFloat(input1);
    const num2 = parseFloat(input2);
    const num3 = parseFloat(input3);

    // Check if any value is NaN (invalid)
    if (isNaN(num1) || isNaN(num2) || isNaN(num3)) {
      return "invalid value";
    }

    // Sum values and convert to string
    const sum = num1 + num2 + num3;
    return sum.toString();
  }

  function submitForm() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("PUT", "submit?VALUE=" + sumInputs(), true);
    console.log("Submitting: " + sumInputs());
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