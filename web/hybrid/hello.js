/*
hello.js

Copyright (C) 2015 Henrik Björkman www.eit.se/hb

Licence: You may use, copy, modify and redistribute this code as long as you keep the History below and add state there if you did any changes.

History:
Created 2015 by Henrik Bjorkman www.eit.se

*/
      

// http://kripken.github.io/emscripten-site/docs/api_reference/preamble.js.html


var mainCanvas;


// mousemove callback
function mainMouseMoveCallback(e) {
	var movementX = e.movementX ||
					e.mozMovementX ||
					0;
	var movementY = e.movementY ||
					e.mozMovementY ||
					0;

	//console.log("X: " + movementX + ', Y: ' + movementY);

	if (movementX != 0)
	{
		a_result = Module.ccall('mainHandleNumericEvent', 'number', ['number', 'number'], [3, movementX]); // 3 = NumericEventMovementX
	}

	if (movementY != 0)
	{
		a_result = Module.ccall('mainHandleNumericEvent', 'number', ['number', 'number'], [4, movementY]); // 4 = NumericEventMovementY
	}

}

function mainIsPointerLock() {

	return document.pointerLockElement === mainCanvas ||
		document.mozPointerLockElement === mainCanvas ||
		document.webkitPointerLockElement === mainCanvas;

	//return document.pointerLockElement === mainCanvas;
}


// pointerlockchange callback
function mainLockChangeAlert() {
	//console.log("mainLockChangeAlert");

	if(mainIsPointerLock()) {
		console.log('The pointer lock status is now locked');
		document.addEventListener("mousemove", mainMouseMoveCallback, false);
	} else {
		console.log('The pointer lock status is now unlocked');
		document.removeEventListener("mousemove", mainMouseMoveCallback, false);
	}
}


function mainSetUpPointerLock()
{
	console.log("mainSetUpPointerLock");

	// https://developer.mozilla.org/en-US/docs/Web/API/Pointer_Lock_API
	mainCanvas.requestPointerLock = mainCanvas.requestPointerLock ||
                            mainCanvas.mozRequestPointerLock ||
                            mainCanvas.webkitRequestPointerLock;
    
	document.exitPointerLock = document.exitPointerLock ||
	         document.mozExitPointerLock ||
	         document.webkitExitPointerLock;
         



	document.addEventListener('pointerlockchange', mainLockChangeAlert, false);
	document.addEventListener('mozpointerlockchange', mainLockChangeAlert, false);
	document.addEventListener('webkitpointerlockchange', mainLockChangeAlert, false);


	// Now we have two options (comment one of the below out):

	// Shall we lock when user click?
	mainCanvas.onclick = function() {
		mainCanvas.requestPointerLock();
	}            

	// Or lock right away?
	//mainCanvas.requestPointerLock();

}

function mainReleasePointerLock()
{
	console.log("mainReleasePointerLock");

	document.exitPointerLock = document.exitPointerLock    ||
	                           document.mozExitPointerLock   ||
	                           document.webkitExitPointerLock;
	
	// Attempt to unlock
	document.exitPointerLock();
}

// http://www.html5canvastutorials.com/advanced/html5-canvas-mouse-coordinates/
// This solution work in most cases. But in android-chrome it does not work if user has zoomed in.
function mapGetMousePos(canvas, evt) {
	var rect = canvas.getBoundingClientRect();
	return {
		x: Math.round((evt.clientX-rect.left)/(rect.right-rect.left)*canvas.width),
		y: Math.round((evt.clientY-rect.top)/(rect.bottom-rect.top)*canvas.height)
	};
}

function mainSetUpKeyboardEvents()
{

	document.onkeydown=function(event)
	{
		//console.log('onkeydown: ' + event.which);		
		a_result = Module.ccall('mainHandleNumericEvent', 'number', ['number', 'number'], [1, event.which]); // 1 = NumericEventKeyDown
	};

	document.onkeyup=function(event)
	{
		//console.log('onkeyup: ' + event.which);		
		a_result = Module.ccall('mainHandleNumericEvent', 'number', ['number', 'number'], [2, event.which]); // 2 = NumericEventKeyUp
	};

	document.onkeypress=function(event)
	{
		//console.log('onkeypress: ' + event.which);		
		a_result = Module.ccall('mainHandleNumericEvent', 'number', ['number', 'number'], [5, event.which]); // 5 = NumericEventKeyPress
	};

	// http://stackoverflow.com/questions/3138756/jquery-repeat-function-every-60-seconds
	/*(function(){
		a_result = Module.ccall('mainHandleStringEvent', 'string', ['number', 'string'], [0, "tick"]);
	    	setTimeout(arguments.callee, 100);
	})();*/
}


