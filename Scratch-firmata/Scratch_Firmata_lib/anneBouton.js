
Javascript avec plus ou moins de fct boutons :D
Anne CASSIM
Aujourd’hui, 16:49Félix VANNESTE

/*

*This program is free software: you can redistribute it and/or modify

*it under the terms of the GNU General Public License as published by

*the Free Software Foundation, either version 3 of the License, or

*(at your option) any later version.

*

*This program is distributed in the hope that it will be useful,

*but WITHOUT ANY WARRANTY; without even the implied warranty of

*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

*GNU General Public License for more details.

*

*You should have received a copy of the GNU General Public License

*along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

 

(function(ext) {

   

    /*==============================================================================

    * VARIABLES

    *============================================================================*/

 

    var PIN_MODE = 0xF4,

        REPORT_DIGITAL = 0xD0,

        REPORT_ANALOG = 0xC0,

        DIGITAL_MESSAGE = 0x90,

        START_SYSEX = 0xF0,

        END_SYSEX = 0xF7,

        QUERY_FIRMWARE = 0x79,

        REPORT_VERSION = 0xF9,

        ANALOG_MESSAGE = 0xE0,

        ANALOG_MAPPING_QUERY = 0x69,

        ANALOG_MAPPING_RESPONSE = 0x6A,

        CAPABILITY_QUERY = 0x6B,

        CAPABILITY_RESPONSE = 0x6C;

 

        MOTOR = 0xA1;

        LED = 0xA2;

        BUZZER = 0xA3;

        BUTTON = 0xA4;

        SENSOR = 0XA5;

 

    var INPUT = 0x00,

        OUTPUT = 0x01,

        ANALOG = 0x02,

        PWM = 0x03,

        SERVO = 0x04,

        SHIFT = 0x05,

        I2C = 0x06,

        ONEWIRE = 0x07,

        STEPPER = 0x08,

        ENCODER = 0x09,

        SERIAL = 0x0A,

        PULLUP = 0x0B,

        IGNORE = 0x7F,

        TOTAL_PIN_MODES = 13;

 

    var LOW = 0,

        HIGH = 1;

 

    var MAX_DATA_BYTES = 4096;

    var MAX_PINS = 128;

 

    var parsingSysex = false,

        waitForData = 0,

        executeMultiByteCommand = 0,

        multiByteChannel = 0,

        sysexBytesRead = 0,

        storedInputData = new Uint8Array(MAX_DATA_BYTES);

 

    var digitalOutputData = new Uint8Array(16),

        digitalInputData = new Uint8Array(16),

        analogInputData = new Uint16Array(16);

 

    var analogChannel = new Uint8Array(MAX_PINS);

    var pinModes = [];

    for (var i = 0; i < TOTAL_PIN_MODES; i++) pinModes[i] = [];

 

    var majorVersion = 0,

        minorVersion = 0;

 

    var connected = false;

    var notifyConnection = false;

    var device = null;

    var inputData = null;

 

    // TEMPORARY WORKAROUND

    // Since _deviceRemoved is not used with Serial devices

    // ping device regularly to check connection

    var pinging = false;

    var pingCount = 0;

    var pinger = null;

 

   //var hwList = new HWList();

   

    var buttonList = ["bouton 1", 22, "bouton 2", 24, "bouton 3", 25, "bouton 4", 23, "bouton 5", 26];

 

    /*==============================================================================

    * INIT FONCTION

    *============================================================================*/

   

    function init() {

 

        for (var i = 0; i < 16; i++) {

            var output = new Uint8Array([REPORT_DIGITAL | i, 0x01]);

            device.send(output.buffer);

        }

 

        queryCapabilities();

 

        // TEMPORARY WORKAROUND

        // Since _deviceRemoved is not used with Serial devices

        // ping device regularly to check connection

        pinger = setInterval(function() {

            if (pinging) {

                if (++pingCount > 6) {

                    clearInterval(pinger);

                    pinger = null;

                    connected = false;

                    if (device) device.close();

                    device = null;

                    return;

                }

            } else {

                if (!device) {

                    clearInterval(pinger);

                    pinger = null;

                    return;

                }

                queryFirmware();

                pinging = true;

            }

        }, 100);

    }

   

    /*==============================================================================

    * SENDING FONCTION

    *============================================================================*/

 

    function processSysexMessage() {

        switch(storedInputData[0]) {

            case CAPABILITY_RESPONSE:

                for (var i = 1, pin = 0; pin < MAX_PINS; pin++) {

                    while (storedInputData[i++] != 0x7F) {

                        pinModes[storedInputData[i-1]].push(pin);

                        i++; //Skip mode resolution

                    }

                    if (i == sysexBytesRead) break;

                }

                queryAnalogMapping();

                break;

            case ANALOG_MAPPING_RESPONSE:

                for (var pin = 0; pin < analogChannel.length; pin++)

                    analogChannel[pin] = 127;

                for (var i = 1; i < sysexBytesRead; i++)

                    analogChannel[i-1] = storedInputData[i];

                for (var pin = 0; pin < analogChannel.length; pin++) {

                    if (analogChannel[pin] != 127) {

                        var out = new Uint8Array([

                            REPORT_ANALOG | analogChannel[pin], 0x01]);

                        device.send(out.buffer);

                    }

                }

                notifyConnection = true;

                setTimeout(function() {

                    notifyConnection = false;

                }, 100);

                break;

            case QUERY_FIRMWARE:

                if (!connected) {

                    clearInterval(poller);

                    poller = null;

                    clearTimeout(watchdog);

                    watchdog = null;

                    connected = true;

                    setTimeout(init, 500); // ADD avant 200

                }

                pinging = false;

                pingCount = 0;

                break;

        }

    }

 

    /*==============================================================================

    * RECEPTION FUNCTION

    *============================================================================*/

   

    function processInput(inputData) {

        for (var i=0; i < inputData.length; i++) {

            if (parsingSysex) {

                if (inputData[i] == END_SYSEX) {

                    parsingSysex = false;

                    processSysexMessage();

                } else {

                    storedInputData[sysexBytesRead++] = inputData[i];

                }

            } else if (waitForData > 0 && inputData[i] < 0x80) {

                storedInputData[--waitForData] = inputData[i];

                if (executeMultiByteCommand !== 0 && waitForData === 0) {

                    switch(executeMultiByteCommand) {

                        case DIGITAL_MESSAGE:

                            console.log("entered processInput");

                            setDigitalInputs(multiByteChannel, (storedInputData[0] << 7) + storedInputData[1]);

                            break;

                        case ANALOG_MESSAGE:

                            setAnalogInput(multiByteChannel, (storedInputData[0] << 7) + storedInputData[1]);

                            break;

                        case REPORT_VERSION:

                            setVersion(storedInputData[1], storedInputData[0]);

                            break;

                    }

                }

            } else {

                if (inputData[i] < 0xF0) {

                    command = inputData[i] & 0xF0;

                    multiByteChannel = inputData[i] & 0x0F;

                } else {

                    command = inputData[i];

                }

                switch(command) {

                    case DIGITAL_MESSAGE:

                    case ANALOG_MESSAGE:

                    case REPORT_VERSION:

                        waitForData = 2;

                        executeMultiByteCommand = command;

                        break;

                   case START_SYSEX:

                        parsingSysex = true;

                        sysexBytesRead = 0;

                        break;

 

                }

            }

        }

    }

 

    /*==============================================================================

    * OTHER FONCTIONS

    *============================================================================*/

    /*

    function HWList() {

            this.devices = [];

 

            this.add = function(dev, pin) {

                var device = this.search(dev);

                if (!device) {

                    device = {name: dev, pin: pin, val: 0};

                    this.devices.push(device);

                } else {

                    device.pin = pin;

                    device.val = 0;

                }

            };

 

            this.search = function(dev) {

                for (var i=0; i<this.devices.length; i++) {

                    if (this.devices[i].name === dev)

                        return this.devices[i];

                }

                return null;

            };

        }

    */

        function hasCapability(pin, mode) {

            if (pinModes[mode].indexOf(pin) > -1)

                return true;

            else

                return false;

        }

 

        function queryFirmware() {

            var output = new Uint8Array([START_SYSEX, QUERY_FIRMWARE, END_SYSEX]);

            device.send(output.buffer);

        }

 

        function queryCapabilities() {

            console.log('Querying ' + device.id + ' capabilities');

            var msg = new Uint8Array([

                START_SYSEX, CAPABILITY_QUERY, END_SYSEX]);

            device.send(msg.buffer);

        }

 

        function queryAnalogMapping() {

           console.log('Querying ' + device.id + ' analog mapping');

            var msg = new Uint8Array([

                START_SYSEX, ANALOG_MAPPING_QUERY, END_SYSEX]);

            device.send(msg.buffer);

        }

 

        function setDigitalInputs(portNum, portData) {

            digitalInputData[portNum] = portData;

        }

 

        function setAnalogInput(pin, val) {

            analogInputData[pin] = val;

        }

 

        function setVersion(major, minor) {

            majorVersion = major;

            minorVersion = minor;

        }

   

    function pinMode(pin, mode) {

        var msg = new Uint8Array([PIN_MODE, pin, mode]);

        console.log(msg);

        device.send(msg.buffer);

    }

 

    function analogRead(pin) {

        if (pin >= 0 && pin < pinModes[ANALOG].length) {

            return Math.round((analogInputData[pin] * 100) / 1023);

        } else {

            var valid = [];

            for (var i = 0; i < pinModes[ANALOG].length; i++)

                valid.push(i);

            console.log('ERROR: valid analog pins are ' + valid.join(', '));

            return;

        }

    }

 

    function digitalRead(pin) {

    if (!hasCapability(pin, INPUT)) {

      console.log('ERROR: valid input pins are ' + pinModes[INPUT].join(', '));

      return;

    }

    pinMode(pin, INPUT);

    return (digitalInputData[pin >> 3] >> (pin & 0x07)) & 0x01;

    }

 

    function analogWrite(pin, val) {

        if (!hasCapability(pin, PWM)) {

            console.log('ERROR: valid PWM pins are ' + pinModes[PWM].join(', '));

            return;

        }

        if (val < 0) val = 0;

        else if (val > 100) val = 100;

        val = Math.round((val / 100) * 255);

        pinMode(pin, PWM);

        var msg = new Uint8Array([

            ANALOG_MESSAGE | (pin & 0x0F),

            val & 0x7F,

            val >> 7]);

        device.send(msg.buffer);

        console.log(msg);

    }

 

    function digitalWrite(pin, val) {

        var portNum = (pin >> 3) & 0x0F;

        if (val == LOW)

            digitalOutputData[portNum] &= ~(1 << (pin & 0x07));

        else

            digitalOutputData[portNum] |= (1 << (pin & 0x07));

        pinMode(pin, OUTPUT);

        var msg = new Uint8Array([

            DIGITAL_MESSAGE | portNum,

            digitalOutputData[portNum] & 0x7F,

            digitalOutputData[portNum] >> 0x07]);

        device.send(msg.buffer);

        console.log(msg);

    }

   

    ext.analogWrite = function(pin, val) {

        analogWrite(pin, val);

    };

 

    ext.digitalWrite = function(pin, val) {

        if (val == menus[lang]['outputs'][0])

            digitalWrite(pin, HIGH);

        else if (val == menus[lang]['outputs'][1])

            digitalWrite(pin, LOW);

    };

 

    ext.analogRead = function(pin) {

        return analogRead(pin);

    };

 

    ext.digitalRead = function(pin) {

        return digitalRead(pin);

    };

 

    ext.whenAnalogRead = function(pin, op, val) {

        if (pin >= 0 && pin < pinModes[ANALOG].length) {

            if (op == '>')

                return analogRead(pin) > val;

            else if (op == '<')

                return analogRead(pin) < val;

            else if (op == '=')

                return analogRead(pin) == val;

            else

                return false;

        }

    };

 

    ext.whenDigitalRead = function(pin, val) {

        if (hasCapability(pin, INPUT)) {

            if (val == menus[lang]['outputs'][0])

                return digitalRead(pin);

            else if (val == menus[lang]['outputs'][1])

                return digitalRead(pin) === false;

        }

    };

   

    ext.connectHW = function(hw, pin) {

        hwList.add(hw, pin);

    };

 

    ext.readInput = function(name) {

        var hw = hwList.search(name);

        if (!hw) return;

        return analogRead(hw.pin);

    };

   

    ext.whenInput = function(name, op, val) {

        var hw = hwList.search(name);

        if (!hw) return;

        if (op == '>')

            return analogRead(hw.pin) > val;

        else if (op == '<')

            return analogRead(hw.pin) < val;

        else if (op == '=')

            return analogRead(hw.pin) == val;

        else

            return false;

    };

   

