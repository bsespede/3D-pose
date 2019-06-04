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
	- El algoritmo con checkboard tiene un error de reproyeccion ~1.5. Vamos a recalibrar usando un board charuco para mejorar presicion.
- Implementacion de calibracion y calculo de distorsion usando charuco board:
	- Se agregan parametros al config file.
	- Impresion de charuco boards.
	- Integracion de generacion de generacion de checkboards a UI.
- Modificacion del tiempo de exposicion durante calibracion para reducir blur, hacer pruebas semana que viene evaluando error de reproyeccion.

__9-4-2019 (9hs)__:
- Se capturaron boards charucos para calibracion intrinseca de las camaras (se podria mejorar los resultados con boards mas grandes)
- Se corrigieron bugs de la calibracion y se obtuvo un error de reproyeccion de ~0.2 pixeles
- Se pueden exportar las intrinsecas en formato json
- Se corrigieron bugs menores en la UI
- Se corrigio el dropeo de frames:
	- Ahora se valida que todas las camaras aporten frames a los paquetes
	- Esto implica que hay menos frames capturados pero que estan 100% sincronizados 
- Se pudo realizar una captura sincronizada con 16 camaras en simultaneo
- Validacion de existencia de capturas antes de procesar
- Faltaria:
	- Durante la proxima asistencia al laboratorio se intentara mejorar el frame rate
	- Falta capturar una escena para las extrinsecas

__10-4-2019 (6hs)__:
- Refactor de captura de frames estaticos
- Refactor de manejo de jsons en captura/calibracion para mayor logging y control
- Al dejar de usar precalibracion aruco anda mejor, solo charuco
- Fix deadlock cuando se frenan las camaras
- Faltaria:
	- Prevenir overwrite de capturas para extrinsics/escenas

__11-4-2019 (6hs)__:
- Refactor gigante para separar el acceso al config file
- Primera implementacion de extrinsics con charuco (+exportacion a JSON) -> solo para probar

__30-4-2019 (6hs)__:
- Se valido alidar lo de los numeros de camara al grabar
- Se verifico el problema del dropeo del frame rate y las camaras en daisy chain
- Se probo el nuevo checkerboard -> disminujo a la mitad el error de reproyeccion
- Se probaron varios niveles de exposion del lente hasta encontrar el ideal (25% del maximo)
- Se grabaron extrinsecas de prueba

__31-4-2019 (9hs)__:
- Se hizo la primera implementacion de extrinsecas de a pares encadenadas
- Faltaria:
	- Probar solo las camaras de arriba
		- Que tal el nuevo frame rate?
		- Se puede bajar la compresion asi sube la calidad de la imagen? Se ven mejor los markers?
	- No se ven los markers en ninguna camara cuando se apoya el board en el piso, pensar solucion
	- Pensar tema de calibracion con error de reproyeccion alto
	- Enfocar camaras
	
_1-5-2019 (6hs)__:
- Refactor del backend
- Primera version del rendereo 3D

_2-5-2019 (3hs)__:
- Refactor del frontend
- Separacion del rendereo 3D del backend
- Cambio de sistema de coordenadas en el renderer
- Fix a pose de la camara en world coordinates
- Faltaria:
	- Renderear point clouds del board en el tiempo
	- Proyectar corners del board a 3D en "visualizacion":
		- Tomar los ptos del modelo del pattern y aplicar su affine transform con los valores del json para llevarlos a 3D

__3-5-2019 (6hs)__:
- Exportar poses del board a json
- Cargar las poses en el renderer 3D y playback de video 3D!
- Faltaria:
	- Emprolijar clase video3D
	- Arreglar sleep en camera/UI

