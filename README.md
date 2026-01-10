# Sistema de Estacionamiento Inteligente IoT

![Status](https://img.shields.io/badge/Status-Finalizado-success)
![Institution](https://img.shields.io/badge/ESCOM-IPN-blue)
![Area](https://img.shields.io/badge/Unidad-Sistemas%20Embebidos-orange)

## Descripción del Proyecto
Este sistema permite la gestión y visualización de espacios de estacionamiento utilizando una arquitectura distribuida. A diferencia de soluciones IoT tradicionales que dependen de la nube, este proyecto implementa **Fog Computing** mediante una Raspberry Pi 4 configurada como Gateway y Servidor Central, garantizando baja latencia y alta disponibilidad.

El sistema es capaz de detectar la presencia vehicular mediante sensores ultrasónicos y actualizar un Dashboard Web en tiempo real utilizando protocolos ligeros (MQTT) y tecnologías web modernas (WebSockets), todo dentro de una red local aislada (Hotspot).

---

## Características Técnicas Destacadas

* **Arquitectura Offline (Fog Computing):** El sistema genera su propia infraestructura de red (Hotspot WiFi) y no requiere salida a Internet para funcionar.
* **Dual-Stack MQTT:** El Broker Mosquitto fue re-ingenierizado para escuchar simultáneamente en:
    * Puerto `1883` (TCP) para microcontroladores ESP32.
    * Puerto `9001` (WebSockets) para el Dashboard Web.
* **Envío por Excepción:** Lógica de firmware optimizada que solo transmite datos cuando existe un cambio de estado físico (LIBRE ↔ OCUPADO), ahorrando ancho de banda y procesamiento.
* **Interfaz Resiliente:** Los recursos gráficos institucionales (Logos IPN/ESCOM) fueron codificados en **Base64** e inyectados en el flujo de Node-RED, eliminando la dependencia de servidores de imágenes externos.
* **Modo Headless:** Servidor administrado remotamente vía VNC y/o SSH.

---

## Arquitectura del Sistema

1.  **Nodos de Sensores (Edge):** ESP32 con sensores HC-SR04. Realizan la adquisición de datos y filtrado de señal.
2.  **Transporte:** WiFi local sobre protocolo MQTT.
3.  **Procesamiento (Fog Server):** Raspberry Pi 4 ejecutando:
    * **Mosquitto:** Broker de mensajería.
    * **Node-RED:** Lógica de negocio, orquestación y servidor UI.
4.  **Visualización:** Dashboard HTML5 consumido vía navegador.

---

## Lista de Materiales (Hardware)

| Cant. | Componente | Descripción / Nota Técnica |
| :---: | :--- | :--- |
| 1 | **Raspberry Pi 4 Model B** | Servidor Central y Broker MQTT. |
| 1 | **Memoria MicroSD** | 32GB / 64GB (Clase 10 recomendada) para el SO. |
| 1 | **Fuente (Raspberry Pi)** | Alimentación dedicada para el servidor. |
| 1 | **Fuente (Sensores/ESP)** | 5V 2A. Alimentación independiente para la etapa de potencia. |
| 2 | **Protoboards** | 830 puntos c/u para montaje de circuitos. |
| 1 | **Cable USB a MicroUSB** | Para carga de código y depuración serial del ESP32. |
| 1 | **Monitor** | *Uso temporal:* Habilitación inicial del modo Headless. |
| 1 | **Cable Ethernet** | *Uso temporal:* Configuración inicial del Hotspot. |
| 1 | **Cable HDMI a MicroHDMI** | *Uso temporal:* Visualización de Raspbian OS. |
| 4 | **Pares de Resistencias** | 1kΩ y 2.2kΩ. Divisor de voltaje para protección de GPIO (Adaptación de nivel 5V $\to$ 3.3V). |
| N/A | **Conectores** | Alambre cal. 22 o Jumpers Macho-Macho. |
| 4 | **Sensores HC-SR04** | Sensores ultrasónicos de detección. |

---

## 💻 Stack de Software

* **S.O.:** Raspberry Pi OS Lite (64-bit).
* **Middleware:** Eclipse Mosquitto (Configurado con WebSockets).
* **Backend/Frontend:** Node-RED.
* **Firmware:** C++ (Arduino Framework) + Librería `PubSubClient`.
* **Herramientas:** SSH, VNC Viewer, Mosquitto Clients.

---

## 📸 Galería de Evidencias

### 1. Tablero de Control (Dashboard)
*Estado del sistema mostrando espacios libres y ocupados con identidad institucional.*

![Dashboard Final](img/dashboard_final.png)
*(Asegúrate de subir tu captura final a una carpeta llamada 'img' y ajustar este nombre)*

### 2. Pruebas de Latencia (Backend)
*Visualización de flujo de datos MQTT en tiempo real vía terminal.*

![Terminal MQTT](img/terminal_mqtt.png)

### 3. Prototipo Físico
*Montaje de la maqueta con sensores y validación física.*

![Maqueta Física](img/maqueta.png)

---

## ⚙️ Instalación y Reproducción

### Requisitos Previos
* Raspberry Pi con Raspberry Pi OS.
* Node-RED y Mosquitto instalados.

### Configuración del Broker (Mosquitto)
Editar `/etc/mosquitto/mosquitto.conf` para habilitar WebSockets:
```conf
listener 1883
allow_anonymous true

listener 9001
protocol websockets
