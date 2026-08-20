---
author:
- Robot Core Project
date: Version 1.0 --- 2026-08-18
title: Robot Core Architecture Baseline
---

# 1. Purpose

This document establishes the initial architectural baseline for the
Robot Core project.

The purpose of the baseline is to define the system's architectural
intent, component boundaries, responsibilities, contracts, ownership
model, dependency direction, and dependency injection strategy before
core implementation begins.

This document is a reference for both development and architectural
review. The implementation should remain consistent with these decisions
unless a deliberate architectural change is made and documented.

# 2. Architectural Vision

Robot Core is a software core responsible for representing robot state,
coordinating robot subsystems, and transforming control decisions into
actuator commands through abstractions that allow the system to be
tested without direct dependence on physical hardware.

The initial architecture is intentionally small. It is designed to
support future evolution toward simulation, real hardware, state
machines, control systems, sensors, planning, observability, and
robotics middleware without introducing unnecessary complexity at the
current stage.

## 2.1 Architectural principles

-   Separation of responsibilities
-   Hardware independence
-   Testability
-   Extensibility
-   Low coupling
-   High cohesion
-   Explicit ownership
-   Dependency injection
-   RAII-based lifetime management
-   Simple architecture before premature abstraction

## 2.2 Scope of the initial phase

The initial architecture focuses on the robot's decision and
coordination core:

``` text
Application
    |
    v
RobotCore
    |
    +---- RobotState
    |
    +---- Controller
    |
    +---- RobotCommand
    |
    +---- Actuator
```

The architecture is prepared for future simulation and hardware
implementations, but those integrations are not part of the initial
implementation.

## 2.3 Deliberately deferred technologies

The following technologies and subsystems are not part of the initial
implementation:

-   ROS 2
-   Qt
-   Physics simulation
-   Kinematics and dynamics
-   Computer vision
-   Machine learning
-   Real-time control
-   Physical hardware drivers
-   Networking
-   Complex multithreading
-   Plugin systems
-   Dependency injection frameworks

They may be introduced later when a concrete requirement justifies them.

# 3. Architectural Model

The initial conceptual model consists of five primary components plus
the command data concept.

``` text
                         +-------------------+
                         |    Application    |
                         |   Entry Point     |
                         +---------+---------+
                                   |
                                   v
                         +-------------------+
                         |    Robot Core     |
                         |   Coordination    |
                         +----+---------+----+
                              |         |
                 +------------+         +------------+
                 v                                   v
        +-------------------+               +-------------------+
        |    Robot State    |               |    Controller     |
        | Current condition |               | Decision / Control|
        +-------------------+               +---------+---------+
                                                      |
                                                      v
                                             +-------------------+
                                             |   Robot Command   |
                                             | Desired action    |
                                             +---------+---------+
                                                       |
                                                       v
                                             +-------------------+
                                             |     Actuator      |
                                             +---------+---------+
                                                       |
                                                       v
                                             +-------------------+
                                             | Concrete Backend  |
                                             | Real / Simulated  |
                                             +-------------------+
```

## 3.1 Central architectural idea

Robot Core coordinates the system and makes decisions possible, but it
does not contain hardware-specific implementation details.

The core should know what an actuator can conceptually do, but it should
not know how a specific motor, servo, communication bus, or driver
performs that operation.

# 4. Component Responsibilities and Contracts

## 4.1 Application

### Responsibility

Application is the executable entry layer and composition root.

It is responsible for:

-   creating and configuring system components;
-   establishing dependencies;
-   starting the Robot Core;
-   controlling application lifetime;
-   initiating orderly shutdown.

### It must not

-   implement robot behavior;
-   calculate control decisions;
-   directly control actuators;
-   contain hardware-specific robot logic;
-   become a substitute for RobotCore.

### Conceptual contract

``` text
Input:
    Configuration and dependencies

Output:
    Initialized and running system
```

# 4.2 RobotCore

### Responsibility

RobotCore is the central coordinator of robot execution.

It is responsible for:

-   coordinating the execution cycle;
-   obtaining the current robot state;
-   requesting a control decision;
-   forwarding the resulting command;
-   maintaining overall operational coordination.

### It must not

-   implement hardware drivers;
-   contain motor-specific logic;
-   directly access hardware APIs;
-   become a God Class;
-   contain GUI responsibilities.

### Conceptual contract

``` text
RobotCore
    update()
        |
        +-- read current state
        +-- request control decision
        +-- execute resulting command
```

# 4.3 RobotState

### Responsibility

RobotState represents the software's current knowledge of the robot's
state.

It is primarily a domain data model.

It is responsible for:

-   storing relevant robot-state information;
-   providing controlled access to that information;
-   representing a coherent state at a given point in time.

### It must not

