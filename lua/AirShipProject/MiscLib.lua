


-- help function used within this script
-- Mode and color are packed into one byte. If packColorAndMode in java is changed some change here will also be needed. 
-- PositionForBlockMode = 0
-- PositionForColor = 2
function packColorAndMode(blockColor, blockMode)
--	blockColorAndMode=(blockColor * 4) + blockMode
	blockColorAndMode=(blockColor)*1 + blockMode * 64
	return blockColorAndMode
end

function unpackColorAndMode(blockColorAndMode)
--	blockColor = math.floor((blockColorAndMode / 4) % 64)
--	blockMode = math.floor(blockColorAndMode % 4)
	blockColor = math.floor((blockColorAndMode / 1) % 64)
	blockMode = math.floor((blockColorAndMode / 64) % 4)
	return blockColor, blockMode
end




-- block types alias block modes
invisibleBlock = 0
nonOpaqueTextureBlock = 1
coloredBlock = 2
opaqueTextureBlock = 3


-- colors
	black = 0
	darkBlue = 1
	blue = 2
	lightBlue = 3
	darkGreen = 4
	green = 8
	lightGreen = 12
	darkRed = 16
	red = 32
	lightRed = 48
	darkCyan = darkGreen + darkBlue
	cyan = blue+green                   
	lightCyan = lightGreen+lightBlue
	darkPurple = darkBlue + darkRed
	purple = blue+red
	lightPurple = lightBlue+lightRed
	oakBrown = darkGreen + darkRed
	darkYellow = green+red
	yellow = lightGreen+lightRed
	grey = darkGreen + darkBlue + darkRed
	silver = blue+green+red
	white = lightRed+lightGreen+lightBlue 

	orange = lightRed+green
	brown = red+darkGreen+darkBlue
	indigo = darkRed+blue
	pearl = lightRed+green+lightBlue
	beige = lightRed+lightGreen+blue
	turquoise = darkRed+lightGreen+blue
	azure = red+lightGreen+lightBlue
	cobaltBlue = darkGreen+lightBlue
	gold = lightRed+lightGreen+darkBlue
	jadeGreen = red+lightGreen+blue
	blueLotus = red+green+lightBlue
	bronze = lightRed+green+darkBlue
	moccasin = red+green+darkBlue
	scarlet = lightRed+darkGreen
	violetRed = lightRed+darkGreen+blue
	wineRed = red+darkBlue
	lapisBlue = darkRed+darkGreen+blue

	-- aliases 
	olive = darkGreen
	lime = lightGreen
	maroon = darkRed
	magenta = lightPurple
	darkForestGreen = darkGreen
	


-- textured block codes
diceTexture = 0
headTexture = 1
canonBox = 5
grass = 6
controlPanel = 7
engineBox = 8


