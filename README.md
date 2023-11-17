# Estacion_MED
## Estaciones meteorológicas UPB

En este repositorio encontrarán el código con el cual se pueden implementar la captura y visualización
de datos meteorológicos y como añadir entidades y suscripciones nuevas al sistema.

# Contenido:

Archivo .yml para ejecutar con docker compose y tener cada servicio (visualización, base de datos, etc.) por separado

código en arduino para implementar en los sensores para la captura de los datos

Como crear una entidad o una suscripción:

Por medio de una sentencia http dirigidas al servidor orion de la universidad de la siguiente forma:

1.) Empleando la herramienta Postman de manera local, no usar la versión web ya que no se permiten conexiones externas

colocando uno de los siguientes argumentos al principio de cada sentencia se ejecuta una acción diferente:

POST: Crea una entidad o suscripción nueva

PATCH: Modifica una entidad o suscripción creada previamente

DELETE: elimina una entidad o suscripción

Luego se debe colocar la dirección ip del servidor la cual es la 10.38.32.137

Seguido a esto se debe añadir : y el puerto del servicio al cual se va hacer la modificación

Luego añadir un /v2 y luego otro / diciendo en caso de que sea una entidad /entities , o en caso de que sea una suscripción /subscriptions

Ejemplo de creación de una entidad:

opción POST en postman >> https://10.38.32.137:1031/v2/entities  


2.) escribiendo el curl de manera manual, debe ser muy preciso ya que las instrucciones en http son sensibles a espacios

# Ejemplo:

POST /v2/subscriptions HTTP/1.1
Host: 10.38.32.137:1031
Content-Type: application/json
Content-Length: valor en bytes del tamaño de la petición

{
  "description": "UPBMED Weather Station Subscription",
	"subject": { 
		"entities" : [
			{
				"id": "Estación_UPB_Med",
				"type":
					"Estacion_Meteorologica"
			}
		],
		"condition": [
		"attrs": [
			"temperature"
			"humidity"
			"solar_radiation"
		]
	}
},
