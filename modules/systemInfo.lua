require "ubus"

function getData()
    local systemInfo = {}

    local conn = ubus.connect()
    if not conn then
        return nil
    end

    local status = conn:call("system", "info", {})
    local systemInfo = {}
    if status then
        systemInfo["total"] = status["memory"]["total"]
        systemInfo["free"] = status["memory"]["free"]
        systemInfo["shared"] = status["memory"]["shared"]
        systemInfo["buffered"] = status["memory"]["buffered"]
    end

    return systemInfo
end
