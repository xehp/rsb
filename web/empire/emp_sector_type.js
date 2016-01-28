// emp_unit_type.js
// Copyright (C) 2015 Henrik Bjorkman www.eit.se/hb
// Created 2015-01-15 by Henrik Bjorkman www.eit.se/hb

var empImagesBeingLoaded=0;
var empImageDefaultSize=32;

function EmpSectorType(typeName)
{	
	this.typeName=typeName;

	var canvas = document.createElement('canvas');
	var context = canvas.getContext('2d');

	canvas.width = empImageDefaultSize;
	canvas.height = empImageDefaultSize;


	this.empImg=new EmpImg(this.typeName);
}


EmpSectorType.prototype.showSelfContextXY=function(context, x, y, width, height)
{
	this.empImg.showSelfContextXY(context, x, y, width, height);
}



