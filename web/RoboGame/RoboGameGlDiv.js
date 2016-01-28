// RoboGameGlDiv.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-03-12 by Henrik Bjorkman www.eit.se/hb

// TODO: Probably gl don't need to be a global variable, but trying to follow this page as close as possible:
// https://developer.mozilla.org/en-US/docs/Web/WebGL/Getting_started_with_WebGL
var gl; // A global variable for the WebGL context.

var x = 0;
var y = 0;






// TODO: Resizing the WebGL context
// https://developer.mozilla.org/en-US/docs/Web/WebGL/Getting_started_with_WebGL
// gl.viewport(0, 0, canvas.width, canvas.height);


// TODO: The canvas is currently empty. How to write something into it:
// https://developer.mozilla.org/en-US/docs/Web/WebGL/Adding_2D_content_to_a_WebGL_context
// http://learningwebgl.com/blog/?p=28

// TODO: We will need pointer lock
// https://developer.mozilla.org/en-US/docs/Web/API/Pointer_Lock_API




RoboGameGlDiv.prototype = Object.create(DivBase.prototype);
RoboGameGlDiv.prototype.constructor = RoboGameGlDiv;

function RoboGameGlDiv(parentWin)
{	
	DivBase.call(this, parentWin); // call super constructor


	this.divSizeX=null; // in pixels
	this.divSizeY=null;
	
	
	this.currentRoom = null;
	
	
}


RoboGameGlDiv.prototype.defineDiv=function(divSize)
{
	var newPage='';	

	this.divSize = {x: divSize.x, y: divSize.y};

	// The central div area of the page (perhaps not needed in this case)	
	newPage+='<div id="terrainDiv" style="width:'+divSize.x+'px; height:'+divSize.y+'px;">';

	// The first thing you need in order to use WebGL to render in 3D is a canvas. Note name/id "glcanvas" here, it will be needed later when calling getElementById.
	newPage+='<canvas id="glcanvas" width="'+this.divSize.x+'" height="'+this.divSize.y+'">';
    newPage+='Your browser does not appear to support the HTML5 <code>&lt;canvas&gt;</code> element.';
	newPage+='</canvas>';
	
	newPage+='</div>';

	return newPage;		
}

/*
	var tracker = document.createElement('p');
	var body = document.querySelector('body');
	body.appendChild(tracker);
	tracker.style.position = 'absolute';
	tracker.style.top = '0';
	tracker.style.right = '10px';
	tracker.style.backgroundColor = 'white';
*/

// Creating a WebGL context
// https://developer.mozilla.org/en-US/docs/Web/WebGL/Getting_started_with_WebGL
// http://learningwebgl.com/blog/?p=28
/*
RoboGameGlDiv.prototype.initWebGL = function(canvas) 
{
	gl = null;
	  
	try {
	    // Try to grab the standard context. If it fails, fallback to experimental.
	    gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
        gl.viewportWidth = canvas.width;
        gl.viewportHeight = canvas.height;
		console.log("initWebGL " + gl.viewportWidth + " " + gl.viewportHeight);
	}
	catch(e) {}
	  
	// If we don't have a GL context, give up now
	if (!gl) {
	    alert("Unable to initialize WebGL. Your browser may not support it.");
	    gl = null;
	}
	  
	return gl;
}
*/

