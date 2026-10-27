# Insectron

# Bullet Hell inspired by touhou and retro games like space invaders, university's project c:

The game uses `Resources/Tr2n.ttf` for its interface. Saved games are stored in
`partida_guardada.txt` as plain-text key/value pairs (`initials`, `score`,
`level`, `wave`, `lives`, `x`, and `y`). Loading validates the score, level,
and lives (1-5), and keeps the player's position within the screen bounds.
