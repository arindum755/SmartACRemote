const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>Smart AC Remote</title>
    <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate">
    <meta http-equiv="Pragma" content="no-cache">
    <meta http-equiv="Expires" content="0">
    <meta name="mobile-web-app-capable" content="yes">
    <meta name="theme-color" content="#2b5876">
    <link rel="icon" sizes="128x128" href="https://arindum755.github.io/SmartACRemote/remote/favicon.png">
    <link rel="icon" type="image/png" href="https://arindum755.github.io/SmartACRemote/remote/favicon.png" />
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" />
    <style>
        body {
            background: linear-gradient(135deg, #2b5876, #4e4376);
            margin: 0;
            padding: 0;
            font-family: Arial, sans-serif;
            color: #ffffff;
        }

        .container {
            text-align: center;
        }

        .counter {
            font-size: 24px;
            margin-bottom: 20px;
        }

        .button {
            padding: 6px 15px;
            font-size: 18px;
            cursor: pointer;
        }

        #gauge-container {
            position: relative;
            width: 200px;
            height: 150px;
            margin: 0 auto;
        }

        #gauge {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
        }

        .mode-buttons {
            margin-top: 20px;
        }

            .mode-buttons .button.active {
                background-color: #3F51B5;
            }

        .additional-buttons {
            display: flex;
            justify-content: center;
            margin-top: 20px;
        }

            .additional-buttons .button.active {
                background-color: #3F51B5;
            }

        .toggle-button {
            margin-bottom: 20px;
        }

        .creator {
            margin-top: 30px;
            margin-bottom: 30px;
        }

        .btn-circle {
            border-radius: 50%;
            width: 50px;
            height: 50px;
            font-size: 14px;
            color: black;
            display: flex;
            justify-content: center;
            align-items: center;
        }
    </style>
    <script src="https://cdn.jsdelivr.net/npm/gaugeJS/dist/gauge.min.js"></script>
    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
