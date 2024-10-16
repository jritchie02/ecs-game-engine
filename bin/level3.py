import blockbyte
import time

PPM = 64 # Pixels per meter (1 Grid Unit)

board = blockbyte.Board(PPM, 20, 12)
app = blockbyte.Application(board, True)
scene = app.GetScene()

# Load a background sprite
backgroundEntity = scene.NewEntity()
t = scene.AssignTransformComponent(backgroundEntity)
app.AddSprite(backgroundEntity, "../assets/background.bmp", 20, 12)

# Load the spritesheet level
app.ImportSpritesheetLevel("../assets/level3.txt", "../assets/spritesheet.bmp")

# Load the player
playerEntity = scene.NewEntity()
t2 = scene.AssignTransformComponent(playerEntity)
input = scene.AssignInputComponent(playerEntity)
app.AddSprite(playerEntity, "../assets/player.bmp", 1, 1)
app.AddBox2D(playerEntity, 18 * PPM, 0, 1, 1, False, False)

# Add the Game over flag
gameOverEntity = scene.NewEntity()
t3 = scene.AssignTransformComponent(gameOverEntity)
t3.x = 0 * PPM
t3.y = 8 * PPM
app.AddBox2D(gameOverEntity, t3.x, t3.y, 1, 1, True, True)
app.AddSprite(gameOverEntity, "../assets/flag.bmp", 1, 1)
box2d = scene.GetBox2DColliderComponent(gameOverEntity)
def on_collision_enter():
    print("YOU WIN! Load next level")
    time.sleep(2)  # Delay for 2 seconds
    setattr(app, "m_isRunning", False)

box2d.onCollisionEnter = on_collision_enter


while app.m_isRunning:
    app.Loop(120)