-   access hardware;
-   read sensors directly;
-   control actuators;
-   decide robot behavior;
-   depend on hardware drivers.

The conceptual relationship is:

``` text
Sensor / Data Source
        |
        v
    RobotState
```

The state model should not need to know where its data originated.

# 4.4 Controller

### Responsibility

Controller transforms the current robot state into a desired action.

Conceptually:

``` text
Command = Controller(State)
```

It is responsible for:

-   analyzing robot state;
-   applying control logic;
-   producing a RobotCommand;
-   maintaining only the internal state required by its control
    algorithm.

### It must not

-   access hardware directly;
-   write to motors;
-   know concrete hardware drivers;
-   initialize the application;
-   own infrastructure concerns.

The Controller decides; the Actuator executes.

# 4.5 RobotCommand

### Responsibility

RobotCommand represents a desired action at the robot-domain level.

It should describe intent rather than hardware implementation details.

Good conceptual representation:

``` text
Desired velocity = X
```

Poor domain representation:

``` text
PWM channel 3 = 127
```

Hardware-specific parameters belong to infrastructure or actuator
implementations.

# 4.6 Actuator

### Responsibility

Actuator represents the boundary between robot-domain commands and an
action-producing implementation.

It is responsible for:

-   receiving RobotCommand objects;
-   validating commands when necessary;
-   forwarding commands to concrete implementations;
-   reporting relevant execution failures.

### It must not

-   decide high-level robot strategy;
-   implement controller logic;
-   know how the Controller works;
-   own high-level robot behavior.

The conceptual responsibility is:

``` text
Controller:
    "This is the desired action."

Actuator:
    "I will execute the desired action."
```

# 5. Data Flow

The initial runtime data flow is:

``` text
RobotState
    |
    v
Controller
    |
    v
RobotCommand
    |
    v
Actuator
```

RobotCore orchestrates this flow.

The architecture distinguishes between data flow and dependency
direction.

Data may flow from state toward control and actuation while dependencies
remain oriented toward stable domain concepts and abstractions.

# 6. Dependency Direction

The initial dependency direction is:

``` text
Application
     |
     v
RobotCore
     |
     +------> Domain Concepts
     |
     +------> Interfaces
                    ^
                    |
              implements
                    |
              Infrastructure
```

The exact physical project structure may evolve, but the central rule
remains:

> Core/domain logic must not become dependent on concrete hardware or
> infrastructure details.

Examples of concerns that should remain outside the core:

-   Qt GUI
-   specific motor SDKs
-   serial communication
-   hardware drivers
-   simulation engines
-   logging backends
-   external communication protocols

# 7. Ownership and Lifetime

## 7.1 Ownership model

The initial implementation uses explicit ownership.

Conceptually:

``` text
Application
    |
    +---- owns ----> RobotState
    |
    +---- owns ----> Controller
    |
    +---- owns ----> Actuator
    |
    +---- owns ----> RobotCore
```

RobotCore uses these components but does not own them.

Therefore:

``` text
Ownership:
    Application -> Components

Dependency:
    RobotCore -> Components
```

Ownership and dependency are separate concepts.

## 7.2 RAII

RAII is the default lifetime-management strategy.

Where possible, components should be created as automatic objects with
clear scope and deterministic destruction.

Manual `new`/`delete` is not the default approach.

## 7.3 Smart pointer policy

`std::unique_ptr` should be used when genuine exclusive dynamic
ownership is required.

`std::shared_ptr` should not be introduced merely because it is
convenient or considered modern C++.

Shared ownership requires a concrete architectural justification.

# 8. Dependency Injection

RobotCore must not construct its own dependencies.

Avoid:

``` text
RobotCore
    +-- creates Controller
    +-- creates Actuator
```

Prefer:

``` text
Application
    |
    +-- creates Controller
    +-- creates Actuator
    |
    +-- injects dependencies
            |
            v
        RobotCore
```

## 8.1 Constructor injection

The preferred initial mechanism is constructor injection.

Conceptually:

``` text
RobotCore(
    RobotState&,
    Controller&,
    Actuator&
)
```

The exact C++ interface will be defined during implementation.

The use of references expresses a non-owning dependency when the
dependency is mandatory.

## 8.2 Why dependency injection?

Dependency injection provides:

-   explicit dependencies;
-   simpler lifetime reasoning;
-   lower coupling;
-   easier testing;
-   support for simulation;
-   support for fake/mock implementations;
-   freedom to replace infrastructure without changing core logic.

The same RobotCore should be usable with different implementations:

``` text
Production:
    RobotCore + RealActuator

Simulation:
    RobotCore + SimulatedActuator

Test:
    RobotCore + FakeActuator
```

# 9. Architectural Rules and Decisions

## 9.1 We will

