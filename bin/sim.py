import blockbyte

board = blockbyte.Board(64, 20, 12)
app = blockbyte.Application(board, False)
scene = app.GetScene()

# Add Grid entity 
gridSim = scene.NewEntity()
gridComponent = scene.AssignGridSimulationComponent(gridSim)

while app.m_isRunning:
    app.Loop(120)