// AKA startDiv, defineDiv shall be called first.
RoboGameGlDiv.prototype.addEventListenersDiv=function()
{
	console.log("addEventListenersDiv");

	// Get the canvas
    var canvas = document.getElementById("glcanvas");  // "glcanvas" is the name/id given to the canvas in defineDiv 

	// Set up the WebGL context.
    try {
        //gl = canvas.getContext("experimental-webgl2"); // TODO would want to use WebGl2
        gl = canvas.getContext("webgl");
        gl.viewportWidth = canvas.width;
        gl.viewportHeight = canvas.height;
    } catch (e) {
    }
    if (!gl) {
        alert("Could not initialise WebGL, sorry :-(");
    }


    this.initShaders();
    this.initBuffers();

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.enable(gl.DEPTH_TEST);


 
	// Only continue if WebGL is available and working  
	if (gl) {

		//gl.depthFunc(gl.LEQUAL);                                // Near things obscure far things
		//gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);      // Clear the color as well as the depth buffer.

	
	
		// Setting up mouse input (keyboard is set up in RoboGameWin.prototype.addEventListenersDiv)
	/*
	
		// Do we need to call super class? Probably not in this case, we will do all below instead.
		// DivBase.prototype.addEventListenersDiv.call(this, "glcanvas");
	
	
	
		// https://developer.mozilla.org/en-US/docs/Web/API/Pointer_Lock_API
		canvas.requestPointerLock = canvas.requestPointerLock ||
	                            canvas.mozRequestPointerLock ||
	                            canvas.webkitRequestPointerLock;
	    
		document.exitPointerLock = document.exitPointerLock ||
		         document.mozExitPointerLock ||
		         document.webkitExitPointerLock;
	         
	
		canvas.onclick = function() {
			canvas.requestPointerLock();
		}             
	
	
		document.addEventListener('pointerlockchange', lockChangeAlert, false);
		document.addEventListener('mozpointerlockchange', lockChangeAlert, false);
		document.addEventListener('webkitpointerlockchange', lockChangeAlert, false);
		
		*/
	}
	else
	{
		console.log("GL failed");
	}
}



// pointerlockchange callback
function lockChangeAlert() {
	if(document.pointerLockElement === canvas ||
		document.mozPointerLockElement === canvas ||
		document.webkitPointerLockElement === canvas) {
		console.log('The pointer lock status is now locked');
		document.addEventListener("mousemove", canvasLoop, false);
	} else {
		console.log('The pointer lock status is now unlocked');
		document.removeEventListener("mousemove", canvasLoop, false);
	}
}

// mousemove callback
function canvasLoop(e) {
	var movementX = e.movementX ||
					e.mozMovementX ||
					e.webkitMovementX ||
					0;
	var movementY = e.movementY ||
					e.mozMovementY ||
					e.webkitMovementY ||
					0;
	x += movementX;
	y += movementY;
	//canvasDraw();
	var animation = requestAnimationFrame(canvasLoop);
	//tracker.innerHTML = "X position: " + x + ', Y position: ' + y;
	console.log("X position: " + x + ', Y position: ' + y);
}


// TODO: There is nobody calling this. RoboGameWin should do that.
/*
RoboGameGlDiv.prototype.removeEventListenersDiv=function()
{
	console.log("addEventListenersDiv");

	document.exitPointerLock = document.exitPointerLock    ||
	                           document.mozExitPointerLock ||
	                           document.webkitExitPointerLock;
	
	// Attempt to unlock
	document.exitPointerLock();
}
*/


// Resizing the WebGL context
// TODO: There is nobody calling this. RoboGameWin should do that.
/*
RoboGameGlDiv.prototype.resizeDiv=function()
{
	gl.viewport(0, 0, canvas.width, canvas.height);
}
*/

/*
	ctx.beginPath();
	ctx.arc(x,y,20,0,degToRad(360), true);
	ctx.fill();
*/



RoboGameGlDiv.prototype.drawDiv=function()
{
	console.log("drawDiv "+this.parentWin.x+" "+this.parentWin.y);
	
    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	// This sets viewing angle and perspective.
    mat4.perspective(45, gl.viewportWidth / gl.viewportHeight, 0.1, 100.0, pMatrix);

    mat4.identity(mvMatrix);

	// Move everything, TODO: Here we will translate for observer/camera. 
    mat4.translate(mvMatrix, [-1.5+this.parentWin.x, 0.0+this.parentWin.y, -7.0+this.parentWin.z]);
    
    // Drawing the white triangle 
    gl.bindBuffer(gl.ARRAY_BUFFER, triangleVertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, triangleVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);
    setMatrixUniforms();
    gl.drawArrays(gl.TRIANGLES, 0, triangleVertexPositionBuffer.numItems);

    // Drawing the white square 
    mat4.translate(mvMatrix, [3.0, 0.0, 0.0]);
    gl.bindBuffer(gl.ARRAY_BUFFER, squareVertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, squareVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);
    setMatrixUniforms();
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, squareVertexPositionBuffer.numItems);
}





