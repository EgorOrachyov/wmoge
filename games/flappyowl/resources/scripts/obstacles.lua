--[[ Columns, money generation and movement --]]
Obstacles = { game = nil,
              scroll_speed = 0,
              do_scroll = true }

--[[ Generate scene coins and columns --]]
function Obstacles:generate()
    for i = 0, self.game.num_columns - 1 do
        local transform = wmoge.Transform2d()
        transform:translate(wmoge.Vec2f.new(80 + i * 70, self.game.screen_size.y / 2 + wmoge.Random.next_float_in_range(-20, 20)))

        local obstacle = self.parent:get_child(i)
        obstacle:get_or_create("Spatial2d"):update_transform(transform)

        local coin = obstacle:get_child(0)
        local coin_sprite = coin:get("SpriteInstance")
        coin_sprite:show()
        coin_sprite:play_animation(0)
    end

    wmoge.Log.log(wmoge.Log.Info, "generate scene obstacles", "obstacles.lua", "generate", 21)
end

function Obstacles:on_scene_enter()
    self.game = self.parent:get_scene():find_child("game"):get("ScriptComponent")
    self.scroll_speed = self.game.scroll_speed_initial

    wmoge.GameTokenManager.set("scroll_speed", 0)

    self.on_restart = wmoge.EventManager.subscribe("restart", function(_)
        self:generate()
        self.do_scroll = true
        self.scroll_speed = self.game.scroll_speed_initial
    end)

    self.on_dead = wmoge.EventManager.subscribe("dead", function(_)
        self.do_scroll = false
    end)
end

--[[ Unsubscribe callbacks for events --]]
function Obstacles:on_scene_exit()
    self.on_restart:unsubscribe()
    self.on_dead:unsubscribe()
end

--[[ Move columns towards player, if first out of the screen, push it back --]]
function Obstacles:on_update(delta_time)
    if self.do_scroll then
        local shift = -self.scroll_speed * delta_time
        self.scroll_speed = self.scroll_speed + self.game.scroll_acceleration * delta_time

        wmoge.GameTokenManager.set("scroll_speed", self.scroll_speed)

        for i = 0, self.game.num_columns - 1 do
            local obstacle = self.parent:get_child(i)
            local spatial = obstacle:get("Spatial2d")
            local transform = spatial:get_transform()

            transform:translate(wmoge.Vec2f.new(shift, 0))

            if transform.translation.x < -10 then
                local new_pos_x = transform.translation.x + 70 * self.game.num_columns
                local new_pos_y = self.game.screen_size.y / 2 + wmoge.Random.next_float_in_range(-20, 20)
                transform.translation = wmoge.Vec2f.new(new_pos_x, new_pos_y)
                obstacle:get_child("coin"):get("SpriteInstance"):show()
            end

            spatial:update_transform(transform)
        end
    end
end

return new(Obstacles, wmoge.SceneObject)