speed = get_variable("Speed")

if get_input("A") then
    if check_collision_side("LEFT") == false then
        x,y = get_position()
        set_position(x - speed, y)
    end
end

if get_input("D") then
    if check_collision_side("RIGHT") == false then
        x,y = get_position()
        set_position(x + speed, y)
    end
end

if get_input("SPACE") then
    if check_collision_side("TOP") == false and get_grounded() == true then
        velx, vely = get_velocity()
        set_velocity(velx, -10)
    end 
end

--[[
-- Top down movement
if get_input("W") then
    if check_collision_side("TOP") == false then
        x,y = get_position()
        set_position(x, y - speed)
    end
end

if get_input("S") then
    if check_collision_side("BOTTOM") == false then
        x,y = get_position()
        set_position(x, y + speed)
    end
end
]]--