-- AvatarMain.lua


-- This script uses some functions in MiscLib.lua, these lines make that possible.
package.path = game.getPath() .. '/?.lua;' .. package.path
require "MiscLib"



-- This is needed if the LUA script stores data that must be saved when the game is saved to disk. It is important to implement this if there is such data.
function writeSelf ( ... )
	print("AvatarMain writeSelf ".. inventoryCounter)

	return inventoryCounter
end


-- This shall deserialize all data the was serialized by writeSelf
function readSelf ( ... )
	print("AvatarMain readSelf")

	if argum then

		a = serial.unserialize(argum)
		inventoryCounter = tonumber(a[1])
	end

	if inventoryCounter == nil then
		inventoryCounter = 0
	end
end



-- This function is called by java code when an object is created
function generateSelf ( ... )
	id=game.getId() -- Id of this object

	print("AvatarMain generateSelf ~" .. id )

	--math.randomseed(os.time())
	--math.random(); math.random(); math.random()
	--math.random(); math.random(); math.random()

	-- send a message to others that this avatar joined the world.
	game.doCmd(id, "say Hello from AvatarMain generateSelf!")

	childId = game.createChildObj(id, "YukigassenBlockRoom")
	game.setInfo(childId, "name head")
	game.setInfo(childId, "roomSizeInBlocks 1 1 1")
	game.setInfo(childId, "pao 0 0 0  1 0 0  0 0 -1 -")
	game.setInfo(childId, "roomSize 0.35 0.35 0.4")
	game.setInfo(childId, "voxelXyz 0 0 0  ".. packColorAndMode(headTexture, opaqueTextureBlock))

	childId = game.createChildObj(id, "YukigassenBlockRoom")
	game.setInfo(childId, "name  body")
	game.setInfo(childId, "pao  0 0 0.5  1 0 0  0 0 -1 -") -- pao = position and orientation, first three numbers are position, then nose vector and then head vector, the last optional field is for velocity, "-" can be used for no velocity.
	game.setInfo(childId, "scriptAndData 'AvatarBody.lua'")
	game.callScript(childId, "generateSelf", "")

--[[

	-- The avatar may pick up things, so we need some place to put those things. Shall we call it inventory or pocket or something? Will call it inv for now.
	invId = game.createChildObj(id, "RsbLong")
	game.setInfo(invId, "name  _inv")

	
	
	for i=0, 64, 1 do
		invItemId = game.createChildObj(invId, "RsbInvItem")
		game.setInfo(invItemId, "name color"..i)
		game.setInfo(invItemId, "value color"..i)
		game.setInfo(invItemId, "numberOf 10")
		
	end
	
--]]

	inventoryCounter=0
end

-- If a player falls of the world and needs to teleport back, this function is called.
function teleportRandom ( ... )
	id=game.getId() -- Id of this object

	-- For now just teleport to center of world
	x100=game.random(100)
	y100=game.random(100)
	z100=game.random(100)

	print("AvatarMain teleportRandom "..x100.." "..y100.." "..z100)

	x=tonumber(x100)-50
	y=tonumber(y100)-50
	z=tonumber(z100)-50

	game.setInfo(id, "pos  "..x.." "..y.." "..z)
end


-- This override is used when the avatar is hit by something.
function handleCollision ( ... )

	a = serial.unserialize(arg[1])
	-- print(a)

	otherObjId = a[1]

	print("AvatarMain handleCollision with ~" .. otherObjId)

	otherObjName = game.getInfo(otherObjId, "name")
	otherObjOwnerId = game.getInfo(otherObjId, "ownerId")
	ownerName = game.getInfo(otherObjOwnerId, "name")
	id=game.getId() -- Id of this object
	n=game.getInfo(id, "name")

	-- Ignore if it was own snowball
	if not (id==otherObjOwnerId) then
		game.postMessage(id, "You " .. n .. "~" .. id .. " where hit by " .. otherObjName .."~" .. otherObjId .. " thrown by " .. ownerName .. "~" .. otherObjOwnerId)
		game.postMessage(otherObjOwnerId, "You " .. ownerName .."~" .. otherObjOwnerId .. " hit " .. n .."~" .. id)
	end
end

