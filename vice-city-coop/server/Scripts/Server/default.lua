function onServerStart()
	print("Default Gamemode Started")
	
	--AddVehicle(130, 530.393555, 618.161865, 10.901563)
	--AddVehicle(131, 538.117798, 626.908203, 10.901563)
	
	--AddPed(7, 532.501892, 636.292664, 11.043379)
	--AddObject(381, 538.117798, 626.908203, 10.901563)
end
function onPlayerConnect(id)
	local name = GetPlayerName(id)
	print(name.." connected!")
end