</head>
<body>
    <div class="container">
        <h1 class="mt-3"></h1>
        <div class="toggle-button mt-4">
            <button class="btn btn-light button" onclick="togglePower(); triggerHapticFeedback();">On/Off</button>
        </div>
        <div id="gauge-container">
            <canvas id="gauge"></canvas>
        </div>
        <div class="selected-values">
            <span class="mr-2"><i class="fas fa-cogs"></i> <span id="selectedModeValue" class="small">Auto</span></span>
            <span><i class="fas fa-fan"></i> <span id="selectedFanSpeedValue" class="small">0</span></span>
        </div>
        <div class="counter">17&deg;C</div>
        <button class="btn btn-light button" onclick="decrement(); triggerHapticFeedback();" disabled>&darr;</button>
        <button class="btn btn-light button" onclick="increment(); triggerHapticFeedback();" disabled>&uarr;</button>
        <div class="mode-buttons">
            <button class="btn btn-light button" onclick="setMode('Auto'); triggerHapticFeedback();" disabled>Auto</button>
            <button class="btn btn-light button" onclick="setMode('Cool'); triggerHapticFeedback();" disabled>Cool</button>
            <button class="btn btn-light button" onclick="setMode('Dry'); triggerHapticFeedback();" disabled>Dry</button>
            <button class="btn btn-light button" onclick="setMode('Fan'); triggerHapticFeedback();" disabled>Fan</button>
        </div>
        <div class="additional-buttons">
            <button id="swingButton" class="btn btn-light btn-circle mr-2 button" onclick="triggerHapticFeedback();" disabled>Swing</button>
            <button id="stepButton" class="btn btn-light btn-circle mr-2 button" onclick="triggerHapticFeedback();" disabled>Step</button>
            <button id="ledButton" class="btn btn-light btn-circle mr-2 button" onclick="triggerHapticFeedback();" disabled>Led</button>
            <button id="turboButton" class="btn btn-light btn-circle button" onclick="triggerHapticFeedback();" disabled>Turbo</button>
        </div>
        <div class="additional-buttons">
            <button class="btn btn-light button mr-2" onclick="decrementFanSpeed(); triggerHapticFeedback();" disabled>Fan &darr;</button>
            <button class="btn btn-light button" onclick="incrementFanSpeed(); triggerHapticFeedback();" disabled>Fan &uarr;</button>
        </div>
        <div class="creator">
            <a href="https://github.com/arindum755/SmartACRemote" target="_blank" style="text-decoration: none;color: #fff;">Created By Arindum</a>
        </div>
    </div>
    <script>

        var arduinoIp;
        var remotename;
        var powerOn;
        var temperature;
        var selectedMode;
        var fanSpeed = 0;
        var counter = 17;

        function getSearchParams(k){
            var p={};
            location.search.replace(/[?&]+([^=&]+)=([^&]*)/gi,function(s,k,v){p[k]=v})
            return k?p[k]:p;
        }

        $( document ).ready(function() {
            remotename = getSearchParams("remote");
            if(remotename) {
                $("h1").text(decodeURI(remotename));
            }
            arduinoIp = window.location.host;
            if(arduinoIp){
                getSetData();
            } else {
                $("h1").text("AC Remote");
            }
        });

        function getSetData(){
            $.ajax("http://" + arduinoIp + "/getdata")
            .done(function(msg) {
                powerOn = parseInt(msg["isOn"]) === 1 ? true : false;    
                selectedMode = msg["isMode"];                            
                fanSpeed = msg["isFan"];                                 
                temperature = msg["isTemp"];                             
                updateFanSpeed(msg["isFan"]);                            
                document.getElementById('selectedModeValue').textContent = selectedMode; 
                PowerStatus();
            });
        }

        var counterElement = document.querySelector('.counter');
        var gauge = new Gauge(document.getElementById("gauge")).setOptions({
            angle: 0,
            lineWidth: 0.3,
            radiusScale: 0.9,
            pointer: {
                length: 0.5,
                strokeWidth: 0.03,
                color: '#000000'
            },
            limitMax: false,
            limitMin: false,
            strokeColor: '#E0E0E0',
            generateGradient: true,
            highDpiSupport: true,
            staticLabels: {
                font: "10px sans-serif",
                labels: [17, 20, 23, 26, 29, 30],
                color: "#FFFFFF",
                fractionDigits: 0
            },
            staticZones: [
              { strokeStyle: "#84CE79", min: 17, max: 20 },
              { strokeStyle: "#6FC662", min: 20, max: 23 },
              { strokeStyle: "#5BBE4C", min: 23, max: 26 },
              { strokeStyle: "#46B636", min: 26, max: 29 },
              { strokeStyle: "#32ae20", min: 29, max: 30 }
            ]
        });
        gauge.maxValue = 30;
        gauge.setMinValue(17);
        gauge.animationSpeed = 32;
        gauge.set(counter);

        var decrementButton = document.querySelector('.button[onclick="decrement(); triggerHapticFeedback();"]');
        var incrementButton = document.querySelector('.button[onclick="increment(); triggerHapticFeedback();"]');
        var modeButtons = document.querySelectorAll('.mode-buttons .button');
        var additionalButtons = document.querySelectorAll('.additional-buttons .button');

        function togglePower() {
            powerOn = !powerOn;
            if (powerOn) {
                enableControls();
                gauge.set(counter);
            } else {
                disableControls();
                gauge.set(17);
            }
            Power(powerOn);
        }

        function PowerStatus()
        {
            if (powerOn) {
                enableControls();
                gauge.set(parseInt(temperature));
                counter = parseInt(temperature);
                counterElement.textContent = counter + String.fromCodePoint(176) + 'C';
            } else {
                disableControls();
                gauge.set(17);
            }
        }

        function enableControls() {
            decrementButton.disabled = false;
            incrementButton.disabled = false;
            modeButtons.forEach(button => button.disabled = false);
            additionalButtons.forEach(button => button.disabled = false);
        }

        function disableControls() {
            decrementButton.disabled = true;
            incrementButton.disabled = true;
            modeButtons.forEach(button => button.disabled = true);
            additionalButtons.forEach(button => button.disabled = true);
        }

        function increment() {
            if (powerOn && counter < 30) {
                counter++;
                counterElement.textContent = counter + String.fromCodePoint(176) + 'C';
                gauge.set(counter);
                Temperature(counter);
            }
        }

        function decrement() {
            if (powerOn && counter > 17) {
                counter--;
                counterElement.textContent = counter + String.fromCodePoint(176) + 'C';
                gauge.set(counter);
                Temperature(counter);
            }
        }

        function setMode(mode) {
            if (powerOn) {
                Mode(mode);
                selectedMode = mode; 
                document.getElementById('selectedModeValue').textContent = mode;
                
            }
        }

        function incrementFanSpeed() {
            if (powerOn && fanSpeed < 3 && selectedMode != "Auto") {
                fanSpeed++;
                Fan(fanSpeed);
                updateFanSpeed(fanSpeed);
            }
        }

        function decrementFanSpeed() {
            if (powerOn && fanSpeed > 0 && selectedMode != "Auto") {
                fanSpeed--;
                Fan(fanSpeed);
                updateFanSpeed(fanSpeed);
            }
        }

        function updateFanSpeed(speed) {
            fanSpeed = speed;
            document.getElementById('selectedFanSpeedValue').textContent = speed;
        }

        document.getElementById("swingButton").addEventListener("click", function() {            
            Extra("Swing");
        });

        document.getElementById("stepButton").addEventListener("click", function() {            
            Extra("Step");
        });

        document.getElementById("ledButton").addEventListener("click", function() {            
            Extra("Led");
        });

        document.getElementById("turboButton").addEventListener("click", function() {            
            if(selectedMode != "Auto")
            {Extra("Turbo");}
        });
        
        function triggerHapticFeedback() {
            if ("vibrate" in navigator) {
                navigator.vibrate(50); 
            }
        }

        function Power(powerOn)
        {
            
            var xhr = new XMLHttpRequest();
            
            var url = "http://" + arduinoIp + "/togglePower";
            var params = "power=" + powerOn;
            
            xhr.open("GET", url + "?" + params, true);
            
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        
                        var response = xhr.responseText;
                        console.log("Response: " + response);
                        PowerStatus();
                    } else {
                        console.log("Error: " + xhr.status);
                    }
                }
            };
            xhr.send();
        }

        function Mode(modes)
        {
            var xhr = new XMLHttpRequest();
            var url = "http://" + arduinoIp + "/toggleMode";
            var params = "mode=" + modes;
            xhr.open("GET", url + "?" + params, true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var response = xhr.responseText;
                        console.log("Response: " + response);
                    } else {
                        console.log("Error: " + xhr.status);
                    }
                }
            };
            xhr.send();
        }

        function Fan(speed)
        {
            var xhr = new XMLHttpRequest();
            var url = "http://" + arduinoIp + "/toggleFan";
            var params = "fan=" + speed;
            xhr.open("GET", url + "?" + params, true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var response = xhr.responseText;
                        console.log("Response: " + response);
                    } else {
                        console.log("Error: " + xhr.status);
                    }
                }
            };
            xhr.send();
        }

        function Temperature(temp)
        {
            var xhr = new XMLHttpRequest();
            var url = "http://" + arduinoIp + "/toggleTemp";
            var params = "temp=" + temp;
            xhr.open("GET", url + "?" + params, true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var response = xhr.responseText;
                        console.log("Response: " + response);
                    } else {
                        console.log("Error: " + xhr.status);
                    }
                }
            };
            xhr.send();
        }

        function Extra(extra)
        {
            var xhr = new XMLHttpRequest();
            var url = "http://" + arduinoIp + "/toggleExtra";
            var params = "extra=" + extra;
            xhr.open("GET", url + "?" + params, true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        var response = xhr.responseText;
                        console.log("Response: " + response);
                    } else {
                        console.log("Error: " + xhr.status);
                    }
                }
            };
            xhr.send();
        }

    </script>
</body>
</html>
)=====";
