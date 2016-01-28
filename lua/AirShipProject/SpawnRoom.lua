
-- TODO have not managed to get require to work without adding path in lua, want id done from java. Perhaps somewhere here is help? http://stackoverflow.com/questions/32597675/luaj-how-to-import-or-require-a-lua-function-library


package.path = game.getPath() .. '/?.lua;' .. package.path
require "MiscLib"


-- This is needed if the LUA script stores data that must be saved when the game is saved to disk. It is important to implement this if there is such data.
function writeSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("SpawnRoom writeSelf")
end


-- This shall deserialize all data the was serialized by writeSelf
function readSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("SpawnRoom readSelf")
end



-- This function is called by java code when an object is created
function generateSelf ( ... )

	id=game.getId()  -- get the ID of this object
	name=game.getInfo(id, "name")  	-- caller is expected to have set the name already
	print("SpawnRoom: ~" .. id .. " " .. name)

	-- Avoid using sizes that are not a power of 2, the engine might not be able to handle that.
	vSize=1024
	hSize=32

	iSize=128

	game.postMessage(id, "SpawnRoom: Hello from generateSelf!")

	--postMessage(id, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
	--postMessage(id, "MLG!")
	--postMessage(id, "Snart sov dags")
	--postMessage(id, "QUICK SCOPE!!!")

	game.setInfo(id, "roomSize "..vSize.." "..vSize.." "..hSize)         -- room size in meters
	game.setInfo(id, "roomSizeInBlocks "..vSize.." "..vSize.." "..hSize) -- how many blocks each dimension is divided into. If same as room size the blocks will be 1x1x1 meters.
	game.setInfo(id, "roomAcceleration  0 0 5")     -- objects in room will accelerate (fall) towards positive Z (down) with 5 m/s2

	-- fill the spawn room with something to walk on.
	for x=vSize/2-iSize, vSize/2+iSize-1, 1 do  -- LUA for loops go from first to and including the last, stepping 
		for y=vSize/2-iSize, vSize/2+iSize-1, 1 do
			for z=hSize/2+2, hSize, 1 do  -- filling z layers 6 & 7  (not filling layers 0, 1, 2, 3, 4, 5)
				-- d = packColorAndMode(green, coloredBlock)
				d = packColorAndMode(grass, opaqueTextureBlock)
				game.setInfo(id, "voxelXyz " .. x .. " " .. y .. " " .. z .. " " .. d)
			end
		end
	end


	-- Create some ships in the world. Eventually we want ships to be built by players (almost there).

	childId = game.createChildObj(id, "YukigassenBlockRoom")

	print("createChildObj ~" .. childId )
	game.regName(childId, "Dreadnought")
	game.setInfo(childId, "pao  20 20 -7  1 0 0  0 0 -1 -") -- pao = position and orientation, first three numbers are position, then nose vector and then head vector, the last optional field is for velocity, "-" can be used for no velocity.
	game.setInfo(childId, "scriptAndData 'AirShip.lua'")
	game.callScript(childId, "generateSelf", "")
	game.doCmd(childId, "yawLeft ".. 0.2);

	childId = game.createChildObj(id, "YukigassenBlockRoom")

	print("createChildObj ~" .. childId )
	game.regName(childId, "TheHood")
	game.setInfo(childId, "pao  -20 20 -7  1 0 0  0 0 -1 -") -- pao = position and orientation, first three numbers are position, then nose vector and then head vector, the last optional field is for velocity, "-" can be used for no velocity.
	game.setInfo(childId, "scriptAndData 'AirShip.lua'")
	game.callScript(childId, "generateSelf", "")

end


-- This function is called by java code at an unspecified intervall.
-- Lua code may use this to check for changes in surounding objects or its own state/integrity.
-- It could for example check that it has something to stand on and if not initiate falling down.
-- If it is a craft it could check that it has enough lift to keep moving.
--[[function maintenanceTick ( ... )
	print("SpawnRoom maintenanceTick")
end--]]



-- java will call this when it is time to create a new avatar
-- This function must return the new avatars ID, java will depend on that.
function createAvatar( ... )

	a = serial.unserialize(arg[1])


	name = a[1]   -- first argument shall be the name of the avatar to be.
	print("SpawnRoom createAvatar name '"..name.."'")


	id=game.getId()

	avatarId = game.createChildObj(id, "YukigassenAvatar")
	print("SpawnRoom avatarId ~"..avatarId)

	game.regName(avatarId, name)
	game.setInfo(avatarId, "scriptAndData  'AvatarMain.lua'") -- Tell java to use a custom LUA script on this object.
	game.callScript(avatarId, "generateSelf", "")		-- Let the custom script generate the avatar body
	game.callScript(avatarId, "teleportRandom", "")         -- TODO: Perhaps this would be better: game.doCmd(avatarId, "teleportRandom"); or this is not needed at all, the generateSelf in AvatarMain.lua should do it. 

	print("SpawnRoom avatarId ~"..avatarId)

	
	return avatarId
end




