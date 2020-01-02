/*
 * HTMLPage.h
 *
 *  Created on: 23 Dec 2019
 *      Author: jp112sdl
 */

/*
 * HTMLPage.h
 *
 *  Created on: 23 Dec 2019
 *      Author: jp112sdl
 */

#ifndef HTML_H_
#define HTML_H_

const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <style>
    body {
      text-align: center;
    }
    .vert {
      margin-bottom: 10%;
    }
    .hori {
      margin-bottom: 0%;
    }
    .range-slider {
      width: 300px;
    }
    .switch {
      position: relative;
      display: inline-block;
      width: 60px;
      height: 34px;
    }
    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }
    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      -webkit-transition: .4s;
      transition: .4s;
    }
    .slider:before {
      position: absolute;
      content: "";
      height: 26px;
      width: 26px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      -webkit-transition: .4s;
      transition: .4s;
    }
    input:checked+.slider {
      background-color: #2196F3;
    }
    input:focus+.slider {
      box-shadow: 0 0 1px #2196F3;
    }
    input:checked+.slider:before {
      -webkit-transform: translateX(26px);
      -ms-transform: translateX(26px);
      transform: translateX(26px);
    }
    .slider.round {
      border-radius: 34px;
    }
    .slider.round:before {
      border-radius: 50%;
    }
  </style>
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
</head>

<body>
  <div>
    <img src="/pic" id="picimg" width="70%">
  </div>
  <div>
    <p>
      <button id="reloadBtn" onclick="location.reload();">Aktualisieren</button>
      <button id="srcBtn" onclick="location.href='?show=video';">Video-Ansicht</button>
      <label class="switch"><input id="flashlight" type="checkbox" onchange="SetFlashlight(this.checked)"><span class="slider round"></span></label>
      <input type="range" min="0" max="180" value="90" class="range-slider" id="servoSlider" onchange="SetServoPos(this.value)" />
    </p>
  </div>
  <script>

    var slider = document.getElementById("servoSlider");
    var picimg = document.getElementById("picimg");
    var reloadBtn = document.getElementById("reloadBtn");
    var srcBtn = document.getElementById("srcBtn");
    var flashlightBtn = document.getElementById("flashlightBtn");

    $.ajaxSetup({
      timeout: 1000
    });

    function ajaxErrorHandler(xhr) {
      console.log('Fehler ' + xhr.status + ' ' + xhr.responseText);
    }

    function GetServoPos() {
      $.get('/get?item=servopos')
        .done(function (data) {
          console.log("GetServoPos = " + data);

          slider.value = data;
        })
        .fail(ajaxErrorHandler);
    }

    function GetFlashlightState() {
      $.get('/get?item=flashlight')
        .done(function (data) {
          flashlight.checked = data === '1';
        })
        .fail(ajaxErrorHandler);
    }

    function SetFlashlight(isOn) {
      console.log('set flashlight', isOn)
      $.post('/set', 'flashlight=' + (isOn ? '1' : '0'))
        .fail(ajaxErrorHandler);
    }

    slider.oninput = function () {
      slider.value = this.value;
    }

    var i = false;
    function SetServoPos(pos) {
      $.post("/set", "servopos=" + pos)
        .fail(ajaxErrorHandler);

      if (i) clearTimeout(i);
      i = setTimeout(function () { 
        document.getElementById("picimg").src = "/pic?" + Date.now(); i = false 
      }, 1500);
    }

    GetServoPos();
    GetFlashlightState();

    var queryStr = window.location.search;
    console.log(queryStr);
    if (queryStr === "?show=video") {
      reloadBtn.style.display = "none";
      // Warum mit FQDN und nicht einfach "/vid" ?
      picimg.src = "http://" + window.location.hostname + ":81/vid";
      srcBtn.innerHTML = "Bild-Ansicht";
      srcBtn.setAttribute("onClick", "location.href='?show=picture';");
    }

    if (queryStr === "?show=picture") {
      reloadBtn.style.display = "inline";
      picimg.src = "/pic";
      srcBtn.innerHTML = "Video-Ansicht";
      srcBtn.setAttribute("onClick", "location.href='?show=video';");
    }

  </script>
</body>

</html>
)=====";


#endif
