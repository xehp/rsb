-- This script defines a small ship in the airship game.

package.path = game.getPath() .. '/?.lua;' .. package.path
require "MiscLib"


-- color codes
baloonGas=63
cannonBase=0
engineBlock=21



liftBlockCounter = 0
wieghtBlockCounter = 0
totalBuoyancy = 0



function checkLiftBlock (prevRealColor, newRealColor, a)


	
	if (prevRealColor == baloonGas) and (not (newRealColor == baloonGas)) then
		liftBlockCounter = liftBlockCounter - 1 -- Boyance block added
		print("DECREASE")
	elseif (not (prevRealColor == baloonGas)) and (newRealColor == baloonGas) then
		liftBlockCounter = liftBlockCounter + 1 -- Boyance block removed
		print("INCREASE")
	end


	if ( (not (newRealColor == baloonGas)) and (not (prevRealColor == baloonGas)) ) then

		if (tonumber(a[2])==0) and (not (tonumber(a[1])==0)) then
			wieghtBlockCounter = wieghtBlockCounter - 1 -- Wieght block removed		
			print("BLOCKREMOVED")
		elseif (not (tonumber(a[2])==0)) and (tonumber(a[1])==0) then
			wieghtBlockCounter = wieghtBlockCounter + 1 -- Wieght block added		
			print("BLOCKADDED")
		end
		if (not (prevRealColor == baloonGas)) and (not (newRealColor == baloonGas)) then
			-- There was no change that was of interest
			print("NO CHANGE IS NEEDED")

		end

	end

end


-- This is called by java whenever a voxel is changed within this object.
function voxelUpdate(argum)
	a = serial.unserialize(argum)


	prevRealColor, prevRealMode = unpackColorAndMode(tonumber(a[1]))
	newRealColor, newRealMode = unpackColorAndMode(tonumber(a[2]))

	x= tonumber(a[3])
	y= tonumber(a[4])
	z= tonumber(a[5])
	print("AirShip voxelUpdate: x="  ..  x .. ", y="..y..", z="..z .. ", prevRealColor="..prevRealColor..", prevRealMode="..prevRealMode..", newRealColor="..newRealColor .. ", newRealMode=" .. newRealMode)
	

	checkLiftBlock (prevRealColor, newRealColor, a)

	totalBuoyancy = wieghtBlockCounter - liftBlockCounter
	print("Buoyancy is "..totalBuoyancy)		

end




-- This is needed if the LUA script stores data that must be saved when the game is saved to disk. It is important to implement this if there is such data.
function writeSelf ( ... )

	print("AirShip  writeSelf")
	return "\"".. liftBlockCounter.."\" \""..wieghtBlockCounter.."\""
end


-- This shall deserialize all data the was serialized by writeSelf
function readSelf ( argum )

	print("AirShip  readSelf")
	a = serial.unserialize(argum)
	liftBlockCounter = tonumber(a[1])
	if liftBlockCounter == nil then
		liftBlockCounter = 0
	end
	wieghtBlockCounter = tonumber(a[2])
	if wieghtBlockCounter == nil then
		wieghtBlockCounter = 0
	end
end



