# ControlEtiquetadora
 
 Instalacion HMI
- 1.-Instalar node-red en raspberry pi
    - sudo apt-get install nodered
- 2.-Autostart
    - sudo systemctl enable nodered.service
- 3.-Ir a localhost:1880 en navegador
- 4.-Exportar flow
- 5.-Descargar en manage palette el siguiente nodo:
    - node-red-dashboard
    - borrar nodo mqtt y añadir serial para arduino
- 6.-Usar on start page en chromium 
    - settings>buscar on star page>Open a specific page or set of pages> http://localhost:1880/ui
- 7.-Pegar hmiAutorun.sh en la carpeta home
- 8.-Pegar hmi.Desktop en:
    - cd /etc/xdg/autostart/


