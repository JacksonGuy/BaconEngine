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

if get_input("T") then
    object = create_entity("./assets/prefabs/test.json")
    set_entity_position(object, 300, -300)
end

-- TODO
--[[
if get_mouse_input("LEFT") then 
    enemy = create_entity("./enemy.json")       -- Returns ID of created entity
    change_entity_variable(enemy, "Health", 10) -- id, name, value
    change_entity_position(enemy, 10, 20)       -- id, x, y

    target = get_entity_name("Player")
    targets = get_enetity_type("Enemy")
    for id in targets do
        name = get_enetity_name(id)
        ConsoleWrite(name)
    end
end
]]--