-- This function is called by java code when an object is created
function generateSelf ( ... )


	id=game.getId()

	print("AirShip generateSelf ~" .. id)

	lSize=16
	wSize=lSize/2
	gravity=5  -- m/s2

	game.setInfo(id, "roomSize "..lSize.." "..wSize.." "..wSize)
	game.setInfo(id, "roomSizeInBlocks "..lSize.." "..wSize.." "..wSize)
	game.setInfo(id, "roomAcceleration  0 0 "..gravity) -- objects in room will accelerate (fall) towards positive Z (down) with 5 m/s2

	cm=packColorAndMode(black, coloredBlock)
	
	-- placing two blocks. The nose of the craft is in positive X axis. So x=0 is in the stern.
	game.setInfo(id, "voxelXyz 0 2 6 "..cm) -- set one voxel in the corner x=0,y=2,z=6. This is in the stern of the craft. 
	game.setInfo(id, "voxelXyz 0 5 6 "..cm) -- set one more voxel in the corner 

	-- fill the room with something
	
	-- this makes a "roof"
	cm=packColorAndMode(white, coloredBlock)  -- color 63 is white, mode 1 is opaque
	for x=0, lSize-1, 1 do  -- LUA for loops go from first to and including the last, stepping 
		for y=0, wSize-1, 1 do
			for z=0, wSize/2, 1 do  -- filling z layers 0, 1, 2, NOTE Z layer 0 is the top layer.
				game.setInfo(id, "voxelXyz " .. x .. " " .. y .. " " .. z .. " "..cm)
			end
		end
	end

	-- this makes a floor
	cm=packColorAndMode(green, coloredBlock)
	for x=0, lSize-1, 1 do  -- LUA for loops go from first to and including the last, stepping 
		for y=0, wSize-1, 1 do
			for z=wSize-1, wSize-1, 1 do  -- filling z layer 7
				game.setInfo(id, "voxelXyz " .. x .. " " .. y .. " " .. z .. " "..cm)
			end
		end
	end


	-- add a cannon. 
	--[[
	childId = game.createChildObj(id, "YukigassenBlockRoom")
	print("createChildObj ~" .. childId )
	game.setInfo(childId, "name Cannon")
	game.setInfo(childId, "pao  2.0 0 2.5   1 0 0  0 0 -1 -") -- pao = position and orientation, first three numbers are position, then nose vector and then head vector, the last optional field is for velocity, "-" can be used for no velocity. Origo for positions is in the center of the ship. So 2.0 0 1.0 is a bit forward and down a little. 
	game.setInfo(childId, "scriptAndData  'Cannon.lua'")
	game.callScript(childId, "generateSelf", "")
	--]]

	forwardVelocity=0

end

prevTickMs=-1  -- used to calculate how mush time has passed since previous tick
--yawRate=0       -- unit is radians per milli second
-- moveSpeed=0  -- unit is meters per milli second

forwardVelocity=0
turnLeftRate=0
downVelocity=0

function checkMovement ( ... )

	if totalBuoyancy > 0 then
		-- The ships is to heavy to float. DROP IT!
		downVelocity = 1
	end


	game.setInfo(id, "yawLeftRate ".. turnLeftRate) 
	if downVelocity==0 then
		game.doCmd(id, "forwardVelocity "..forwardVelocity);
	else
		game.doCmd(id, "downVelocity " .. downVelocity)
	end

	-- If Ship is not moving and not turning/rotating/spinning/yawing then turn of velocity (it uses a little less CPU time that way)
	if forwardVelocity==0 and turnLeftRate==0 and downVelocity==0 then

		game.setInfo(id, "objectVelocity -")	
	end

end



-- This function is called by java code at an unspecified interval.
-- LUA code may use this to check for changes in surrounding objects or its own state/integrity.
-- It could for example check that it has something to stand on and if not initiate falling down.
-- If it is a craft it could check that it has enough lift to keep moving.
function slowTick ( argum )
	a = serial.unserialize(argum)
	-- print(a)

	tickMs = tonumber(a[1])
	id=game.getId()

	-- TODO the argument to slowTick shall be delta time not accumulated time.
	deltaMs=0
	if (prevTickMs>=0) then
		deltaMs=tickMs-prevTickMs
		-- TODO if we burn fuel do it here since delta time is needed to calculate that
	end
	
	checkMovement()

	
	-- print("Dreadnought maintenanceTick "..prevTickMs.." "..tickMs.." ~"..id)


