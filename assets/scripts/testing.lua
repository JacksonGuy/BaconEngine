if get_input_single("T") then
    current = get_sprite()
    if current == "./assets/player.jpg" then
        set_sprite("./assets/player2.png")
    elseif current == "./assets/player2.png" then
        set_sprite("./assets/player.jpg")
    end
end