-   keep ownership explicit;
-   inject required dependencies;
-   prefer references for mandatory non-owning dependencies;
-   use RAII by default;
-   use `std::unique_ptr` only when dynamic exclusive ownership is
    required;
-   keep hardware details outside the core;
-   keep components focused on one responsibility;
-   test components with replaceable dependencies;
-   add abstractions when there is a concrete need.

## 9.2 We will not, at this stage

-   use `std::shared_ptr` without a genuine shared-ownership
    requirement;
-   use global mutable state;
-   use global singletons for core services;
-   use a Service Locator;
-   introduce a dependency injection framework;
-   create interfaces for every class;
-   create factories without a concrete need;
-   couple the core directly to hardware;
-   add technologies solely because they are popular.

# 10. Immediate Benefits

The architecture provides four immediate engineering benefits.

## Testability

Core components can be tested independently using controlled or fake
dependencies.

## Extensibility

New controllers, actuators, and implementations can be introduced
without rewriting the core.

## Maintainability

Each component has a focused responsibility and a limited reason to
change.

## Hardware Independence

The core can run against simulated or real implementations without
changing its central logic.

# 11. Future Architectural Layers

The following components may be introduced when requirements justify
them:

``` text
Sensors
    |
    v
State Estimator
    |
    v
Robot State
    |
    v
State Machine / Planner
    |
    v
Controller
    |
    v
Robot Command
    |
    v
Actuator Layer
    |
    +---- Simulation
    |
    +---- Real Hardware
```

Potential future supporting components include:

-   Sensors
-   State Estimator
-   State Machine
-   Planner
-   Motion Control
-   Logging
-   Telemetry
-   Configuration
-   Simulation
-   Hardware interfaces
-   ROS 2 integration

These are intentionally outside the initial implementation baseline.

# 12. Architectural Contract

The central architectural contract is:

``` text
Command = Controller(State)
```

with RobotCore coordinating the execution:

``` text
RobotState
    |
    v
Controller
    |
    v
RobotCommand
    |
    v
Actuator
```

The fundamental responsibility split is:

``` text
RobotState  -> represents
Controller  -> decides
Command     -> expresses intent
Actuator    -> executes
RobotCore   -> orchestrates
Application -> composes and owns
```

# 13. Implementation Guidance

The next implementation phase should follow this sequence:

``` text
Architecture Baseline
        |
        v
Project Structure
        |
        v
C++ Types and Contracts
        |
        v
Minimal Implementation
        |
        v
CMake Build
        |
        v
Unit Tests
        |
        v
Debug / Refactor
        |
        v
Documentation
        |
        v
Git Commit
```

The implementation should begin with the smallest useful system and
should not introduce future subsystems prematurely.

# 14. Change Policy

This document represents Architecture Baseline v1.0.

If implementation reveals that an architectural decision is inadequate,
the decision should be reviewed explicitly rather than silently
violated.

A meaningful architectural change should:

1.  identify the problem;
2.  explain the alternatives considered;
3.  record the chosen solution;
4.  update this document;
5.  be reflected in the implementation;
6.  be tested where appropriate;
7.  receive a coherent Git commit.

The architecture is therefore a living baseline, not a permanent
constraint.

# 14.1 Physical Project Structure

The current physical project structure reflects the architectural
separation established by this baseline.

```text
Robot-Core/
|
+-- app/
|   +-- main.cpp
|
+-- include/
|   +-- robot_core/
|
+-- src/
|
+-- tests/
|
+-- docs/
|   +-- architecture/
|
+-- CMakeLists.txt
+-- README.md
+-- .gitignore
´´´

# 15. Current Status

Architecture sections completed:

- 3.1 Architectural Vision
- 3.2 Architectural Model
- 3.3 Component Responsibilities & Contracts
- 3.4 Ownership & Dependency Injection
- 3.5 Architecture Baseline
- 3.6 Physical Project Structure
- 3.7 Physical Structure Review
- 3.8 Project Structure Implementation

The physical project structure has been implemented and reviewed.

Current project structure:

    app/
        main.cpp

    include/
        robot_core/

    src/

    tests/

    docs/
        architecture/

The application entry point is separated from the Robot Core
implementation area.

The CMake project has been updated accordingly, and the application
build and execution have been validated.

Next phase:

> Translate the architectural component model into minimal C++ types
> and contracts while preserving the dependency and ownership rules
> established by this baseline.

# 16. Final Engineering Principles

The Robot Core should evolve according to these principles:

``` text
Understand
    ->
Design
    ->
Document
    ->
Implement
    ->
Build
    ->
Test
    ->
Debug
    ->
Refactor
    ->
Document
    ->
Commit
```

The objective is not to build the largest architecture.

The objective is to build the smallest architecture that demonstrates
sound engineering and can evolve naturally toward professional robotics
software.
