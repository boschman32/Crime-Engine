function OnStartGuardA(guardId)
	print("[Lua] On start guard A")
    GuardSetFoV(guardId, 90)
end

function OnPlayerSeenGuardA(guardId, playerId)
    GuardShoot(guardId, playerId, 50)
end