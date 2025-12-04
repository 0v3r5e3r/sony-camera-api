#include "API.h"
#include "CRSDK/CameraRemote_SDK.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<api::API> server;

void signal_handler(int signal)
{
    std::cout << "\nShutting down server..." << std::endl;
    if (server) {
        server->stop();
    }
    
    // Release SDK
    SCRSDK::Release();
    
    exit(0);
}

int main(int argc, char* argv[])
{
    // Set up signal handlers for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    int port = 8080;
    
    // Allow port to be specified via command line
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port number. Using default: 8080" << std::endl;
            port = 8080;
        }
    }
    cli::tout << "Initialize Remote SDK...\n";
    auto init_success = SCRSDK::Init();
    if (!init_success) {
        std::cerr << "Failed to initialize Remote SDK. Terminating.\n";
        SCRSDK::Release();
        return 1;
    }
    cli::tout << "Remote SDK successfully initialized.\n\n";

    std::cout << "Sony Camera Remote API Server" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "Starting server on port " << port << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    std::cout << std::endl;
    std::cout << "API Endpoints:" << std::endl;
    std::cout << "  POST   /v1/camera                       - Connect a camera" << std::endl;
    std::cout << "  GET    /v1/camera                       - List connected cameras" << std::endl;
    std::cout << "  POST   /v1/camera/{serial}/recording    - Toggle recording" << std::endl;
    std::cout << "  PATCH  /v1/camera/{serial}/recording    - Toggle pause/resume" << std::endl;
    std::cout << "  GET    /v1/camera/{serial}/recording    - Get recording state" << std::endl;
    std::cout << std::endl;



    try {
        server = std::make_unique<api::API>(port);
        server->start(); // Blocks until server stops
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}