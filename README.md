# Emulador de Luces de Parqueo y Direccionales en un STM32 (Nucleo)

Este documento describe una arquitectura de referencia para emular el comportamiento de las luces de parqueo (hazard) y direccionales en una placa STM32 (por ejemplo, Nucleo). La solución propuesta es mantenible, escalable y fácil de entender, ofreciendo un ejemplo didáctico para la asignatura de estructuras computacionales y desarrollo embebido.

## Objetivos

- **Luces de Parqueo:**
  Las luces parpadean de forma continua mientras el modo hazard está activado. Un evento (botón o comando UART) activa o desactiva este modo.

- **Direccionales con Comportamiento Inteligente:**
  - Una pulsación simple del botón de direccional (izquierda o derecha) produce que la luz correspondiente parpadee 3 veces y luego se detenga.
  - Dos pulsaciones consecutivas rápidas del botón de direccional hacen que el parpadeo sea indefinido hasta recibir un comando de parada.
  - **Condición adicional:** Si está activa la direccional de un lado (ej. izquierda) de forma indefinida y se recibe un evento del otro lado (ej. se presiona el botón derecho), esto se interpreta como comando de parar la direccional actual.

Este comportamiento se asemeja a sistemas de ayuda de giro en algunos vehículos modernos como el [Tesla Model 3](https://www.youtube.com/watch?v=3wZVLvbsBrc), donde una pulsación corta de la palanca de direccionales produce un parpadeo corto (3 veces) y una pulsación más firme inicia el parpadeo continuo. También si la direccional izquierda está encendida y el conductor toca la direccional derecha, se interpreta que desea cancelar la izquierda, deteniendo el parpadeo.

## Arquitectura Propuesta

La arquitectura se basa en capas de abstracción y una Máquina de Estados Finita (FSM):

1. **Capa HAL (Abstracción de Hardware):**  
   - `gpio.[c/h]`: Configuración de pines, acceso básico a LEDs y botones.  
   - `uart.[c/h]`: Comunicación UART (opcional, para comandos remotos).  
   - `systick.[c/h]`: Temporización básica con SysTick.

2. **Drivers Específicos:**  
   - `led_driver.[c/h]`: Funciones de más alto nivel para encender, apagar o alternar LEDs según el lado y el patrón deseado.  
   - `button_driver.[c/h]`: Detección de eventos de botón (simple, doble pulsación), eventos de hazard, y ahora también manejo del evento que detiene la direccional opuesta.  
   - `commands.[c/h]` (opcional): Interpretación de comandos UART para activar/desactivar hazard o detener direccionales infinitas.

3. **Capa de Lógica de Aplicación (FSM):**  
   - `parking_lights.[c/h]`: Aquí se define la FSM y su comportamiento. Estados propuestos:
     - `STATE_OFF`
     - `STATE_HAZARD`
     - `STATE_TURN_LEFT_3BLINKS`
     - `STATE_TURN_LEFT_INFINITE`
     - `STATE_TURN_RIGHT_3BLINKS`
     - `STATE_TURN_RIGHT_INFINITE`

## Lógica de la Máquina de Estados

- **Ejemplos de Estados y Transiciones:**

  | Estado                         | Evento                      | Nuevo Estado              |
  |--------------------------------|-----------------------------|---------------------------|
  | OFF                            | hazard botón o comando      | HAZARD                    |
  | OFF                            | dir. izq (1 pulsación)      | TURN_LEFT_3BLINKS         |
  | OFF                            | dir. izq (2 pulsaciones)    | TURN_LEFT_INFINITE        |
  | OFF                            | dir. der (1 pulsación)      | TURN_RIGHT_3BLINKS        |
  | OFF                            | dir. der (2 pulsaciones)    | TURN_RIGHT_INFINITE       |
  | HAZARD                         | hazard botón o comando      | OFF                       |
  | TURN_LEFT_3BLINKS              | completar 3 parpadeos       | OFF                       |
  | TURN_LEFT_INFINITE             | comando stop / dir. der     | OFF                       |
  | TURN_RIGHT_3BLINKS             | completar 3 parpadeos       | OFF                       |
  | TURN_RIGHT_INFINITE            | comando stop / dir. izq     | OFF                       |

- **Manejo del Evento "Parar":**  
  En los estados `TURN_LEFT_INFINITE` y `TURN_RIGHT_INFINITE`, además del comando stop (por UART u otro), una pulsación de la direccional opuesta también detiene la parpadeante actual. Por ejemplo, si estás en `TURN_LEFT_INFINITE` y se pulsa la direccional derecha (ya sea simple o doble vez), se interpreta como una orden de parar y el estado retorna a `OFF`.  
  De igual forma, si estás en `TURN_RIGHT_INFINITE` y se presiona la direccional izquierda, el estado se mueve a `OFF`.

- **Lógica de Tiempo:**

  * El SysTick o un timer sirve para contar milisegundos. Por ejemplo, cada periodo dado en ms se alterna el estado de la luz (encendido/apagado) para generar el parpadeo. [Ver (ECE/SAE/FMVS)]
  * Para los patrones de 3 parpadeos, se lleva un conteo interno (ej: cada toggle cuenta como medio ciclo de parpadeo; 6 toggles = 3 parpadeos on/off).

## Ejemplo de Código Simplificado

```c
switch (current_state) {
    case STATE_TURN_LEFT_INFINITE:
        // blink Left Light 
        if (stop_command || right_evt == BUTTON_SINGLE_PRESS || right_evt == BUTTON_DOUBLE_PRESS) {
            enter_state(STATE_OFF);
        }
        break;

    case STATE_TURN_RIGHT_INFINITE:
        // blink Left Light
        if (stop_command || left_evt == BUTTON_SINGLE_PRESS || left_evt == BUTTON_DOUBLE_PRESS) {
            enter_state(STATE_OFF);
        }
        break;
}
```
