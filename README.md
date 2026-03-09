# RemoteObjectDetector
Project designed to detect objects on a camera view, using OpenCV.  
**IMPORTANT:** Detector designed to work in local network, so traffic is not secure. In future could be update with custom encryption protocol, but now it's not implemented.

### READY
1. ManagementPanel: GUI carcas, server real-time status (only local, with hardcode)
2. Server: UDP data receiving, WS connection listening and validation, future business logics carcas
3. DetectorEndpoint: WS connecting to server, waiting for events, future business logics carcas

### PLANS (step by step)
1. ManagementPanel: Detector real-time status
2. Server, ManagementPanel: UDP image (video) streaming to ManagementPanel (TEST VERSION)
3. Server, DetectorEndpoint: UDP data retranslation to ManagementPanel
4. DetectorEndpoint: Buffering data to send on reconnected (detected objects and events)
5. DetectorEndpoint: Camera data streaming
6. Image processing (same on server and detector endpoint)
7. Fixing current and making Docker images for Server and DetectorEndpoint
8. Unit and integration test writing for everything

## Contents
1. ManagePanel -- GUI application on Qt5, used to handle server and detectors (boot, status, data streaming, etc.)
2. DetectorEndpoint -- Endpoints designed to be placed in Raspberry Pi devices, they must just know their common settings (such as main server IP and ports), ID (token, frankly-speaking), camera (this will be proceed later) and time between shots to send streaming data (in case of processor usage and power management)
3. Server -- Main server, holding data about detectors (including events), processing images they sent (with transmitting to ManagePanel, if streaming is turned on for a device) and some other small functions