/*
// https://www.flynsarmy.com/2012/05/an-introduction-to-the-html5-pointer-lock-and-fullscreen-apis/

// You may request full screen mode like so

element.requestFullScreen =
	element.requestFullScreen    ||
	element.mozRequestFullScreen ||
	element.webkitRequestFullScreen;
element.requestFullScreen(element.ALLOW_KEYBOARD_INPUT);

// Once in full screen mode, pointer lock should become available:

navigator.pointer = navigator.pointer || navigator.webkitPointer;
navigator.pointer.lock(element,
	function() {
		console.log('Pointer lock');
	},
	function() {
		console.log('No pointer lock');
	}
);
*/



// TODO: Use WebGL2
// http://analyticalgraphicsinc.github.io/webglreport/?v=1
// http://analyticalgraphicsinc.github.io/webglreport/?v=2
// https://wiki.mozilla.org/Platform/GFX/WebGL2


// http://learningwebgl.com/blog/?p=28




// Loading shaders from the DOM
RoboGameGlDiv.prototype.getShader=function(gl, id) {
    var shaderScript = document.getElementById(id);
    if (!shaderScript) {
        return null;
    }

    var str = "";
    var k = shaderScript.firstChild;
    while (k) {
        if (k.nodeType == 3) {
            str += k.textContent;
        }
        k = k.nextSibling;
    }

    var shader;
    if (shaderScript.type == "x-shader/x-fragment") {
        shader = gl.createShader(gl.FRAGMENT_SHADER);
    } else if (shaderScript.type == "x-shader/x-vertex") {
        shader = gl.createShader(gl.VERTEX_SHADER);
    } else {
        return null;
    }

    gl.shaderSource(shader, str);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert(gl.getShaderInfoLog(shader));
        return null;
    }

    return shader;
}


var shaderProgram;

// http://www.html5rocks.com/en/tutorials/webgl/webgl_fundamentals/
// fs = FragmentShader
// vs = VertexShader


// Initializing the shaders
// "shader-fs" and "shader-vs" are defined in the  html page
RoboGameGlDiv.prototype.initShaders=function()
{
	// setup a GLSL program
	
	var fragmentShader = this.getShader(gl, "shader-fs");
	var vertexShader = this.getShader(gl, "shader-vs");
	
	shaderProgram = gl.createProgram();
	gl.attachShader(shaderProgram, vertexShader);
	gl.attachShader(shaderProgram, fragmentShader);
	gl.linkProgram(shaderProgram);
	
	if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
	    alert("Could not initialise shaders");
	}
	
	gl.useProgram(shaderProgram);
	
	shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
	gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);
	
	shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
	shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
}


var mvMatrix = mat4.create();
var pMatrix = mat4.create();

function setMatrixUniforms() {
    gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
    gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
}



var triangleVertexPositionBuffer;
var squareVertexPositionBuffer;

RoboGameGlDiv.prototype.initBuffers=function()
{
	// Init triangle
    triangleVertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, triangleVertexPositionBuffer);
    var vertices = [
         0.0,  1.0,  0.0,
        -1.0, -1.0,  0.0,
         1.0, -1.0,  0.0
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    triangleVertexPositionBuffer.itemSize = 3;
    triangleVertexPositionBuffer.numItems = 3;

	// Init square
    squareVertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, squareVertexPositionBuffer);
    vertices = [
         1.0,  1.0,  0.0,
        -1.0,  1.0,  0.0,
         1.0, -1.0,  0.0,
        -1.0, -1.0,  0.0
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    squareVertexPositionBuffer.itemSize = 3;
    squareVertexPositionBuffer.numItems = 4;
}






