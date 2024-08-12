# Lua Scripting

## Contents
1. [Entity](#Entity)
2. [Text](#Text)
3. [Camera](#Camera)
4. [Sound](#Sound)
5. [Gameplay](#Gameplay)

## Entity

### set_variable(name, value)
Sets the value of a variable defined within the editor.
If the variable doesn't exist, then this function has no effect.

Parameters

    name (string) : Name of the variable to change
    value (number/string) : Value to set the variable to  

Example

    set_variable("Speed", 10)
    set_variable("PlayerName, "Steve")

---

### get_variable(name)
Returns the value of the given variable. If the variable doesn't
exist, then it return nil.

Parameters

    name (string) : Name of the variable to get

Returns

    Value of the given variable, if it exists.
    If not, returns nil.

Example

    health = get_variable("Health")
    set_variable("Health", health - 1)

---

### get_position()
Returns the coordinates of the entity.

Returns

    number: x coordinate of the entity
    number: y coordinate of the entity

Example

    x,y = get_position()
    set_position(x, y + 5)

---

### set_position(x, y)
Sets the position of the entity

Parameters

    x (number) : the x coordinate to set
    y (number) : the y coordinate to set

Example

    if get_input("W") then
        x,y = get_position()
        set_position(x, y - 5)
    end

--- 

### get_velocity()
Gets the velocity of the entity

Returns

    vx (number) : the velocity in the x direction
    vy (number) : the velocity in the y direction

Example

    vx,vy = get_velocity()

---

### set_velocity()
Sets the velocity of the entity

Parameters

    vx (number) : the velocity in the x direction
    vy (number) : the velocity in the y direction

Example

    -- Jump controls
    if get_input_single("SPACE") then
        if get_grounded then
            vx,vy = get_velocity()
            set_velcity(vx, vy - 10)
        end
    end

---

### get_acceleration()
Gets the acceleration of the entity

Returns

    ax (number) : the acceleration in the x direction
    ay (number) : the acceleration in the y direction

Example

    ax,ay = get_acceleration()

---

### set_acceleration(ax, ay)
Sets the acceleration of the entity

Parameters

    ax (number) : the acceleration in the x direction
    ay (number) : the acceleration in the y direction

Example

    if not get_grounded() then
        set_acceleration(0, 9.8)
    end

---

### get_grounded()
Returns whether the entity is on the ground

Returns

    True if the entity is on the ground, otherwise returns false

Example

    if not get_grounded() then
        set_acceleration(0, 9.8)
    end

---

### get_visible()
Gets whether the entity is visible

Returns

    True is the entity is visible, otherwise false

Example

    visible = get_visible()

---

### set_visible(visible)
Sets the visibility of the entity

Parameters
    
    visible (bool) : whether the entity should be visibile

Example

    -- Toggle visibility
    if get_input_single("V") then
        vis = get_visible()
        set_visible(not vis)
    end

---

### get_clicked()
Checks if the user clicked on the entity. Will return true for the duration the
left mouse button is held down on the entity.

Returns

    True if the user clicked on the entity, otherwise false

Example

    if get_clicked() then
        health = get_variable("Health")
        set_variable("Health", health - 1)
    end

---

### get_clicked_single()
Checks if the user clicked on the entity. Returns once per click.

Returns

    True if the user clicked on the entity, otherwise false

Example

    -- Toggle visibility once per click
    if get_clicked_single() then
        vis = get_visible()
        set_visible(not vis)
    end

---

### get_sprite()
Returns the path of the entity's current sprite relative to the main project file.
For example, a sprite with the absolute path "C:/Users/Steve/Desktop/Project/sprites/mario.png"
will return the path "sprites/mario.png" by this function.

Returns

    string : relative file path of the entity's sprite

Example

    -- Toggle between two sprites
    if get_input("T") then
        if get_sprite() == "sprites/mario.png" then
            set_sprite("sprites/luigi.png")
        elseif get_sprite() == "sprites/luigi.png" then
            set_sprite("sprites/mario.png")
        end
    end

---

### set_sprite(path)
Sets the sprite of the entity to the given sprite. Path is relative to the main project file.

Parameters

    path (string) : the relative path of the sprite

Example

    if get_input("A") then
        set_sprite("sprites/mario_left.png")
    elseif get_input("D") then
        set_sprite("sprites/mario_right.png")
    end

---

### get_name()
Gets the name of the entity

Returns

    name (string) : the name of the entity

Example

    name = get_name()

---

### set_name(name)
Sets the name of the entity

Parameters

    name (string) : the new name for the entity

Example

    set_name("Player 1")

---

### get_width()
Gets the width of the entity

Returns

    width (number) : the width of the entity

Example

    width = get_width()

---

### set_width(width)
Sets the width of the entity

Parameters

    width (number) : the new width of the entity

Example

    set_width(64)

---

### get_height()
Gets the height of the entity

Returns

    height (number) : the height of the entity

Example

    height = get_height()

---

### set_height()
Sets the height of the entity

Parameters

    height (number) : the height of the entity

Example

    set_height(64)

---

### get_tag()
Gets the tag of the entity

Returns

    tag (string) : the tag of the entity

Example

    tag = get_tag()

---

### set_tag()
Sets the tag of the entity

Parameters

    tag (string) : the new tag for the entity

Example

    set_tag("Enemy")

---

### create_entity(path)
Creates a new entity from a given prefab

Parameters

    path (string) : the relative path of the prefab

Returns

    id (number) : the unique ID of the created entity

Example

    if get_input_single("SPACE") then
        rocket = create_entity("prefabs/rocket.json")
        ConsoleWrite("Rocket ID: " + rocket)
    end

--- 

### delete_entity()
Deletes the entity

Example

    if check_collision() then
        delete_entity()
    end

---

### check_collision()
Checks if the entity has collided with any other entity

Returns

    True if the entity has collided with something, false otherwise

Example

    if check_collision() then
        health = get_variable("Health")
        set_variable("Health", health - 5)
    end

---

### check_collision_side(side)
Checks if one side of an entity is colliding with any other entity.
Valid sides are: TOP, BOTTOM, LEFT, RIGHT

Parameters

    side (string) : the side of the entity to check

Returns

    True is the given side is colliding with an entity, otherwise false

Example

    if get_input("A") then
        if not check_collision_side("LEFT") then
            x,y = get_position()
            set_position(x - 5, y)
        end
    end

---

### get_collision_entities()
Returns a table of entity IDs that this entity is colliding with

Returns

    Table of IDs of entities being collided with. If no entities are
    begin collided with, returns nil.

Example

    colliding = get_collision_entities()
    if colliding ~= nil then
        for _, id in ipairs(colliding) do
            ConsoleWrite(id)
        end
    end

---

## Text

## Camera

## Sound

## Gameplay

### ConsoleWrite(text)
Writes a string to the editor's console

Parameters

    text (string) : the string to write to the console

Example

    if get_input_single("SPACE") then
        ConsoleWrite("Jump!")
    end

---

### get_time_seconds()
Gets the number of seconds which has passed since the start of the game

Returns

    The number of seconds since the game started

Example

    -- 5 second timer
    timer = get_variable("timer")
    current_time = get_time_seconds()
    if current_time >= timer + 5 then
        -- Do something here

        set_variable("timer", current_time)
    end

--- 

### get_entity_list()
Returns a table containing the IDs of every entity in the game

Returns

    Table containing the IDs of every entity in the game.
    If no entities exist, returns nil.

Example

    entities = get_entity_list()
    if entities ~= nil then
        ConsoleWrite("Entity List: ")
        for _, id in ipairs(entities) do
            ConsoleWrite(id)
        end
        ConsoleWrite("")
    end

---

### get_entities_by_tag(tag)
Returns a table containing the IDs of every entity with a given tag

Parameters

    tag (string) : the tag for search for

Returns

    Table containing the IDs of every entity with the tag
    in the game. If no entities with the tag exist, returns nil.

Example

    entities = get_entities_by_tag("Enemy")
    if entities ~= nil then
        ConsoleWrite("Entity List: ")
        for _, id in ipairs(entities) do
            ConsoleWrite(id)
        end
        ConsoleWrite("")
    end

---

### get_entity_by_name(name)
Returns the ID of the first entity found with the given name

Parameters

    name (string) : the name to search for

Returns

    The ID of the entity with the given name

Example

    door = get_entity_by_name("BossDoor")

---


## Input
The input codes for keyboard and mouse are the following: 

    Keys = {
        A, B, C, ..., Y, Z,
        ESC, LCONTROL, LSHIFT, LALT, 
        RCONTROL, RSHIFT, RALT, SPACE, TAB, 
        UP, DOWN, LEFT, RIGHT,
        LBRACKET, RBRACKET
    }

    Mouse = {
        MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE,
        MOUSE_BUTTON_4, MOUSE_BUTTON_5
    }

### get_input(key)
Checks if a key is being held down.

Parameters

    key (string) : the keycode to check

Returns

    True if the key is pressed, otherwise false.

Example

    if get_input("W") then
        x,y = get_position()
        if get_input("LSHIFT") then
            set_position(x, y - 10)
        else
            set_position(x, y - 5)
        end
    end

---

### get_input_single(key)
Checks if a key was pressed. Only returns once per key press.

Parameters

    key (string) : the keycode to check

Returns

    True if the key is pressed, otherwise false.

Example

    if get_input_single("T") then
        create_entity("prefabs/enemy.json")
    end

---

### get_mouse_input(button)
Checks if a mouse button is being pressed.

Parameters

    button (string) : the mouse button to check

Returns

    True if the button is being pressed, otherwise false

Example

    if get_mouse_input("MOUSE_LEFT") then
        bullet = create_entity("prefabs/bullet.json")
        set_entity_velocity(bullet, 5, 0)
    end

---

### get_mouse_input_single(button)
Checks if a mouse button was pressed. Only returns once per click.

Parameters

    button (string) : the mouse button to check

Returns

    True is the button was pressed, otherwise false

Example

    if get_mouse_input_single("MOUSE_RIGHT") then
        mx,my = get_mouse_position()
        set_position(mx, my)
    end

---

### get_mouse_position()
Gets the position of the mouse cursor

Returns

    mx (number) : the x coordinate of the mouse
    my (number) : the y coordinate of the mouse

Example

    mx, my = get_mouse_position()

---
