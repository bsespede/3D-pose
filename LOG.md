__19-3-2019 (20hs):__
- Preparar entorno:
	- Instalacion de Windows 10.
	- Instalacion de SDK de Optitrack.
	- Correr samples del SDK de Optitrack.
	- Compilar OpenCV 4.1 + contrib modules.
- Arquitectura MVC:
	- Preparar archivito de CMake para levantar librerias.
	- Implementacion de CLI.
- Leer papers de calibracion para wands

__25-3-2019 (20hs):__
- Arquitectura MVC:
	- Modelado de camaras y escenas.
	- Manejo de input y output de escenas.
	- Implementacion de previsualizacion de camaras casi completo.
- Implementacion de captura sincronizada no bloqueante casi completa.
	- Correcion de problemas con deadlocks.
- Implementacion de SDK de Optitrack para capturar frames.
- Seguir leyendo papers de calibracion.

__3-4-2019 (3hs):__
- Charla con Marcela en la sala y presentacion de posibles algoritmos.
- Se termina primera implementacion del modulo de captura sincronizada.

__11-4-2019 (3hs):__
- Fix a frames vacios (revisar el front y el consumo de memoria).
- Ver el tema de los hubs.
- Ver el tema de apagar el filtro infrarrojo.
- Probar captura intrinsics y extrinsics.
- Captura sincronizada andando (solo por usb directo por ahora).
- Cosas a probar:
	- Clase camera:
		- ver maximo tiempo de exposicion e intensity para distintos framerates
		- sendEmptyFrames y sendInvalidFrames a false
		- Ver en detalle gain en la tool (https://v110.wiki.optitrack.com/index.php?title=Devices_pane y filter switch)
		- LED illumination

__25-4-2019 (6hs):__
- Se arreglo los memory leaks de frames vacios.
- Tema hubs: Al grabar en maxima resolucion no daba el ancho de banda, se baja el frame rate y se comprimen en MJPEG y se pueden usar los hubs. Falta probar varios hubs en daisy chain.
- No se puede apagar el filtro infrarrojo para estas camaras, seria ideal camaras RGBD como las intel D415.
- Grabamos los checkboards para intrinsics. 
- A veces faltan frames -> averiguar por que pasa la semana que viene.
- Cosas que se probaron:
	- Se hizo una grabacion con el tiempo de exposicion al limite para que haya mas contraste en la imagen. Hay mas blur pero parece no afectar a openPose.
	- Falta ver lo de SendEmptyFrames e InvalidFrames
	- Falta ver lo del gain en la tool
	- Falta ver lo del indicador de camara en motive
- BUG: No se pueden hacer 2 grabaciones seguidas en la tool (hay que abrir y cerrar la tool).

__28-4-2019 (6hs):__
- Se agrego un archivo de configuracion para las camaras y la tool:
	- Se agrego una orden predeterminado de numero de serie de camaras definido en dicho archivo (solo con las camaras de abajo por ahora)
- Se arreglo la herramienta de previsualizacion de camaras.
- Se incorporo el algoritmo de calibracion de intrinsecas:
	- El algoritmo con checkboard tiene un error de reproyeccion > 1.5. Vamos a recalibrar usando un board charuco para mejorar presicion.
- Implementacion de calibracion y calculo de distorsion usando charuco board:
	- Se agregan parametros al config file.
	- Impresion de charuco boards.
	- Integracion de generacion de generacion de checkboards a UI.
- Modificacion del tiempo de exposicion durante calibracion para reducir blur, hacer pruebas semana que viene evaluando error de reproyeccion.

