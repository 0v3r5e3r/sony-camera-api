# Sony Camera Remote REST API

REST API server for controlling Sony cameras using the Sony Camera Remote SDK.

## Project Structure

```
project/
├── deps/                      # Git submodules (managed automatically)
│   ├── cpp-httplib/          # HTTP server library
│   └── json/                 # JSON library (nlohmann-json)
├── external/                  # Sony SDK and OpenCV (not in git)
│   ├── crsdk/                # Sony Camera Remote SDK
│   └── opencv/               # OpenCV libraries
├── cmake/                     # CMake scripts
├── main.cpp                   # REST API server entry point
├── API.h/cpp                  # REST API implementation
├── CameraDevice.h/cpp         # Camera device wrapper
├── OpenCVWrapper.h/cpp        # OpenCV utilities
├── Text.h/cpp                 # Text utilities
├── ConnectionInfo.h/cpp       # Connection info
├── CrDebugString.h/cpp        # Debug strings
├── MessageDefine.h/cpp        # Message definitions
├── PropertyValueTable.h/cpp   # Property table
├── CMakeLists.txt             # Build configuration
└── run.sh                     # Build and run script
```

## Dependencies

### Managed via Git Submodules (Automatic)

The following dependencies are managed as git submodules in the `deps/` directory:

- **cpp-httplib** - Lightweight HTTP server library (header-only)
- **nlohmann-json** - JSON library for C++ (header-only)

These are automatically set up when you clone or initialize submodules.

### External Dependencies (Manual Setup Required)

You need to provide these in the `external/` directory:

1. **Sony Camera Remote SDK** - Place in `external/crsdk/`
2. **OpenCV 4.8** - Place platform-specific libraries in:
   - `external/opencv/Linux/` (Linux)
   - `external/opencv/Windows/` (Windows)
   - `external/opencv/Darwin/` (macOS)

## Quick Start

### 1. Clone the Repository

```bash
# Clone the repository
git clone <repository-url>
cd <project-directory>

# Submodules should already be initialized
```

### 2. Set Up External Dependencies

Place the Sony Camera Remote SDK and OpenCV libraries in the `external/` directory as described above.

### 3. Build and Run

```bash
# Make the run script executable
chmod +x run.sh

# Build and optionally run
./run.sh
```

Or manually:

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
./CameraRestAPI [port]
```

Default port is 8080 if not specified.

## API Endpoints

### Connect a Camera

```http
POST /v1/camera
Content-Type: application/json

{
    "ip": "192.168.1.100",
    "password": "optional_password"
}
```

**Response (201 Created):**
```json
{
    "serial": "1234567890",
    "ip": "192.168.1.100",
    "model": "ILCE-7M4",
    "recording": null
}
```

### List Connected Cameras

```http
GET /v1/camera
```

**Response (200 OK):**
```json
{
    "cameras": [
        {
            "serial": "1234567890",
            "ip": "192.168.1.100",
            "model": "ILCE-7M4",
            "recording": null
        }
    ]
}
```

### Toggle Recording

```http
POST /v1/camera/{serial}/recording
```

**Response (200 OK):**
```json
{
    "recording": true
}
```

### Toggle Pause/Resume

```http
PATCH /v1/camera/{serial}/recording
```

**Response (200 OK):**
```json
{
    "recording": false
}
```

### Get Recording State

```http
GET /v1/camera/{serial}/recording
```

**Response (200 OK):**
```json
{
    "recording": true
}
```

## Recording States

The `recording` field has three possible values:

- **`null`** - Camera is not recording
- **`true`** - Camera is actively recording (live)
- **`false`** - Recording is paused

## Example Usage

### Using curl

```bash
# Connect a camera
curl -X POST http://localhost:8080/v1/camera \
  -H "Content-Type: application/json" \
  -d '{"ip": "192.168.1.100", "password": ""}'

# List connected cameras
curl http://localhost:8080/v1/camera

