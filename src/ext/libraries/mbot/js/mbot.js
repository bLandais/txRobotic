// mBot.js

(function(ext) {
    var device = null;
    var _rxBuf = [];

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

	var MOTOR = 0xA1,
		LED = 0xA2,
        BUZZER = 0xA3,
        EMOTION = 0xA4,
		BUTTON = 0xA5,
		SENSOR = 0xA6;

    // Sensor states:
    var ports = {
        Port1: 1,
        Port2: 2,
        Port3: 3,
        Port4: 4,
        Port5: 5,
        Port6: 6,
        Port7: 7,
        Port8: 8,
		M1:9,
		M2:10,
		'led on board':7,
		'light sensor on board':6
    };
	var slots = {
		Slot1:1,
		Slot2:2
	};
	var switchStatus = {
		On:1,
		Off:0
	};
	var shutterStatus = {
		Press:1,
		Release:0,
		'Focus On':3,
		'Focus Off':2
	};
	var button_keys = {
		"key1":1,
		"key2":2,
		"key3":3,
		"key4":4
	};
	var tones ={"B0":31,"C1":33,"D1":37,"E1":41,"F1":44,"G1":49,"A1":55,"B1":62,
			"C2":65,"D2":73,"E2":82,"F2":87,"G2":98,"A2":110,"B2":123,
			"C3":131,"D3":147,"E3":165,"F3":175,"G3":196,"A3":220,"B3":247,
			"C4":262,"D4":294,"E4":330,"F4":349,"G4":392,"A4":440,"B4":494,
			"C5":523,"D5":587,"E5":659,"F5":698,"G5":784,"A5":880,"B5":988,
			"C6":1047,"D6":1175,"E6":1319,"F6":1397,"G6":1568,"A6":1760,"B6":1976,
			"C7":2093,"D7":2349,"E7":2637,"F7":2794,"G7":3136,"A7":3520,"B7":3951,
	"C8":4186,"D8":4699};
	var beats = {"Half":500,"Quarter":250,"Eighth":125,"Whole":1000,"Double":2000,"Zero":0};
	var ircodes = {	"A":69,
		"B":70,
		"C":71,
		"D":68,
		"E":67,
		"F":13,
		"↑":64,
		"↓":25,
		"←":7,
		"→":9,
		"Setting":21,
		"R0":22,
		"R1":12,
		"R2":24,
		"R3":94,
		"R4":8,
		"R5":28,
		"R6":90,
		"R7":66,
		"R8":82,
		"R9":74};
	var axis = {
		'X-Axis':1,
		'Y-Axis':2,
		'Z-Axis':3
	}
    var inputs = {
        slider: 0,
        light: 0,
        sound: 0,
        button: 0,
        'resistance-A': 0,
        'resistance-B': 0,
        'resistance-C': 0,
        'resistance-D': 0
    };
	var values = {};
	var indexs = [];
	var startTimer = 0;
	var versionIndex = 0xFA;
	var responsePreprocessor = {};
    ext.resetAll = function(){
    	// Cette fonction est appellée lorsque l'on clique sur le flag vert.
    };
	ext.runArduino = function(){
		responseValue();
	};
	
	function queryFirmware() {
    	device.send([START_SYSEX, QUERY_FIRMWARE, END_SYSEX]);
	}
	
	// ============================ LED Functions ================================
	
	function setColorUnit(numLed, R, G, B) {
    	var msg = [START_SYSEX,LED, 1, numLed, R, G, B,END_SYSEX];
        device.send(msg);
    }

    ext.setColorUnit = function(numLed, R, G, B){
    	var threshold = 255;
        if (R > threshold) R = threshold;
        if (R < 0) R = 0;
        if (G > threshold) G = threshold;
        if (G < 0) G = 0;
        if (B > threshold) B = threshold;
        if (B < 0) B = 0;       
        setColorUnit(numLed, R, G, B); 
    }
    
     //----------------------------------------------------------------------------

	function setColorAll(R,G,B){
    	device.send([START_SYSEX, LED, 2, R, G, B, END_SYSEX]);
    }

	ext.setColorAll = function(R, G, B){
        var threshold = 255;
        if (R > threshold) R = threshold;
        if (R < 0) R = 0;
        if (G > threshold) G = threshold;
        if (G < 0) G = 0;
        if (B > threshold) B = threshold;
        if (B < 0) B = 0;  
    	setColorAll(R, G, B);
	}
    
    //----------------------------------------------------------------------------

    function setColor(nbrColor) {
        device.send([START_SYSEX, LED, 3, nbrColor, END_SYSEX]);
	}

	ext.setColor = function(couleur){
        var nbrColor = 0;
		switch(couleur) {
            case "blanc":
                nbrColor = 0
            	break;
            case "rouge":
                nbrColor = 1
            	break;
            case "bleu":
                nbrColor = 2
            	break;
            case "vert":
                nbrColor = 3
            	break;
            case "turquoise":
                nbrColor = 4
            	break;
            case "orange":
                nbrColor = 5
            	break;
            case "gris":
                nbrColor = 6
            	break;
			case "jaune":
                nbrColor = 7
            	break;
			case "magenta":
                nbrColor = 8
            	break;
			case "violet":
                nbrColor = 9
            	break;
            default:
        		nbrColor = 0
        }
    	setColor(nbrColor);
    }
    
    // ------------------------------------------------------
    
	function ledOnOff(etat){
		device.send([START_SYSEX,LED, 4, etat, END_SYSEX]);
	}

    ext.ledOnOff = function(val){
    	if (val == "On") etat = 1;
    	if (val == "Off") etat = 0;
    	ledOnOff(etat);
    }
    
    
    // ========================== END LED functions =============================
	
	/*==============================================================================
	* BUZZER FUNCTIONS
	*=============================================================================*/
	
	//----------------------------------------------------------------------------

	function setDelayRythme(time) {
		device.send([START_SYSEX, BUZZER,5, time, END_SYSEX]);
	}

	ext.setDelayRythme = function(time)  {
    	var timeBis = 1;
		if (time == 0) time = timeBis;
		setDelayRythme(time);
	}
	
	//----------------------------------------------------------------------------
	
	function buzzerOnOff(state) {
		device.send([START_SYSEX, BUZZER,8, state, END_SYSEX]);
	}

	ext.buzzerOnOff = function(state){
    	if (state == "On") etat = 1;
        if (state == "Off") etat = 0;
		buzzerOnOff(etat);
	}
	
	//----------------------------------------------------------------------------

	function buzzerOnOffDelay(state,time) {
		device.send([START_SYSEX, BUZZER,9, state, time, END_SYSEX]);
	}

    ext.buzzerOnOffDelay = function(state,time){
		var timeBis = 1;
		if (time == 0) time = timeBis;
		if (state == "On") etat = 1;
		if (state == "Off") etat = 0;
		buzzerOnOffDelay(etat,time);
	}
	
	//----------------------------------------------------------------------------

	function setDelayAttente(time) {
        device.send([START_SYSEX, BUZZER,6, time, END_SYSEX]);
	}

	ext.setDelayAttente = function(time) {
    	var timeBis = 1;
        if (time == 0) time = timeBis;
        setDelayAttente(time);
	}
	
	//----------------------------------------------------------------------------

	function playNote(octave, newNote) {
        device.send([START_SYSEX, BUZZER,2, octave, newNote, END_SYSEX]);
	}

    ext.playNote = function(octave,note)   {
    	var newNote = 100;
        switch (note){
        	case "do": newNote = 1;
            	break;
			case "do#": newNote = 2;
            	break;
			case "re": newNote = 3;
            	break;
			case "re#": newNote = 4;
            	break;
			case "mi": newNote = 5;
            	break;
			case "fa": newNote = 6;
            	break; 
			case "fa#": newNote = 7;
            	break;
			case "sol": newNote = 8;
            	break;
			case "sol#": newNote = 9;
            	break;   
			case "la": newNote = 10;
            	break;
			case "la#": newNote = 11;
            	break;
			case "si": newNote = 12;
            	break;
			default: 
            	break;               
        }
    	playNote(octave,newNote);
	}

	//----------------------------------------------------------------------------

	function playSonDelay(frequency, time) {
        device.send([START_SYSEX, BUZZER,4, frequency, time, END_SYSEX]);
	}

    ext.playSonDelay = function(frequency,time)    {
    	var threshold_d = 80;
        var threshold_u = 8000; 
        if (frequency < threshold_d) frequency = threshold_d;
        if (frequency > threshold_u) frequency = threshold_u;
        var timeBis = 1;
        if (time == 0) time = timeBis;
        playSonDelay(frequency,time);
	}

	
	/*==============================================================================
    * EMOTION FONCTIONS
    *============================================================================*/

        function expression(emotionNbr,matrixNbr) {
            device.send([START_SYSEX,EMOTION,emotionNbr,matrixNbr,END_SYSEX]);
        }
        ext.expression = function(emotion,wichMatrix) {
            var emotionNbr = 0;
            var matrixNbr = 0;
            if (emotion == "happy")         emotionNbr = 1;
            else if (emotion == "inLove")   emotionNbr = 2;
            else if (emotion == "crazyEye") emotionNbr = 3;
            else if (emotion == "deadEye")  emotionNbr = 4;
            else if (emotion == "snowEye")  emotionNbr = 5;
            else if (emotion == "starEye")  emotionNbr = 6;

            if (wichMatrix == "l'oeil gauche")      matrixNbr = 1;
            else if (wichMatrix == "l'oeil droit")  matrixNbr = 2;
            else if (wichMatrix == "les 2 yeux")    matrixNbr = 3;

            expression(emotionNbr,matrixNbr);
        }
        
    // ===================================== END Emotion functions ===========================
	
	
	ext.runBot = function(direction,speed) {
		var leftSpeed = 0;
		var rightSpeed = 0;
		if(direction=="run forward"){
			leftSpeed = -speed;
			rightSpeed = speed;
		}else if(direction=="run backward"){
			leftSpeed = speed;
			rightSpeed = -speed;
		}else if(direction=="turn left"){
			leftSpeed = speed;
			rightSpeed = speed;
		}else if(direction=="turn right"){
			leftSpeed = -speed;
			rightSpeed = -speed;
		}
        runPackage(5,short2array(leftSpeed),short2array(rightSpeed));
    };
    
	ext.runMotor = function(port,speed) {
		if(typeof port=="string"){
			port = ports[port];
		}
		if(port == 9){
			speed = -speed;
		}
        runPackage(10,port,short2array(speed));
    };
    ext.runServo = function(port,slot,angle) {
		if(typeof port=="string"){
			port = ports[port];
		}
		if(typeof slot=="string"){
			slot = slots[slot];
		}
		if(angle > 180){
			angle = 180;
		}
        runPackage(11,port,slot,angle);
    };
	ext.runBuzzer = function(tone, beat){
		if(typeof tone == "string"){
			tone = tones[tone];
		}
		if(typeof beat == "string"){
			beat = parseInt(beat) || beats[beat];
		}
		runPackage(34,short2array(tone), short2array(beat));
	};
	
	ext.stopBuzzer = function(){
		runPackage(34,short2array(0));
	};
	
	// ==================== MOTOR FUNCTIONS ==============================
	
	function motor(moteur, direction, pwmMot){
    	device.send([START_SYSEX, MOTOR,1,moteur, direction, pwmMot, END_SYSEX]);
    }

	ext.motor = function(quelMoteur, direction, pwmMot){
            
        var threshold = 100;
        var moteur = 3;
        var dir = 1;

        if (quelMoteur == 'gauche') moteur = 1;
        else if (quelMoteur == 'droit') moteur = 2;
        else if (quelMoteur == 'gauche et droit') moteur = 3;

        if (direction == 'avance') dir = 0;
        else if (direction == 'recule') dir = 1;

        if (pwmMot > threshold) pwmMot = 100;
		if (pwmMot < 0) pwmMot = 0;

    	motor(moteur, dir, pwmMot); 
	}	
	
	function stop(moteur) {
        device.send([START_SYSEX, MOTOR, 2, moteur, END_SYSEX]);
	}

    ext.stop = function(quelMoteur) {
    	var moteur = 3;
        if (quelMoteur == 'gauche') moteur = 1;
        else if (quelMoteur == 'droit') moteur = 2;
        else if (quelMoteur == 'gauche et droit') moteur = 3;

        stop(moteur); // 
    }
	
	ext.runSevseg = function(port,display){
		if(typeof port=="string"){
			port = ports[port];
		}
		runPackage(9,port,float2array(display));
	};
	
	ext.runLed = function(ledIndex,red,green,blue){
		if(ledIndex == "led left"){
			ledIndex = 2;
		}else if(ledIndex == "led right"){
			ledIndex = 1;
		}
		ext.runLedStrip(7, 2, ledIndex, red,green,blue);
	};
	ext.runLedExternal = function(port,ledIndex,red,green,blue){
		ext.runLedStrip(port, 2, ledIndex, red,green,blue);
	};
	ext.runLedStrip = function(port,slot,ledIndex,red,green,blue){
		if(typeof port=="string"){
			port = ports[port];
		}
		if("all" == ledIndex){
			ledIndex = 0;
		}
		if(typeof slot=="string"){
			slot = slots[slot];
		}
		if(port == 7 && ledIndex > 2){
			interruptThread("mCore not support led index greater than 2");
			return;
		}
		runPackage(8,port,slot,ledIndex,red,green,blue);
	};
	ext.runLightSensor = function(port,status){
		if(typeof port=="string"){
			port = ports[port];
		}
		if(typeof status=="string"){
			status = switchStatus[status];
		}
		runPackage(3,port,status);
	};
	ext.runShutter = function(port,status){
		if(typeof port=="string"){
			port = ports[port];
		}
		runPackage(20,port,shutterStatus[status]);
	};
	ext.runFan = function(port, direction) {
		var portToPin = {
			Port1: [11, 12],
			Port2: [9, 10],
			Port3: [16, 17],
			Port4: [14, 15]
		};
		var directionToValue = {
			"clockwise": [1,0], 
			"counter-clockwise": [0,1], 
			"stop": [0,0]
		};
		if(typeof port=="string"){
			pins = portToPin[port];
			if(pins) {
				runPackage(0x1e, pins[0], directionToValue[direction][0]);
				runPackage(0x1e, pins[1], directionToValue[direction][1]);
			}
		}
	};
	ext.runIR = function(message){
		runPackage(13,string2array(message));
	};
	ext.showNumber = function(port,message){
		if(typeof port=="string"){
			port = ports[port];
		}
		runPackage(41,port,4,float2array(message));
	};
	ext.showCharacters = function(port,x,y,message){
		if(typeof port=="string"){
			port = ports[port];
		}
		var index = Math.max(0, Math.floor(x / -6));
		message = message.toString().substr(index, 4);
		if(index > 0){
			x += index * 6;
		}
		if(x >  16) x = 16;
		if(y >  8) y = 8;
		if(y < -8) y = -8;
		runPackage(41,port,1,x,y+7,message.length,string2array(message));
	}
	ext.showTime = function(port,hour,point,min){
		if(typeof port=="string"){
			port = ports[port];
		}
		runPackage(41,port,3,point==":"?1:0,hour,min);
	}
	ext.showDraw = function(port,x,y,bytes){
		if(typeof port=="string"){
			port = ports[port];
		}
		if(x >  16) x = 16;
		if(x < -16) x = -16;
		if(y >  8) y = 8;
		if(y < -8) y = -8;
		runPackage(41,port,2,x,y,bytes);
	}
	ext.resetTimer = function(){
		startTimer = (new Date().getTime())/1000.0;
		responseValue();
	};
	/*
	ext.getLightOnBoard = function(nextID){
		var deviceId = 31;
		getPackage(nextID,deviceId,6);
	}
	*/
	ext.getButtonOnBoard = function(nextID,status){
		var deviceId = 35;
		if(typeof status == "string"){
			if(status=="pressed"){
				status = 0;
			}else if(status=="released"){
				status = 1;
			}
		}
		getPackage(nextID,deviceId,7,status);
	}
	ext.getUltrasonic = function(nextID,port){
		var deviceId = 1;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
	};
	ext.getPotentiometer = function(nextID,port) {
		var deviceId = 4;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
    };
    ext.getHumiture = function(nextID,port,valueType){
    	var deviceId = 23;
		if(typeof port=="string"){
			port = ports[port];
		}
		if(typeof valueType=="string"){
			valueType = ("humidity" == valueType) ? 0 : 1;
		}
		getPackage(nextID,deviceId,port,valueType);
    };
    ext.getFlame = function(nextID,port){
   		var deviceId = 24;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
    };
    ext.getGas = function(nextID,port){
    	var deviceId = 25;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
    };
    ext.gatCompass = function(nextID,port){
    	var deviceId = 26;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
    };
	ext.getLinefollower = function(nextID,port) {
		var deviceId = 17;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
    };
	ext.getLinefollowerStatus = function(nextID, port, type, color) {
		var deviceId = 17;
		if(typeof port=="string"){
			port = ports[port];
		}
		// the response of line follower is 0~3; in binary 00, 10, 01, 11, the higher digit is for left sensor
		var sensorMask = type=='leftSide'? 2 : 1;
		var reverseFlag = color=='black'? true : false;
		responsePreprocessor[nextID] = (function(mask, flag){return function(value){
			var result = (value & mask);
			if(flag) result = !result;
			return result?1:0;
		}})(sensorMask, reverseFlag);
		getPackage(nextID,deviceId,port);
    };
	ext.getLightSensor = function(nextID,port) {
		var deviceId = 3;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(0,deviceId,port);
    };
	ext.getJoystick = function(nextID,port,ax) {
		var deviceId = 5;
		if(typeof port=="string"){
			port = ports[port];
		}
		if(typeof ax=="string"){
			ax = axis[ax];
		}
		getPackage(nextID,deviceId,port,ax);
    };
	ext.getSoundSensor = function(nextID,port) {
		var deviceId = 7;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
    };
	ext.getInfrared = function(nextID,port) {
		var deviceId = 16;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
    };
	ext.getLimitswitch = function(nextID,port,slot) {
		var deviceId = 21;
		if(typeof port=="string"){
			port = ports[port];
		}
		if(typeof slot=="string"){
			slot = slots[slot];
		}
		getPackage(nextID,deviceId,port,slot);
    };
    ext.getTouchSensor = function(port){
    	var deviceId = 51;
    	if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(0,deviceId,port);
    };
    ext.getButton = function(port, key){
    	var deviceId = 22;
    	if(typeof port=="string"){
			port = ports[port];
		}
		if(typeof key == "string"){
			key = button_keys[key];
		}
		getPackage(0,deviceId,port, key);
    };
	ext.getPirmotion = function(nextID,port) {
		var deviceId = 15;
		if(typeof port=="string"){
			port = ports[port];
		}
		getPackage(nextID,deviceId,port);
    };
	ext.getTemperature = function(nextID,port,slot) {
		var deviceId = 2;
		if(typeof port=="string"){
			port = ports[port];
		}
		if(typeof slot=="string"){
			slot = slots[slot];
		}
		getPackage(nextID,deviceId,port,slot);
    };
    ext.getGyro = function(nextID,ax) {
		var deviceId = 6;
		if(typeof ax=="string"){
			ax = axis[ax];
		}
		getPackage(nextID,deviceId,0,ax);
    };
	ext.getIrRemote = function(nextID,code){
		var deviceId = 14;
		if(typeof code=="string"){
			code = ircodes[code];
		}
		getPackage(nextID,deviceId,0,code);
	}
	ext.getIR = function(nextID){
		var deviceId = 13;
		getPackage(nextID,deviceId);
	}
	ext.getTimer = function(nextID){
		if(startTimer==0){
			startTimer = (new Date().getTime())/1000.0;
		}
		responseValue(nextID,(new Date().getTime())/1000.0-startTimer);
	}
	function sendPackage(argList, type){
		var bytes = [0xff, 0x55, 0, 0, type];
		for(var i=0;i<argList.length;++i){
			var val = argList[i];
			if(val.constructor == "[class Array]"){
				bytes = bytes.concat(val);
			}else{
				bytes.push(val);
			}
		}
		bytes[2] = bytes.length - 3;
		device.send(bytes);
	}
	
	function runPackage(){
		sendPackage(arguments, 2);
	}
	function getPackage(){
		var nextID = arguments[0];
		Array.prototype.shift.call(arguments);
		sendPackage(arguments, 1);
	}

    var inputArray = [];
	var _isParseStart = false;
	var _isParseStartIndex = 0;
    function processData(bytes) {
		var len = bytes.length;
		if(_rxBuf.length>30){
			_rxBuf = [];
		}
		for(var index=0;index<bytes.length;index++){
			var c = bytes[index];
			_rxBuf.push(c);
			if(_rxBuf.length>=2){
				if(_rxBuf[_rxBuf.length-1]==0x55 && _rxBuf[_rxBuf.length-2]==0xff){
					_isParseStart = true;
					_isParseStartIndex = _rxBuf.length-2;
				}
				if(_rxBuf[_rxBuf.length-1]==0xa && _rxBuf[_rxBuf.length-2]==0xd&&_isParseStart){
					_isParseStart = false;
					
					var position = _isParseStartIndex+2;
					var extId = _rxBuf[position];
					position++;
					var type = _rxBuf[position];
					position++;
					//1 byte 2 float 3 short 4 len+string 5 double
					var value;
					switch(type){
						case 1:{
							value = _rxBuf[position];
							position++;
						}
							break;
						case 2:{
							value = readFloat(_rxBuf,position);
							position+=4;
						}
							break;
						case 3:{
							value = readInt(_rxBuf,position,2);
							position+=2;
						}
							break;
						case 4:{
							var l = _rxBuf[position];
							position++;
							value = readString(_rxBuf,position,l);
						}
							break;
						case 5:{
							value = readDouble(_rxBuf,position);
							position+=4;
						}
							break;
						case 6:
							value = readInt(_rxBuf,position,4);
							position+=4;
							break;
					}
					if(type<=6){
						if (responsePreprocessor[extId] && responsePreprocessor[extId] != null) {
							value = responsePreprocessor[extId](value);
							responsePreprocessor[extId] = null;
						}
						responseValue(extId,value);
					}else{
						responseValue();
					}
					_rxBuf = [];
				}
			} 
		}
    }
	function readFloat(arr,position){
		var f= [arr[position],arr[position+1],arr[position+2],arr[position+3]];
		return parseFloat(f);
	}
	function readInt(arr,position,count){
		var result = 0;
		for(var i=0; i<count; ++i){
			result |= arr[position+i] << (i << 3);
		}
		return result;
	}
	function readDouble(arr,position){
		return readFloat(arr,position);
	}
	function readString(arr,position,len){
		var value = "";
		for(var ii=0;ii<len;ii++){
			value += String.fromCharCode(_rxBuf[ii+position]);
		}
		return value;
	}
    function appendBuffer( buffer1, buffer2 ) {
        return buffer1.concat( buffer2 );
    }

    // Extension API interactions
    var potentialDevices = [];
    ext._deviceConnected = function(dev) {
        potentialDevices.push(dev);
        if (!device) {
            tryNextDevice();
        }
    }

    function tryNextDevice() {
        // If potentialDevices is empty, device will be undefined.
        // That will get us back here next time a device is connected.
        device = potentialDevices.shift();
        if (device) {
            device.open({ stopBits: 0, bitRate: 57600, ctsFlowControl: 0 }, deviceOpened);
        }
    }

    var watchdog = null;
    function deviceOpened(dev) {
        if (!dev) {
            // Opening the port failed.
            tryNextDevice();
            return;
        }
        device.set_receive_handler('mbot',processData);
    };

    ext._deviceRemoved = function(dev) {
        if(device != dev) return;
        device = null;
    };

    ext._shutdown = function() {
        if(device) device.close();
        device = null;
    };

    ext._getStatus = function() {
        if(!device) return {status: 1, msg: 'mBot disconnected'};
        if(watchdog) return {status: 1, msg: 'Probing for mBot'};
        return {status: 2, msg: 'mBot connected'};
    }
    var descriptor = {};
	ScratchExtensions.register('mBot', descriptor, ext, {type: 'serial'});
})({});