// ----------------------------------------------------------------------------------------

   

    /*==============================================================================

    * MOTOR FONCTIONS

    *============================================================================*/

       

        function forward(target) {

            var msg = new Uint8Array([START_SYSEX,MOTOR, 1, target, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.forward = function(target){

            target = target * 10; // conversion cm -> mm (fonction de clara prend des mm)

            forward(target);

        }

       

        //----------------------------------------------------------------------------

       

        function backward(target) {

            var msg = new Uint8Array([START_SYSEX,MOTOR, 2, target, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.backward = function(target){

            target = target * 10; // conversion cm -> mm

            backward(target);

        }

       

        //----------------------------------------------------------------------------

       

        function stop() {

            var msg = new Uint8Array([START_SYSEX,MOTOR, 3, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.stop = function(){

            stop();

        }

       

        //----------------------------------------------------------------------------

       

        function turnRight(angle) {

            var msg = new Uint8Array([START_SYSEX,MOTOR, 4, angle, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.turnRight = function(angle){

            turnRight(angle);

        }

       

        //----------------------------------------------------------------------------

       

        function turnLeft(angle) {

            var msg = new Uint8Array([START_SYSEX,MOTOR, 5, angle, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.turnLeft = function(angle){

            turnLeft(angle);

        }

       

        //----------------------------------------------------------------------------

       

        function drawRectangle(longueur, largeur) {

            var msg = new Uint8Array([START_SYSEX,MOTOR, 6, longueur, largeur, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.drawRectangle = function(longueur, largeur){

            drawRectangle(longueur, largeur);

        }

       

        //----------------------------------------------------------------------------

       

        function drawTriangle(cote) {

            var msg = new Uint8Array([START_SYSEX,MOTOR, 7, cote, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.drawTriangle = function(cote){

            drawTriangle(cote);

        }

       

    /*==============================================================================

    * LED FUNCTIONS

    *============================================================================*/

 

        function setColorUnit(numLed, R, G, B) {

            var msg = new Uint8Array([START_SYSEX,LED, 1, numLed, R, G, B,END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.setColorUnit = function(numLed, R, G, B){

            var threshold = 255;

            if (R > threshold) R = 255;

            if (R < 0) R = 0;

            if (G > threshold) G = 255;

            if (G < 0) G = 0;

            if (B > threshold) B = 255;

            if (B < 0) B = 0;      

            setColorUnit(numLed, R, G, B);

        }

 

        //----------------------------------------------------------------------------

 

        function setColorAll(R,G,B){

            var msg = new Uint8Array([START_SYSEX,LED, 2, R, G, B, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.setColorAll = function(R,G,B){

            var threshold = 255;

            if (R > threshold) R = 255;

            if (R < 0) R = 0;

            if (G > threshold) G = 255;

            if (G < 0) G = 0;

            if (B > threshold) B = 255;

            if (B < 0) B = 0; 

            setColorAll(R,G,B);

        }

 

        //----------------------------------------------------------------------------

 

        function setColor(nbrColor){

            var msg = new Uint8Array([START_SYSEX,LED, 3, nbrColor, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.setColor = function(couleur){

            var nbrColor = 0;

            switch(couleur) {

                case 'blanc':

                    nbrColor = 0

                    break;

                case 'rouge':

                    nbrColor = 1

                    break;

                case 'vert':

                    nbrColor = 3

                    break;

                case 'bleu':

                    nbrColor = 2

                    break;

                case 'turquoise':

                    nbrColor = 4

                    break;

                case 'orange':

                    nbrColor = 5

                    break;

                case 'gris':

                    nbrColor = 6

                    break;

                case 'jaune':

                    nbrColor = 7

                    break;

                case 'magenta':

                    nbrColor = 8

                    break;

                case 'violet':

                    nbrColor = 9

                    break;

                default:

                    nbrColor = 0

            }

            console.log(nbrColor);

            setColor(nbrColor);

        }

 

        //----------------------------------------------------------------------------

 

        function ledOnOff(etat){

            var msg = new Uint8Array([START_SYSEX,LED, 4, etat, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.ledOnOff = function(val){

            if (val == "On") etat = 1;

            if (val == "Off") etat = 0;

            ledOnOff(etat);

        }

 

        //----------------------------------------------------------------------------

 

        function expression(emotion) {

            var msg = new Uint8Array([START_SYSEX,LED, 5, emotion,END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.expression = function(emotion) {

            expression(emotion);

        }

       

       

    /*==============================================================================

    * BUZZER FONCTIONS

    *============================================================================*/

   

    function playSon(frequency) {

            var msg = new Uint8Array([START_SYSEX, BUZZER, 1, frequency, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.playSon = function(frequency)  {

            playSon(frequency);

        }

 

        //----------------------------------------------------------------------------

 

        function playNote(octave, newNote) {

            var msg = new Uint8Array([START_SYSEX, BUZZER, 2, octave, newNote, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.playNote = function(octave,note)   {

            var newNote = 100;

            switch (note){

                case 'do': newNote = 1;

                    break;

                case 'do#': newNote = 2;

                    break;

                case 're': newNote = 3;

                    break;

                case 're#': newNote = 4;

                    break;

                case 'mi': newNote = 5;

                    break;

                case 'fa': newNote = 6;

                    break;

                case 'fa#': newNote = 7;

                    break;

                case 'sol': newNote = 8;

                    break;

                case 'sol#': newNote = 9;

                    break;  

                case 'la': newNote = 10;

                    break;

                case 'la#': newNote = 11;

                    break;

                case 'si': newNote = 12;

                    break;

                default:

                    break;              

            }

            playNote(octave,newNote);

        }

 

        //----------------------------------------------------------------------------

 

        function playSonDelay(frequency, time) {

            var msg = new Uint8Array([START_SYSEX, BUZZER, 3, frequency, time, END_SYSEX]);  // YA PAS LA FONCTION 3 ??

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.playSonDelay = function(frequency,time)    {

            var timeBis = 1;

            if (time == 0) time = timeBis;

            playSonDelay(frequency,time);

        }

 

        //----------------------------------------------------------------------------

 

        function setDelayRythme(time) {

            var msg = new Uint8Array([START_SYSEX, BUZZER, 4, time, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.setDelayRythme = function(time)  {

            var timeBis = 1;

            if (time == 0) time = timeBis;

            setDelayRythme(time);

        }

 

        //----------------------------------------------------------------------------

 

        function setDelayAttente(time) {

            var msg = new Uint8Array([START_SYSEX, BUZZER, 5, time, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.setDelayAttente = function(time) {

            var timeBis = 1;

            if (time == 0) time = timeBis;

            setDelayAttente(time);

        }

 

        //----------------------------------------------------------------------------

 

        function playMelody(num) {

            var msg = new Uint8Array([START_SYSEX, BUZZER, 6, num, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.playMelody = function (numMelodie) {

            if (numMelodie < 0) numMelodie = 1;

            if (numMelodie > 10) numMelodie = 10;

            buzzer(numMelodie);

        }

 

        //----------------------------------------------------------------------------

 

        function buzzerOnOff(state) {

            var msg = new Uint8Array([START_SYSEX, BUZZER, 7, state, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.buzzerOnOff = function(state){

            if (state == "On") etat = 1;

            if (state == "Off") etat = 0;

            buzzerOnOff(etat);

        }

 

        //----------------------------------------------------------------------------

 

        function buzzerOnOffDelay(state,time) {

            var msg = new Uint8Array([START_SYSEX, BUZZER, 8, state, time, END_SYSEX]);

            console.log(msg);

            device.send(msg.buffer);

        }

 

        ext.buzzerOnOffDelay = function(state,time){

            var timeBis = 1;

            if (time == 0) time = timeBis;

            if (state == "On") etat = 1;

            if (state == "Off") etat = 0;

            buzzerOnOffDelay(etat,time);

        }

       

    /*==============================================================================

    * BUTTON FONCTIONS

    *============================================================================*/

       

        ext.whenButton = function(bouton, etatBouton){

        var indice = buttonList.indexOf(bouton);

        var pin = buttonList[indice+1];

        if (!pin) return;

        if (etatBouton === 'pressé') {

                                            console.log("entered bouton pressé");

                                            console.log(digitalRead(pin));

            if (digitalRead(pin) == 0)

                                                           return true;

                                            else return false;

                              }

        else if (etatBouton === 'relaché') {

                                            console.log("enteret bouton relaché");

                                            console.log(digitalRead(pin));

            if (digitalRead(pin) == 1)

                                                           return true;

                                            else return false;

                              }

        }

     

       //----------------------------------------------------------------------------

 

        ext.buttonPressed = function(bouton) {

        var indice = buttonList.indexOf(bouton);

        var pin = buttonList[indice+1];

        if (!pin) return;

        return !digitalRead(pin);

        }

       

        ext.lecture = function(bouton) {

                              var indice = buttonList.indexOf(bouton);

                              var pin = buttonList[indice+1];

        lecture(pin);

                              }

                             

                              function lecture(pin) {

                              console.log(digitalRead(pin));

                              }

                             

    /*==============================================================================

    * SENSOR FONCTIONS

    *============================================================================*/

   

    ext.detection = function() {

        digitalRead(); // mettre la pin des sensors

    }

   

    /*==============================================================================

    * UTILITY FONCTIONS

    *============================================================================*/

   

    ext.whenConnected = function() {

        if (notifyConnection) return true;

        return false;

    };

 

    ext.mapValues = function(val, aMin, aMax, bMin, bMax) {

            var output = (((bMax - bMin) * (val - aMin)) / (aMax - aMin)) + bMin;

            return Math.round(output);

    };

 

    ext._getStatus = function() {

        if (!connected)

            return { status:1, msg:'Disconnected' };

        else

            return { status:2, msg:'Connected' };

    };

 

    ext._deviceRemoved = function(dev) {

        console.log('Device removed');

        // Not currently implemented with serial devices

    };

 

    var potentialDevices = [];

    ext._deviceConnected = function(dev) {

        potentialDevices.push(dev);

        if (!device)

            tryNextDevice();

    };

 

    var poller = null;

    var watchdog = null;

    function tryNextDevice() {

        device = potentialDevices.shift();

        if (!device) return;

 

        device.open({ stopBits: 0, bitRate: 57600, ctsFlowControl: 0 });

        console.log('Attempting connection with ' + device.id);

        device.set_receive_handler(function(data) {

            var inputData = new Uint8Array(data);

            processInput(inputData);

        });

 

        poller = setInterval(function() {

            queryFirmware();

        }, 1000);

 

        watchdog = setTimeout(function() {

            clearInterval(poller);

            poller = null;

            device.set_receive_handler(null);

            device.close();

            device = null;

            tryNextDevice();

        }, 5000);

    }

 

    ext._shutdown = function() {

        // TODO: Bring all pins down

        if (device) device.close();

        if (poller) clearInterval(poller);

        device = null;

    };

 

    // Check for GET param 'lang'

    var paramString = window.location.search.replace(/^\?|\/$/g, '');

    var vars = paramString.split("&");

    var lang = 'fr';

 

    /*==============================================================================

    * BLOCKS

    *============================================================================*/

           

    var blocks = {

        fr : [

 

            // MOTOR BLOCKS

                        [' ','Avancer de %n cm'                                                                 , 'forward'         , 20],          // à valider

                        [' ','Reculer de %n cm'                                                                 , 'backward'        ,  20],         // à valider

                        [' ', 'STOP'                                                                            , 'stop'],                          // à valider

                        [' ','Tourner vers la droite de %n degrés'                                              , 'turnRight'       , 90],          // à valider

                        [' ','Tourner vers la gauche de %n degrés'                                              , 'turnLeft'        , 90],          // à valider

                        [' ','Dessiner un rectangle de longueur %m.longueur cm et de largeur %m.largeur cm'     , 'drawRectangle'   , '15', '10'],  // à valider

                        [' ','Dessiner un triangle de côté %m.largeur cm'                                       , 'drawTriangle'    , 10],          // à valider

                        [' '],

            // LED BLOCKS

                        [' ','Régler la LED %m.numLed à R: %n G: %n B: %n'                                      ,'setColorUnit'    , 1],           // validé

                        [' ','Allumer les LED à R: %n G: %n B: %n'                                              ,'setColorAll'     ,'0','0','0'],  //validé

                        [' ','Etat led : %m.onOff'                                                              ,'ledOnOff'         ,'Off'],        // validé

                        [' ','Choisir couleur led : %m.couleur'                                                 ,'setColor'         ,'null'],       // à valider

                        [' ','Mettre expression faciale n°%m.emotion'                                           ,'expression'      , 1],           // à valider

                        [' '],

            // BUZZER BLOCKS

                        [' ','Rythme du buzzer : %n s'                                                          ,'setDelayRythme'   ,'1'],          // validé

                        [' ','Buzzer : %m.onOff'                                                                ,'buzzerOnOff'      ,'Off'],        // validé

                        [' ','Buzzer : %m.onOff pendant %n sc'                                                  ,'buzzerOnOffDelay' ,'Off',1],      // validé

                        [' ','Fréquence : %n Hz durée : %n'                                                     ,'playSonDelay'     ,'500','1'],   // à valider

                        [' ','Jouer la mélodie %m.melodie'                                                      ,'playMelody'      , 1],           // à valider

                        [' ','Attente buzzer : %n s'                                                            ,'setDelayAttente'  ,'1'],          // à valider

                        [' ','Octave : %m.octave note : %m.note'                                                ,'playNote'         ,'4','do'],     // à valider

                        [' '],

            // BUTTONS

                        ['h','Quand le %m.bouton est %m.etatBouton'                                             , 'whenButton'      , 'bouton 1', 'pressé'],    // à valider

                        ['b','%m.bouton pressé ?'                                                               , 'buttonPressed'   , 'bouton 1'],              // à valider

                                                                                         [' ','Lecture de létat du bouton %m.bouton'                                                                                                                                             , 'lecture'                                          , 'bouton 1'],

                                                                                         [' '],

                                                                                        

            // SENSOR

                        ['b','Obstacle détecté?'                                                                , 'detection'],                                 // à valider

                        [' '],

            // OTHER

                        ['h',"Quand l'appareil est connecté"                                                    , 'whenConnected']

        ]

    };

 

    var menus = {

        fr: {      

            // MOTOR

            longueur: [15, 25, 35],

            largeur: [10, 20, 30],

           

            // LED

            numLed: ['1','2','3','4','5','6'],

            couleur: ['blanc','rouge','vert','bleu','turquoise','orange','gris','jaune','magenta','violet'],

            emotion: [1,2,3],

           

            

            //  BUZZER

            melodie: [1,2,3], // même si y'en a qu'une

            octave: [1,2,3,4,5,6,7],

            note: ['do','do#','re','re#','mi','fa','fa#','sol','sol#','la','la#','si'],

       

            // BUTTONS

            bouton: ['bouton 1', 'bouton 2', 'bouton 3', 'bouton 4', 'bouton 5'],

            etatBouton: ['pressé', 'relaché'],

            onOff: ['On', 'Off']

        }

    };

 

    var descriptor = {

        blocks: blocks[lang],

        menus: menus[lang],

        url: ''

    };

 

    ScratchExtensions.register('Librairie', descriptor, ext, {type:'serial'});

 

})({});

 
