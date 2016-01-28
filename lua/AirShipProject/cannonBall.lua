
-- package.path = game.getPath() .. '/?.lua;' .. package.path
-- require "MiscLib"


-- This is needed if the LUA script stores data that must be saved when the game is saved to disk. It is important to implement this if there is such data.
function writeSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("cannonBall writeSelf")
end


-- This shall deserialize all data the was serialized by writeSelf
function readSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("cannonBall readSelf")
end



-- This function is called by java code when an object is created or is it called only from other LUA scripts? Don't remember.
function generateSelf ( ... )
	--game.postMessage("Hello from generateSelf!")

	id=game.getId()

	print("cannonBall generateSelf ~" .. id)

	game.setInfo(id, "roomSize 0.25 0.25 0.25")
	game.setInfo(id, "objectVelocity " .. 30 .. " " .. 0 .. " " .. 0)
	game.setInfo(id, "entityColor 46")
	game.setInfo(id, "entityMode 2")

end



function hitOnBlock ( argum )

	a = serial.unserialize(argum)

	roomHitId=tonumber(a[1])

	hx = tonumber(a[2])
	hy = tonumber(a[3])
	hz = tonumber(a[4])

	id=game.getId()


	print("cannonBall hitOnBlock "..id.."  "..roomHitId.." "..hx.." "..hy.." "..hz)

	-- This will stop the object from moving. Does not matter here since it is goung to be unlinked soon anyway.
	game.setInfo(id, "objectVelocity -")

	-- We shall have some sort of destruction here. Loop nearest voxels.
	for x=-2, 2, 1 do  -- LUA for loops go from first to and including the last, stepping 
		for y=-2, 2, 1 do
			for z=-2, 2, 1 do
				tx=hx+x
				ty=hy+y
				tz=hz+z
				-- print("set voxel "..tx.." "..ty.." "..tz)
				game.setInfo(roomHitId, "voxelXyz " .. tx .. " " .. ty .. " " .. tz .. " 0")
			end
		end
	end

	-- The cannonball itself shall now be removed from the game.
	game.unlinkObj(id)

	-- Do not do anything more with the object once it is unlinked.
end





