
-- This script generates other body parts, those extending from body

package.path = game.getPath() .. '/?.lua;' .. package.path
require "MiscLib"





-- This is needed if the LUA script stores data that must be saved when the game is saved to disk. It is important to implement this if there is such data.
function writeSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("AvatarBody writeSelf")
end


-- This shall deserialize all data the was serialized by writeSelf
function readSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("AvatarBody readSelf")
end



-- This function is called by java code when an object is created
function generateSelf ( ... )
	--game.postMessage("Hello from generateSelf!")

	id=game.getId()

	print("generateSelf ~" .. id)

	game.setInfo(id, "roomSizeInBlocks  1 1 1")
	game.setInfo(id, "subDiv  1 1 1")
	game.setInfo(id, "roomSize  0.35 0.5 0.5")
	game.setInfo(id, "voxelXyz 0 0 0  " .. packColorAndMode(darkGreen, coloredBlock))


	childId = game.createChildObj(id, "YukigassenBlockRoom")
	print("neck ~" .. childId )
	game.setInfo(childId, "name neck")
	game.setInfo(childId, "roomSizeInBlocks 1 1 1")
	game.setInfo(childId, "subDiv 1 1 1")
	game.setInfo(childId, "pao 0 0 -0.2  1 0 0  0 0 -1 -")
	game.setInfo(childId, "roomSize 0.2 0.2 0.2")
	game.setInfo(childId, "voxelXyz 0 0 0  " .. packColorAndMode(darkGreen, coloredBlock))


	childId = game.createChildObj(id, "YukigassenBlockRoom")
	print("leftLeg ~" .. childId )
	game.setInfo(childId, "name leftLeg")
	game.setInfo(childId, "roomSizeInBlocks 1 1 1")
	game.setInfo(childId, "subDiv 1 1 1")
	game.setInfo(childId, "pao 0 -0.15 0.5  1 0 0  0 0 -1 -")
	game.setInfo(childId, "roomSize 0.15 0.15 0.4")
	game.setInfo(childId, "voxelXyz 0 0 0  " .. packColorAndMode(darkGreen, coloredBlock))

	childId = game.createChildObj(id, "YukigassenBlockRoom")
	print("rightLeg ~" .. childId )
	game.setInfo(childId, "name rightLeg")
	game.setInfo(childId, "roomSizeInBlocks 1 1 1")
	game.setInfo(childId, "subDiv 1 1 1")
	game.setInfo(childId, "pao 0 0.15 0.5  1 0 0  0 0 -1 -")
	game.setInfo(childId, "roomSize 0.15 0.15 0.4")
	game.setInfo(childId, "voxelXyz 0 0 0  " .. packColorAndMode(darkGreen, coloredBlock))


	childId = game.createChildObj(id, "YukigassenBlockRoom")
	print("leftArm ~" .. childId )
	game.setInfo(childId, "name leftArm")
	game.setInfo(childId, "roomSizeInBlocks 1 1 1")
	game.setInfo(childId, "pao 0 -0.35 -0.03  1 0 0  0 0 -1 -")
	game.setInfo(childId, "roomSize 0.15 0.15 0.4")
	game.setInfo(childId, "voxelXyz 0 0 0  " .. packColorAndMode(darkGreen, coloredBlock))

	childId = game.createChildObj(id, "YukigassenBlockRoom")
	print("rightArm ~" .. childId )
	game.setInfo(childId, "name rightArm")
	game.setInfo(childId, "roomSizeInBlocks 1 1 1")
	game.setInfo(childId, "subDiv 1 1 1")
	game.setInfo(childId, "pao 0 0.35 -0.03  1 0 0  0 0 -1 -")
	game.setInfo(childId, "roomSize 0.15 0.15 0.4")
	game.setInfo(childId, "voxelXyz 0 0 0  " .. packColorAndMode(darkGreen, coloredBlock))

	-- name=game.getInfo(childId, "name");
	-- print("AvatarBody: generateSelf: name of child object is: " .. name)
end




