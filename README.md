# RemoteObjectDetector
Project designed to detect objects on a camera view, using OpenCV.

### READY
1. Common: Carcas of internal structures, common architecture parts
2. Server: Docker container (for future) with main server
3. ManagePanel: GUI carcas
4. Server: HTTP API server processing
5. Server: HTTP API device operations processing (no actual work on this step)
6. ManagePanel: Server management
7. Server: UDP streaming (test version)

### PLANS (step by step)
1. Server: WSS Event processing (setup server to process ManagePanel and DeviceEndpoint messages)
2. ManagePanel: Device management
3. ManagePanel: Streaming base
4. DeviceEndpoint: WSS Event processing
5. DeviceEndpoint: Image processing -- copy legacy work, check it up, search for optimisation
6. Server: Image processing -- setup optimised version. Connect DeviceEndpoint as source for images

## Contents
1. ManagePanel -- GUI application on Qt5, used to work with detectors
2. DetectorEndpoint -- Endpoints designed to be placed in Raspberry Pi devices, they must just know their common settings (such as main server IP and ports), ID (token, frankly-speaking), camera (this will be proceed later) and time between shots to send streaming data (in case of processor usage and power management)
3. Server -- Main server, holding data about detectors (including events), processing images they sent (with transmitting to ManagePanel, if streaming is turned on for a device) and some other small functions
