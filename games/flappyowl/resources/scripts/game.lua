--[[ shared game state, global params and movement speed --]]
Game = { num_columns = 4,
         scroll_speed_initial = 10,
         scroll_speed_initial_back = 4,
         scroll_acceleration = 0.4,
         scroll_acceleration_back = 0.2,
         screen_size = wmoge.Vec2f.new(128, 72) }

return new(Game, wmoge.SceneObject)