
-- This is the main LUA script for a game.
-- It is used to tell what other scripts to use.


-- This is needed if the LUA script stores data that must be saved when the game is saved to disk. It is important to implement this if there is such data.
function writeSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("WorldMain writeSelf")
end


-- This shall deserialize all data the was serialized by writeSelf
function readSelf ( ... )
	-- Do nothing, No data is currently stored in the script
	print("WorldMain readSelf")
end



-- This function is called by java code when the world is created
function generateSelf ( ... )
	id=game.getId()

	-- What LUA version do we have
	print(_VERSION)

	-- create the spawn room
	childId = game.createChildObj(id, "YukigassenChunkRoom")
	game.regName(childId, "spawnRoom") -- one room (and only one) shall have name "spawnRoom", game engine need that to know where new players can spawn. Other rooms can have any other name but not "spawnRoom".
	game.setInfo(childId, "scriptAndData  'SpawnRoom.lua'") -- Tell game engine which script to use to generate the world
	game.callScript(childId, "generateSelf", "") -- Now generate using the script set in the line above.

	-- tell game engine we want to use a custom script to create avatars
	-- game.setInfo(id, "customAvatarScript AvatarMain.lua")

end