// http://www.sitepoint.com/html5-javascript-mouse-wheel/
function MouseWheelHandler(e) {

	// cross-browser wheel delta
	var e = window.event || e;
	var d = e.deltaY || e.wheelDelta || -e.detail;

	//console.log("e "+e+" d "+d);

	var msg=""+d;
	a_result = Module.ccall('mainHandleStringEvent', 'number', ['number', 'string'], [7, msg]); // 7 = StringEventMouseButtonDown
	return false;
}


function mainSetUpMouseEvents()
{
	mainCanvas.addEventListener('mousedown', function (evt) {
		var mouseUpPos = mapGetMousePos(canvas, evt);
		var x=0;
		var y=0;
		var msg=""+mouseUpPos.x+" "+mouseUpPos.y;
		a_result = Module.ccall('mainHandleStringEvent', 'number', ['number', 'string'], [5, msg]); // 5 = StringEventMouseButtonDown
	}, false);

	mainCanvas.addEventListener('mouseup', function (evt) {
		var mouseUpPos = mapGetMousePos(canvas, evt);
		var x=0;
		var y=0;
		var msg=""+mouseUpPos.x+" "+mouseUpPos.y;
		a_result = Module.ccall('mainHandleStringEvent', 'number', ['number', 'string'], [6, msg]); // 6 = StringEventMouseButtonUp
	}, false);

	// http://www.sitepoint.com/html5-javascript-mouse-wheel/
	if (mainCanvas.addEventListener) {
		// IE9, Chrome, Safari, Opera
		mainCanvas.addEventListener("mousewheel", MouseWheelHandler, false);
		// Firefox
		mainCanvas.addEventListener("DOMMouseScroll", MouseWheelHandler, false);

		mainCanvas.addEventListener("wheel", MouseWheelHandler, false);

		//mainCanvas.addEventListener("onmousewheel", MouseWheelHandler, false);

	}

	/*
	mainCanvas.addEventListener('wheel', function (evt) {
		// www.javascriptkit.com/javatutors/onmousewheel.shtml
		//var delta=evt.detail? evt.detail*(-120) : evt.wheelDelta //check for detail first so Opera uses that instead of wheelDelta

		// http://www.sitepoint.com/html5-javascript-mouse-wheel/
		var delta = Math.max(-1, Math.min(1, (evt.wheelDelta || -evt.detail)));

		var msg=""+delta;
		a_result = Module.ccall('mainHandleStringEvent', 'number', ['number', 'string'], [7, msg]); // 7 = StringEventMouseButtonDown
	}, false);
	*/
}


// Connect to server via web sockets, ask for IP and port if not given by server already.
function mainConnectToServer()
{
	// http://stackoverflow.com/questions/519145/how-can-i-check-whether-a-variable-is-defined-in-javascript
	// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/typeof
	if (typeof wsUri === 'undefined') {
   	    // wsUri is undefined, ask user for which server to connect to.
   	    wsUri=prompt("Please enter url", "ws://b3.eit.se:8080/");
	}

	console.log("wsUri "+wsUri);

	if (wsUri == null)
	{
		wsUri = "ws://localhost:8080/"
	}

	onTestWebSocket(wsUri);
}


// This is called from html body "onload". Or is it? Perhaps this is not used?
/*
function mainStart() {
	var newPage = "";

	console.log("mainStart or what");



        var element = document.getElementById('textOutput');
	if (element) {
              element.value += "text" + "\n";
              element.scrollTop = element.scrollHeight; // focus on bottom
            }

	Module.print('testing print');
	
	

	
	
	a_result = Module.ccall('mainInit', 'number', ['number', 'string'], [10, "who is this"]);
	

	Module.print('a_result: '+ a_result);




	// Pointer lock
	();
	




	// Keyboard events
	mainSetUpKeyboardEvents();
	
	

	

	//window.resizeTo(640,400);
	console.log("windowSize "+window.innerWidth + " "+window.innerHeight);


	// Connecting to a server
	
	mainConnectToServer();

}
*/      

// This shall be called from c++ code when its main has been called.
// It calls this method so we can setup things that C++ could not set up.
function allReady()
{
	console.log("allReady");

	mainCanvas = document.getElementById("canvas"); 

	mainSetUpMouseEvents();

	// Pointer lock
	// locking here disabled for now, user will need to press home button to get pointer lock (end button to release it), no got problems if not setting up pointer lock here. Probably because mainCanvas was then undefined.
	//mainSetUpPointerLock();

	mainSetUpKeyboardEvents();

	mainConnectToServer();	
}

// This shall be called from c++ code before it exits
// It calls this method so we can take down things set up by allReady.
function allOver()
{
	console.log("allOver");
	mainReleasePointerLock();	
}

    
