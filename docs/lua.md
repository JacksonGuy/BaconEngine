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
    health -= 1
    set_variable("Health", health)

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

    if get_input("W") == true then
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

    if get_input_single("SPACE") == true then
        if get_grounded == true then
            vx,vy = get_velocity()
            set_velcity(vx, vy - 10)
        end
    end

---

## Text

## Camera

## Sound

## Gameplay