function OnStartGuardB(guardId)
	print("[Lua] On start guard B")
    GuardSetFoV(guardId, 180)
end

function OnPlayerSeenGuardB(guardId, playerId)
	playerPosX = GetPlayerPositionX()
	playerPosY = GetPlayerPositionY()
	GuardMove(guardId, playerPosX, playerPosY)
	GuardMelee(guardId, playerId, 10)
end