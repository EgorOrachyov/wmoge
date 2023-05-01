--[[ Controls movement of the far background with sky --]]
Background = { sprite_instance = nil,
               sprite_size = nil,
               material = nil,
               game = nil,
               scroll = 0,
               scroll_speed = 0,
               do_scroll = true }

--[[ On enter acquire background sprite which will be animated --]]
function Background:on_scene_enter()
    self.sprite_instance = self.parent:get("SpriteInstance")
    self.sprite_size = self.sprite_instance:get_sprite():get_size()
    self.material = self.sprite_instance:get_material()
    self.game = self.parent:get_scene():find_child("game"):get("ScriptComponent")
    self.scroll = 0
    self.scroll_speed = self.game.scroll_speed_initial_back

    self.on_restart = wmoge.EventManager.subscribe("restart", function(_)
        self.do_scroll = true
        self.scroll = 0
        self.scroll_speed = self.game.scroll_speed_initial
    end)

    self.on_dead = wmoge.EventManager.subscribe("dead", function(_)
        self.do_scroll = false
    end)
end

--[[ Unsubscribe callbacks for events --]]
function Background:on_scene_exit()
    self.on_restart:unsubscribe()
    self.on_dead:unsubscribe()
end

--[[ On update increase scroll on uv axis and shift sprite texture uv --]]
function Background:on_update(delta_time)
    if self.do_scroll then
        self.scroll = self.scroll + self.scroll_speed * delta_time
        self.scroll_speed = self.scroll_speed + self.game.scroll_acceleration_back * delta_time
        self.material:set_vec2(wmoge.StringId.new("uv_offset"), wmoge.Vec2f.new(self.scroll / self.sprite_size.x, 0))
    end
end

return new(Background, wmoge.SceneObject)