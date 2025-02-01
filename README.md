# Go-Back-N Protocol Simulation with OMNeT++

This repository contains an OMNeT++ simulation project that implements the Go-Back-N protocol.

## Project Overview

The Go-Back-N protocol is a sliding window protocol used for reliable data transmission over a network. This project simulates the protocol using the OMNeT++ discrete event simulator.

## Project Structure

- `src/`: Contains the source code for the simulation.
  - `Coordinator.cc`, `Coordinator.h`, `Coordinator.ned`: Implementation of the Coordinator module.
  - `Node.cc`, `Node.h`, `Node.ned`: Implementation of the Node module.
  - `MyMessage.msg`: Definition of the custom message type.
  - `Makefile`: Makefile for building the project.
- `simulations/`: Contains the simulation configuration files.
  - `omnetpp.ini`: Main configuration file for the simulation.
  - `package.ned`: NED package definition.

## Building the Project

To build the project, run the following command in the `src/` directory:

```sh
make
