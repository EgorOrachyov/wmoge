--[[ game ui system --]]
UI = { label_score = nil,
       label_dead = nil,
       label_restart = nil }

function UI:on_scene_enter()
    self.label_score = self.parent:find_child("score"):get("CanvasText")
    self.label_dead = self.parent:find_child("dead"):get("CanvasText")
    self.label_restart = self.parent:find_child("restart"):get("CanvasText")

    wmoge.GameTokenManager.set("score", 0)

    self.on_restart = wmoge.EventManager.subscribe("restart", function(_)
        self.label_dead:hide()
        self.label_restart:hide()
    end)

    self.on_dead = wmoge.EventManager.subscribe("dead", function(_)
        self.label_dead:show()
        self.label_restart:show()
        self.parent:get("AudioSource2d"):play()
    end)
end

--[[ Unsubscribe callbacks for events --]]
function UI:on_scene_exit()
    self.on_restart:unsubscribe()
    self.on_dead:unsubscribe()
end

function UI:on_token(event)
    if event.token.str == "score" then
        score = wmoge.GameTokenManager.get("score")
        self.label_score:set_text("score: " .. tostring(math.floor(score)))
    end
end

return new(UI, wmoge.SceneObject)