-- Get XYZ as 3 individual variables from a vector
function getXYZ(vector)
	words = {}

	for word in vector:gmatch("%S+") do table.insert(words, word) end
	x =  tonumber(words[1])
	y = tonumber(words[2])
	z = tonumber(words[3])

	print("xyz " .. x .. " " .. y .. " " .. z)
	return x,y,z
end


function throwSnowball ( argum )

	a = serial.unserialize(argum)

	color = a[1]
	mode = a[2]

	id=game.getId() -- Id of this object

	print("AvatarMain throwSnowball " .. color .. " " .. mode)

	parentId = game.getParentId(id)

	avatarAmmo =  tonumber(game.getInfo(id, "avatarAmmo")) -- how much amo the avatar have
	pos = game.getInfo(id, "pos")
	nose = game.getInfo(id, "wantedNose") -- using wantedNose instead of nose since nose will be adjusted to horizon, actually perpendicular to gravitation.


	

	if avatarAmmo > 1000 then


		childId = game.createChildObj(parentId, "YukigassenBillboard")

		game.setInfo(childId, "ownerId " .. id)
		game.setInfo(childId, "pos " .. pos)
		game.setInfo(childId, "roomSize 0.1 0.1 0.1")
		game.setInfo(childId, "entityColor " .. color)
		game.setInfo(childId, "entityThrowMode " .. mode)
		game.regName(childId, "snowball")
		game.doCmd(id,"moveForward 0.3"); -- move it a little forward so we avoid hitting ourselves. Distance is given in meters.

		--[[
			-- nose is a 3 dimensional vector. To scale it We need to get the x y z from it.
			x,y,z=getXYZ(nose);

			-- Here we set the speed of the snow ball. Multiply by 15 will give it 15m/s.
			x = x * 15;
			y = y * 15;
			z = z * 15;

			game.setInfo(childId, "objectVelocity " .. x .. " " .. y .. " " .. z)
		--]]
		-- Instead of the nose to xyz above velocity can be set using nose and the scaleSpeed command
		game.setInfo(childId, "objectVelocity " .. nose)
		game.doCmd(childId,"scaleSpeed 15")
		-- keeping it as an excercise in LUA

		avatarAmmo = avatarAmmo - 100
		game.setInfo(id, "avatarAmmo " .. avatarAmmo)

	end

end

-- This is called when player issues a custom command (cc).
function interpretCustomCommand ( argum )

	a = serial.unserialize(argum)

	cmd = a[1]

	id=game.getId() -- Id of this object

	print("AvatarMain interpretCustomCommand ~" .. id .. " " .. cmd)

	if cmd=="help" then
		
		return "custom commands: createAirShip"
	end
	
	if cmd=="createAirShip" or cmd=="cas" then
		parentId = game.getParentId(id)

		myPao=game.getInfo(id, "pao")
		myName=game.getInfo(id, "name")
		name="AirShip_" .. myName .. "_".. inventoryCounter

		childId = game.createChildObj(parentId, "YukigassenBlockRoom")

		print("createChildObj ~" .. childId.." "..name.." "..myPao)
		game.regName(childId, name)
		game.setInfo(childId, "pao " .. myPao) -- pao is short for positionAndOrientation
		game.doCmd(childId, "moveForward 20")
		game.doCmd(childId, "moveUp 7.5")
		game.setInfo(childId, "scriptAndData  AirShip.lua")
		game.callScript(childId, "generateSelf", "")

		inventoryCounter=inventoryCounter+1

		return "custom air ship id ~" .. childId
	end

	if cmd=="createCannon" or cmd=="cc" then
		parentId = game.getParentId(id)

		myPao=game.getInfo(id, "pao")
		myName=game.getInfo(id, "name")
		name="Cannon_" .. myName .. "_".. inventoryCounter

		childId = game.createChildObj(parentId, "YukigassenBlockRoom")

		print("createChildObj ~" .. childId.." "..name.." "..myPao)
		game.regName(childId, name)
		game.setInfo(childId, "pao " .. myPao) -- pao is short for positionAndOrientation
		game.doCmd(childId, "moveForward 3")
		game.doCmd(childId, "moveUp 0")
		game.setInfo(id, "objectVelocity 0 0 0")	
		game.setInfo(childId, "scriptAndData  Cannon.lua")
		game.callScript(childId, "generateSelf", "")

		inventoryCounter=inventoryCounter+1

		return "custom cannon id ~" .. childId
	end


	return "custom command " .. cmd .. " not found"
end