# Start recording (use serial from list response)
curl -X POST http://localhost:8080/v1/camera/1234567890/recording
# Response: {"recording": true}

# Pause recording
curl -X PATCH http://localhost:8080/v1/camera/1234567890/recording
# Response: {"recording": false}

# Resume recording
curl -X PATCH http://localhost:8080/v1/camera/1234567890/recording
# Response: {"recording": true}

# Stop recording
curl -X POST http://localhost:8080/v1/camera/1234567890/recording
# Response: {"recording": null}

# Check recording state
curl http://localhost:8080/v1/camera/1234567890/recording
```

### Using JavaScript/fetch

```javascript
// Connect a camera
const response = await fetch('http://localhost:8080/v1/camera', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({ip: '192.168.1.100', password: ''})
});
const camera = await response.json();
console.log(camera.serial);

// Start recording
await fetch(`http://localhost:8080/v1/camera/${camera.serial}/recording`, {
    method: 'POST'
});

// Check state
const state = await fetch(`http://localhost:8080/v1/camera/${camera.serial}/recording`);
const {recording} = await state.json();
console.log(recording); // true, false, or null
```

### Using Python

```python
import requests

# Connect a camera
response = requests.post('http://localhost:8080/v1/camera', 
    json={'ip': '192.168.1.100', 'password': ''})
camera = response.json()
serial = camera['serial']

# Start recording
requests.post(f'http://localhost:8080/v1/camera/{serial}/recording')

# Get state
state = requests.get(f'http://localhost:8080/v1/camera/{serial}/recording')
print(state.json()['recording'])  # True, False, or None
```

## Building for Different Platforms

### Linux

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./CameraRestAPI
```

### Windows (Visual Studio)

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
.\Release\CameraRestAPI.exe
```

### macOS

```bash
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
./CameraRestAPI
```

## Troubleshooting

### Submodule Issues

**Problem:** Submodules not found or empty directories in `deps/`

**Solution:**
```bash
git submodule update --init --recursive
```

### Camera Connection Issues

**Problem:** "Camera not found at IP"

**Solutions:**
- Verify camera is powered on and network-accessible
- Check camera's remote control settings are enabled
- Ensure correct IP address
- Check firewall settings

### Build Errors

**Problem:** "Cannot find Cr_Core library"

**Solution:** Ensure Sony Camera Remote SDK is properly placed in `external/crsdk/`

**Problem:** "OpenCV libraries not found"

**Solution:** Place OpenCV libraries in the appropriate `external/opencv/` platform directory

### Recording Issues

**Note:** Some Sony cameras may require specific setup:
- Camera must be in movie mode
- Recording media must be available and formatted
- Battery level must be sufficient
- Verify lens is attached and functional

## Development

### Adding New Endpoints

1. Add handler function in `API.h`
2. Implement handler in `API.cpp`
3. Register route in `API::start()`

### Debugging

Build in debug mode:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

Run with verbose logging (modify source to add logging as needed).

## Production Deployment

For production use, consider:

1. **Add authentication** - JWT tokens, API keys
2. **Add HTTPS** - Use reverse proxy (nginx) or integrate TLS
3. **Add rate limiting** - Prevent API abuse
4. **Add persistence** - Store camera configs in database
5. **Add monitoring** - Health checks, metrics, logging
6. **Run as service** - Systemd unit file (Linux)

### Example Systemd Service

```ini
[Unit]
Description=Sony Camera REST API
After=network.target

[Service]
Type=simple
User=camera
WorkingDirectory=/opt/camera-api
ExecStart=/opt/camera-api/CameraRestAPI 8080
Restart=always

[Install]
WantedBy=multi-user.target
```

## License

Check the Sony Camera Remote SDK license for usage restrictions.

## Support

For issues:
1. Verify all dependencies are correctly installed
2. Check that submodules are initialized
3. Ensure Sony SDK and OpenCV are in correct locations
4. Test camera connectivity directly before using API
5. Review build logs for errors