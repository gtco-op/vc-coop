function onServerStart()
	print("Default Gamemode Started")

	local modelID = 226
	local randColorA = GetRandomVehicleColor()
	local randColorB = GetRandomVehicleColor()
	
	AddVehicle(modelID, 538.117798, 626.908203, 10.901563, randColorA, randColorB)
	AddVehicle(modelID, 540.117798, 630.908203, 10.901563, randColorA, randColorB)
end
function onPlayerConnect(id)
	print("Player "..GetPlayerName(id).. " joined!")

    SetPlayerModel(id, GetRandomModel())
    SetPlayerHealth(id, 100)
    SetPlayerArmour(id, 100)
end
function onPlayerMessage(playerid, msg)
	local playerName = GetPlayerName(playerid);
	local matchText = string.match(msg, "/setskin (%d+)")
	
	if (matchText) then
		SetPlayerModel(playerid, matchText)		
	end
	
	matchText,ammo = string.match(msg, "/weapon (%d+) (%d+)")
	if (matchText and ammo) then
		GivePlayerWeapon(playerid, matchText, ammo)		
		SendPlayerMessage(playerid, "Given weapon "..matchText.." with "..ammo.." ammo.")
	else
		SendPlayerMessage(playerid, "Incorrect arguments!")
		SendPlayerMessage(playerid, "/weapon <weapon id> (ammo)")
	end
end
