
-- package.path = game.getPath() .. '/?.lua;' .. package.path
-- require "MiscLib"


-- This is needed if the LUA script stores data that must be saved when the game is saved to disk. It is important to implement this if there is such data.
function writeSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("Cannon writeSelf")
end


-- This shall deserialize all data the was serialized by writeSelf
function readSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("Cannon readSelf")
end



-- This function is called by java code when an object is created
function generateSelf ( ... )
	--game.postMessage("Hello from generateSelf!")

	id=game.getId()

	print("Cannon generateSelf ~" .. id)

	game.setInfo(id, "roomSizeInBlocks  2 2 2")
	game.setInfo(id, "roomSize  1 1 1")
	game.setInfo(id, "voxelXyz 0 0 0  23")
	game.setInfo(id, "voxelXyz 0 0 1  23")
	game.setInfo(id, "voxelXyz 0 1 0  23")
	game.setInfo(id, "voxelXyz 0 1 1  23")
	game.setInfo(id, "voxelXyz 1 0 0  23")
	game.setInfo(id, "voxelXyz 1 0 1  23")
	game.setInfo(id, "voxelXyz 1 1 0  23")
	game.setInfo(id, "voxelXyz 1 1 1  23")

	-- Create the barrel
	childId = game.createChildObj(id, "YukigassenBlockRoom")
	print("Cannon barrel" .. childId )
	game.setInfo(childId, "name barrel")
	game.setInfo(childId, "roomSizeInBlocks 2 2 2")
	game.setInfo(childId, "pao 0.5 0 -0.2  1 0 0  0 0 -1 -")
	game.setInfo(childId, "roomSize 1.5 0.3 0.3")
	game.setInfo(childId, "voxelXyz 0 0 0  23")
	game.setInfo(childId, "voxelXyz 0 0 1  23")
	game.setInfo(childId, "voxelXyz 0 1 0  23")
	game.setInfo(childId, "voxelXyz 0 1 1  23")
	game.setInfo(childId, "voxelXyz 1 0 0  23")
	game.setInfo(childId, "voxelXyz 1 0 1  23")
	game.setInfo(childId, "voxelXyz 1 1 0  23")
	game.setInfo(childId, "voxelXyz 1 1 1  23")
	game.doCmd(childId, "pitchUp ".. 0.3);

end


-- This is called when player is close to the object and issues a "do" command.
function panelAction ( argum )

	a = serial.unserialize(argum)
	-- print(a)

	x = tonumber(a[1])
	y = tonumber(a[2])
	z = tonumber(a[3])
	order = a[4]
	id=game.getId()


	print("Cannon panelAction "..x.." "..y.." "..z.." "..order)




	if order=="off" or order=="fire" then
		-- Using off for fire, a little awkward but all others are used for aiming.

		parentId = game.getParentId(id)
		barrelId = game.findChildIdByNameOrIndex(id, "barrel")
		if (barrelId<0) then
			print("cannon has no barrel, can't shoot.")
		else
			print("parent ID is ~"..parentId)
			print("barrel ID is ~"..barrelId)
	 
			-- We need sound effects, until then just say boom. Say will say something to all in same room so we don't need to use parentId here.
			game.doCmd(id, "say BOOM")
			game.doCmd(parentId, "say boom")

			childId = game.createChildObj(barrelId, "YukigassenBillboard")

			game.setInfo(childId, "ownerId " .. barrelId)
			game.setInfo(childId, "pos 2 0 0")
			game.setInfo(childId, "scriptAndData  'cannonBall.lua'")
			game.callScript(childId, "generateSelf", "")
			game.regName(childId, "cannonBall")
		end
				
	elseif order=="down" then
		barrelId = game.findChildIdByNameOrIndex(id, "barrel")
		game.doCmd(barrelId, "pitchUp -0.1")
	elseif order=="left" then
		game.doCmd(id, "yawLeft 0.1")
	elseif order=="right" then
		game.doCmd(id, "yawLeft -0.1")
	elseif order=="up" then
		barrelId = game.findChildIdByNameOrIndex(id, "barrel")
		game.doCmd(barrelId, "pitchUp 0.1")
	elseif order=="help" then
		return "commands: up, down, left, right, fire"
	elseif order=="starboard" then
		game.setInfo(id, "nose 0 1 0")
		game.setInfo(id, "head 0 0 -1")
	elseif order=="port" then
		game.setInfo(id, "nose 0 -1 0")
		game.setInfo(id, "head 0 0 -1")
	elseif order=="stern" then
		game.setInfo(id, "nose -1 0 0")
		game.setInfo(id, "head 0 0 -1")
	elseif order=="bow" then
		game.setInfo(id, "nose 1 0 0")
		game.setInfo(id, "head 0 0 -1")
	else
		print("Cannon did not understand order "..order)
		return
	end		

	return "OK"
end


-- This is called when someone say something to this object.
--[[
function postMessage( argum )

	a = serial.unserialize(argum)
	print("cannon postMessage received " .. a)
end
--]]


-- Mode and color are packed into one byte. If BitsUsedForBlockMode in java is changed some change here will also be needed. 
function unpackColorAndMode(blockColorAndMode)
	blockColor = math.floor((blockColorAndMode / 4) % 64)
	blockMode = math.floor(blockColorAndMode % 4)
	return blockColor, blockMode
end


-- This is called by java whenever a voxel is changed within this object.
function voxelUpdate(argum)
	a = serial.unserialize(argum)


	prevRealColor, prevRealMode = unpackColorAndMode(tonumber(a[1]))
	newRealColor, newRealMode = unpackColorAndMode(tonumber(a[2]))
	x= tonumber(a[3])
	y= tonumber(a[4])
	z= tonumber(a[5])

	print("Cannon voxelUpdate: x="  ..  x .. ", y="..y..", z="..z .. ", prevRealColor="..prevRealColor..", prevRealMode="..prevRealMode..", newRealColor="..newRealColor .. ", newRealMode=" .. newRealMode)

	if newRealMode==0 and x==0 and y==0 and z==0 then
		id=game.getId()
		print("Cannon voxelUpdate delete ~" .. id)
		game.unlinkRecursive(id)
	end

end




