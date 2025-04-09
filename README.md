
# TPE1-SO: ChompChamps

## Descripción
Este proyecto implementa el juego "ChompChamps", un juego concurrente en el que múltiples jugadores (bots) compiten por recolectar recompensas en un tablero compartido. El sistema utiliza memoria compartida y semáforos para coordinar las acciones entre un proceso master, los jugadores, y una vista que muestra el estado del juego.

### Objetivo
- **Master**: Controla el estado del juego, actualiza el tablero y coordina a los jugadores y la vista.
- **Jugadores**: Bots que se mueven en el tablero buscando maximizar su puntaje al ocupar celdas con recompensas.
- **Vista**: Muestra el estado actual del tablero y los jugadores activos.