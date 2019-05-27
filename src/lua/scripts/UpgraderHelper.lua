
function makeRequest(context)
	local _, err = pcall (doMakeRequest, context)
	
	if (err) then
		context:setStepName("end")
		context:miner():setStat('failed: ' .. err)
		context:setCanYield(true)
	end
	
end

function makeResult(context, response, stat)
	local _, err = pcall (doMakeResult, context, response, stat)
	
	if (err) then
		context:setStepName("end")
		context:miner():setStat('failed: ' .. err)
		context:setCanYield(true)
	end
end

function doMakeRequest(context)
	local typeStr = context:miner():typeStr()
	local success, minerProcessor = pcall (require, "minerUpgrader." .. typeStr)
    
  if success then
		minerProcessor.doMakeRequest(context)
	else
    print (minerProcessor)
		context:setStepName("end")
		context:miner():setStat("Don't support: " .. typeStr)
		context:setCanYield(true)
  end
end

function doMakeResult(context, response, stat)
  local typeStr = context:miner():typeStr()
	
	if (context:stepName() ~= "doWaitFinish" and stat ~= "success") then
		if (stat == "Element not found") then
			context:miner():setStat("Firmware file not found")
		else
			context:miner():setStat(stat)
		end
		context:setStepName("end")
		context:setCanYield(true)
		return
  end
    
  local success, minerProcessor = pcall (require, "minerUpgrader." .. typeStr)
    
  if success then
		minerProcessor.doMakeResult(context, response, stat)
	else
    print (minerProcessor)
		context:setStepName("end")
		context:miner():setStat("Don't support: " .. typeStr)
		context:setCanYield(true)
  end
end