--[[

	ctrlBlock=tonumber(game.getInfo(id, "voxelXyz 0 5 6"))
	blockColor,blockMode=unpackColorAndMode(ctrlBlock)
	-- print("ctrlBlock "..ctrlBlock.." blockColor "..blockColor.." blockMode "..blockMode)


	nose=game.getInfo(id, "nose");

	if blockColor==1 then
		game.doCmd(id, "forwardVelocity "..-1); 
	elseif blockColor==2 then
		game.setInfo(id, "objectVelocity " .. nose) 
	else
		game.setInfo(id, "objectVelocity 0 0 0") 
	end




	ctrlBlock=tonumber(game.getInfo(id, "voxelXyz 0 2 6"))
	blockColor,blockMode=unpackColorAndMode(ctrlBlock)
	-- print("ctrlBlock "..ctrlBlock.." blockColor "..blockColor.." blockMode "..blockMode)


	if blockColor==1 then
		game.setInfo(id, "yawLeftRate 0.1")
	elseif blockColor==2 then
		game.setInfo(id, "yawLeftRate -0.1")
	else
		game.setInfo(id, "yawLeftRate 0") 
	end

--]]
	
	-- Set the velocity every tick since if ship is also turning the velocity needs to reflect the current direction of the ship not the direction it had when movement began.






	prevTickMs = tickMs;

end


inventoryCounter = 0

-- This is called when the player looks at a control panel and presses a control key.
-- First three digits tells which panel it was if the ship has more than one. 
-- The forth digit tells which control key it was.
function panelAction ( argum )

	a = serial.unserialize(argum)
	-- print(a)

	x = tonumber(a[1])
	y = tonumber(a[2])
	z = tonumber(a[3])
	cmd = a[4]
	id=game.getId()


	print("AirShip panelAction "..x.." "..y.." "..z.." "..cmd);

	-- TODO Only listen if x y z is coordinates (actually index not coordinates) of the control panel for the ship

	-- This is just to test getChildIds, remove this later, it is not needed.
	-- list=game.getChildIds(id)
	-- print("List of child object IDs "..list);


	if cmd=="off" then
		forwardVelocity=0
		turnLeftRate=0
		climbSpeed=0
		checkMovement()
		return "stop"

	elseif cmd=="left" then
		turnLeftRate=0.1
		checkMovement()
		return "turning left"
	elseif cmd=="center" then
		turnLeftRate=0
		checkMovement()
		return "going straight ahead"
	elseif cmd=="right" then
		turnLeftRate=-0.1
		checkMovement()
		return "turning right"

	elseif cmd=="reverse" then
		forwardVelocity=-2.0
		checkMovement()
		return "moving backwards"
	elseif cmd=="stop" then
		forwardVelocity=0
		checkMovement()
		return "not moving forward or backward"
	elseif cmd=="forward" then
		forwardVelocity=2.0
		checkMovement()
		return "moving forward"

	elseif cmd=="up" then
		downVelocity=-0.2
		checkMovement()
		return "climbing"
	elseif cmd=="level" then
		downVelocity=0
		checkMovement()
		return "not climing or sinking"
	elseif cmd=="down" then
		downVelocity=0.2
		checkMovement()
		return "sinking"

	elseif cmd=="on" then

		blockColor=tonumber(game.getInfo(id, "voxelColor ".. x .. " "..y.." "..z))

		if blockColor == cannonBase and x>=0 and y>=0 and z>=0 then

			name="Cannon_" .. game.getInfo(id, "name") .. "_".. inventoryCounter
			pos=game.doCmd(id, "translateBlockXyzIndexToPos " .. x .. " "..y.." "..z)

			childId = game.createChildObj(id, "YukigassenBlockRoom")

			print("createChildObj ~" .. childId.." "..name.." "..pos)

			game.setInfo(id, "voxelXyz "..x.." "..y.." "..z.. " 0")

			game.regName(childId, name)
			game.setInfo(childId, "pos " .. pos) 
			game.setInfo(childId, "scriptAndData  'Cannon.lua'")
			game.callScript(childId, "generateSelf", "")

			inventoryCounter=inventoryCounter+1

			return "custom cannon id ~" .. childId
		else
			return "you need a black block to turn into a cannon"

		end

	else
		return "AirShip did not understand command "..cmd
	end		


end


