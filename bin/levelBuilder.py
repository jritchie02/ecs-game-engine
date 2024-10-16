import blockbyte

board = blockbyte.Board(64, 20, 12)
app = blockbyte.Application(board, True)
scene = app.GetScene()

# Load a background sprite
backgroundEntity = scene.NewEntity()
t = scene.AssignTransformComponent(backgroundEntity)
app.AddSprite(backgroundEntity, "../assets/background.bmp", 20, 12)

# Add a spritesheet component entity
spritesheetEntity = scene.NewEntity()
spritesheet = scene.AssignSpriteSheetComponent(spritesheetEntity)

while app.m_isRunning:
    app.Loop(120)