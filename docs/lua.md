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

## get_visible()
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


## Text

## Camera

## Sound

## Gameplay