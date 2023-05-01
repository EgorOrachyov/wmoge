--[[ Controls movement and input process for Owl --]]
Owl = { game = nil,
        speed = 0,
        speed_initial = 30,
        rotation_scale = 0.01,
        rotation_eps = 0.1,
        acceleration = -80,
        dead_zone = -2,
        is_dead = false,
        obstacles = nil,
        feathers = nil,
        sparks = nil }

--[[ Setup initials, get game, ui and obstacles --]]
function Owl:on_scene_enter()
    local scene = self.parent:get_scene()

    -- cache refs to labels and obstacles for later use
    self.game = scene:find_child("game"):get("ScriptComponent")
    self.obstacles = self.parent:get_scene():find_child("obstacles")
    self.sprite = self.parent:get("SpriteInstance")

    -- cache ref to particle emitters
    self.sparks = self.obstacles:get("Particles2d")
    self.feathers = self.parent:get("Particles2d")

    -- load and enable game action map for controls
    wmoge.ActionManager.load_action_map("root://actions/action_map_owl.xml")
    wmoge.ActionManager.enable_action_map(wmoge.StringId.new("owl"))
end

--[[ Go to dead state --]]
function Owl:dead()
    self.is_dead = true
    self.sprite:hide()

    wmoge.EventManager.dispatch("dead", nil)

    -- emit death feathers
    local params = wmoge.SpawnParams()
    local emit_pos = self.feathers:get_pos_global()
    local range = 2
    local vel = 3
    for _ = 1, 20 do
        params.amount = 1
        params.pos2d = wmoge.Vec2f.new(emit_pos.x + wmoge.Random.next_float_in_range(-range, range), emit_pos.y + wmoge.Random.next_float_in_range(-range, range))
        params.vel2d = wmoge.Vec2f.new(wmoge.Random.next_float_in_range(-vel, vel), wmoge.Random.next_float_in_range(-vel, vel))
        params.angle = wmoge.Random.next_float_in_range(-2, 2)
        self.feathers:emit(params)
    end
end

--[[ Collect coin if can --]]
function Owl:collect_coin()
    local pos = self.parent:get("Spatial2d"):get_transform().translation.x

    for i = 0, self.game.num_columns - 1 do
        local obstacle = self.obstacles:get_child(i)
        local coin = obstacle:get_child(0):get("SpriteInstance")

        if coin:is_visible() and obstacle:get("Spatial2d"):get_transform().translation.x < pos then
            -- hide coin so it is collected
            coin:hide()

            -- emit sparks where is collected
            local params = wmoge.SpawnParams()
            local emit_pos = self.feathers:get_pos_global()
            for _ = 1, 10 do
                params.amount = 1
                params.pos2d = emit_pos
                params.vel2d = wmoge.Vec2f.new(-wmoge.GameTokenManager.get("scroll_speed"), 0)
                params.angle = wmoge.Random.next_float()
                self.sparks:emit(params)
            end

            -- play collect sound
            self.obstacles:get("AudioSource2d"):play()

            -- update score of the game
            local score = wmoge.GameTokenManager.get("score")
            wmoge.GameTokenManager.set("score", score + 1)

            return
        end
    end
end

--[[ Collide owl and a column using built-in math function --]]
function Owl:collide_obstacle(obstacle)
    local sprite = self.parent:get("SpriteInstance"):get_sprite()
    local spatial = self.parent:get("Spatial2d")
    local matr = spatial:get_matr_global()
    local inv_matr = spatial:get_matr_global_inv()
    local size = sprite:get_size() * 0.5
    local pivot = sprite:get_pivot()

    local other_sprite = obstacle:get("SpriteInstance"):get_sprite()
    local other_spatial = obstacle:get("Spatial2d")
    local other_matr = other_spatial:get_matr_global()
    local other_inv_matr = other_spatial:get_matr_global_inv()
    local other_size = other_sprite:get_size()
    local other_pivot = other_sprite:get_pivot()

    return wmoge.Math2d.intersects(size, other_size, pivot, other_pivot, matr, other_matr, inv_matr, other_inv_matr)
end

--[[ Check collision with column on the scene --]]
function Owl:collide_obstacles()
    for i = 0, self.game.num_columns - 1 do
        local obstacle = self.obstacles:get_child(i)
        local lower = obstacle:find_child("lower")
        local upper = obstacle:find_child("upper")

        if self:collide_obstacle(lower) then
            return true
        end
        if self:collide_obstacle(upper) then
            return true
        end
    end

    return false
end

--[[ Update owl vertical position and check that is not dead --]]
function Owl:on_update(delta_time)
    if not self.is_dead then
        local spatial = self.parent:get("Spatial2d")
        local transform = spatial:get_transform()
        local pos = transform.translation
        local new_pos_x = pos.x
        local new_pos_y = pos.y + self.speed * delta_time
        local new_angle = wmoge.Math.clampf(self.speed * self.rotation_scale,
                -wmoge.Math.HALF_PIf + self.rotation_eps,
                wmoge.Math.HALF_PIf - self.rotation_eps)

        transform.translation = wmoge.Vec2f.new(new_pos_x, new_pos_y)
        transform.rotation = new_angle
        spatial:update_transform(transform)

        if new_pos_y < self.dead_zone or new_pos_y > self.game.screen_size.y then
            self:dead()
        end

        if self:collide_obstacles() then
            self:dead()
        end

        self:collect_coin()
        self.speed = self.speed + self.acceleration * delta_time
    end
end

--[[ On action jump or restart do something if can --]]
function Owl:on_action(action)
    if not self.is_dead and action.name.str == "jump" then
        self.speed = math.max(self.speed, 0)
        self.speed = self.speed + self.speed_initial

        -- emit feather where jump / fly
        local params = wmoge.SpawnParams()
        local emit_pos = self.feathers:get_pos_global()
        params.amount = 1
        params.pos2d = wmoge.Vec2f.new(emit_pos.x, emit_pos.y + wmoge.Random.next_float_in_range(-1, 1))
        params.vel2d = wmoge.Vec2f.new(-wmoge.GameTokenManager.get("scroll_speed"), 0)
        params.angle = wmoge.Random.next_float_in_range(-2, 2)
        self.feathers:emit(params)

        -- play flap sound
        self.parent:get("AudioSource2d"):play()
    end

    if self.is_dead and action.name.str == "restart" then
        local spatial = self.parent:get("Spatial2d")
        local transform = spatial:get_transform()
        local pos = transform.translation
        local new_pos_x = pos.x
        local new_pos_y = self.game.screen_size.y * 0.5

        transform.translation = wmoge.Vec2f.new(new_pos_x, new_pos_y)
        spatial:update_transform(transform)

        self.speed = 0
        self.is_dead = false
        self.sprite:show()

        wmoge.GameTokenManager.set("score", 0)
        wmoge.EventManager.dispatch("restart", nil)
    end
end

return new(Owl, wmoge.SceneObject)