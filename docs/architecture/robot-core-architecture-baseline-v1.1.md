---
author:
- Robot Core Project
date: Version 1.1 --- 2026-08-26
title: Robot Core Architecture Baseline
---

# 1. Purpose

This document establishes the architectural baseline for the Robot Core
project.

The purpose of the baseline is to define the system's architectural
intent, component boundaries, responsibilities, contracts, ownership
model, dependency direction, dependency injection strategy, and the
initial C++ domain model.

This document is a reference for both development and architectural
review. The implementation should remain consistent with these decisions
unless a deliberate architectural change is made and documented.

Version 1.1 incorporates the decisions established during Phase 4.1 ---
C++ Types & Contracts.

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
-   Strong domain typing where it provides concrete value
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
-   Sensors and perception implementations
-   State estimation
-   Planning systems
-   State machines

They may be introduced later when a concrete requirement justifies them.

# 3. Architectural Model

The initial conceptual model consists of five primary components plus
domain data concepts for state, objectives, positions, and commands.

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

RobotCore coordinates the system and makes decisions possible, but it
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
-   configuring the initial Controller objective;
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

## 4.2 RobotCore

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
-   contain GUI responsibilities;
-   determine high-level control decisions.

### Conceptual contract

``` text
RobotCore
    update()
        |
        +-- read current state
        +-- request control decision
        +-- execute resulting command
```

## 4.3 RobotState

### Responsibility

RobotState represents the software's current knowledge of the robot's
state.

It is primarily a domain data model.

The minimum initial state consists of:

-   operational status;
-   current spatial position.

### Operational status

For the initial implementation, operational status represents whether
the capabilities and required supporting components needed for operation
are available for use.

RobotState represents this knowledge but does not determine it by
directly inspecting hardware or infrastructure.

### Spatial position

The initial RobotState contains a three-dimensional Position.

Position is represented in a fixed World Frame. The first implementation
defines the World Frame origin at system initialization.

The initial spatial model is:

``` text
Position
    x : Length
    y : Length
    z : Length
```

where Length represents a physical length expressed in meters.

### It must not

-   access hardware;
-   read sensors directly;
-   control actuators;
-   decide robot behavior;
-   depend on hardware drivers;
-   contain high-level objectives or mission definitions;
-   contain perception data merely because future behaviors may require
    it.

The state model should not need to know where its data originated.

### Conceptual contract

``` text
RobotState
    represents:
        operational status
        current position
```

## 4.4 Position

### Responsibility

Position represents a three-dimensional spatial point in the Robot Core
domain.

### Initial spatial conventions

``` text
Dimension:
    3D

Unit:
    meter

Coordinates:
    X, Y, Z

Reference:
    fixed World Frame

Origin:
    defined at system initialization
```

Position is a domain value and should not be represented as an
unstructured collection of unrelated numeric values.

### It must not

-   access sensors;
-   determine its own reference frame;
-   perform localization;
-   represent environmental perception;
-   contain robot behavior.

Future systems may introduce additional reference frames, but the
initial implementation uses only the World Frame.

## 4.5 Length

### Responsibility

Length represents a physical length in meters.

Length is a strong domain type rather than an alias such as:

``` cpp
using Length = double;
```

The underlying numeric representation is an implementation detail and
must not be treated as the semantic meaning of the type.

The initial implementation may use a floating-point numeric
representation internally, subject to validation through implementation
and tests.

### Design principle

The distinction is:

``` text
Semantic type:
    Length

Underlying representation:
    numeric type
```

The project should not introduce a complete physical-units framework at
this stage. Additional physical types should be introduced only when a
concrete requirement justifies them.

## 4.6 Controller

### Responsibility

Controller transforms the current robot state and its configured control
objective into a desired action.

Conceptually:

``` text
Command = Controller(State)
```

The Controller receives RobotState during each control cycle.

The Controller maintains or receives its current control objective
through configuration/dependency injection external to RobotState.

### Initial behavior

The first Controller supports the following conceptual cases:

``` text
Robot not operational
    -> STOP

Robot operational + no objective
    -> WAIT

Robot operational + target not reached
    -> MOVE

Robot operational + target reached
    -> STOP
```

### It must not

-   access hardware directly;
-   write to motors;
-   know concrete hardware drivers;
-   initialize the application;
-   own infrastructure concerns;
-   determine whether the robot is operational by directly inspecting
    hardware.

The Controller decides; the Actuator executes.

## 4.7 ControlObjective

### Responsibility

ControlObjective represents what the Controller is currently expected to
accomplish.

The objective is distinct from RobotState.

RobotState answers:

``` text
What is true about the robot?
```

ControlObjective answers:

``` text
What should the Controller accomplish?
```

### Initial objective model

The first implementation recognizes:

``` text
No Objective

Reach Target Position
    |
    +-- target Position
```

The objective is configured externally and does not belong to
RobotState.

The initial implementation does not introduce a Planner, Mission
Manager, Task Manager, or State Machine.

## 4.8 RobotCommand

### Responsibility

RobotCommand represents a desired action at the robot-domain level.

It should describe intent rather than hardware implementation details.

The initial command model contains the following high-level actions:

``` text
WAIT
STOP
MOVE
```

The initial MOVE command is intentionally abstract and does not
prescribe motor-level parameters.

Good conceptual representation:

``` text
Desired action = MOVE
```

Poor domain representation:

``` text
PWM channel 3 = 127
```

Hardware-specific parameters belong to infrastructure or actuator
implementations.

## 4.9 Actuator

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

The Controller additionally has a configured ControlObjective that is
external to RobotState:

``` text
                 ControlObjective
                        |
                        v
RobotState ------> Controller
                        |
                        v
                  RobotCommand
                        |
                        v
                    Actuator
```

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
    v
Controller
    |
    v
Domain Concepts
    |
    v
Interfaces / Abstractions
    |
    v
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
-   sensor drivers
-   perception implementations

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

## 8.2 Controller objective configuration

The Controller's ControlObjective is configured externally rather than
stored in RobotState.

The initial architecture should prefer simple configuration or
constructor/setter mechanisms as appropriate to the implementation
rather than introducing a dedicated objective-management subsystem.

## 8.3 Why dependency injection?

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
-   add abstractions when there is a concrete need;
-   use strong domain types when they provide meaningful semantic
    protection;
-   represent Position as a 3D domain value in meters;
-   use a fixed World Frame for the initial spatial model;
-   keep ControlObjective separate from RobotState;
-   allow an operational robot to have no active objective;
-   allow the Controller to produce WAIT or STOP without a movement
    objective;
-   keep RobotCommand at the robot-domain intent level.

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
-   add technologies solely because they are popular;
-   introduce a full units-of-measure framework without a concrete need;
-   introduce sensors or perception implementations;
-   introduce obstacle avoidance;
-   introduce velocity control;
-   introduce orientation control;
-   introduce kinematics or dynamics;
-   introduce a planner, mission manager, task manager, or state
    machine;
-   introduce ROS 2, Qt, or simulation infrastructure without a
    justified requirement.

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
Perception / State Estimator
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
-   Perception
-   State Estimator
-   Additional reference frames
-   Orientation
-   Velocity
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

The central architectural contract remains:

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

The Controller also has a configured ControlObjective that remains
separate from RobotState:

``` text
ControlObjective
       |
       v
Controller <---- RobotState
       |
       v
RobotCommand
```

The fundamental responsibility split is:

``` text
RobotState       -> represents
ControlObjective -> defines what should be accomplished
Controller       -> decides
Command          -> expresses intent
Actuator         -> executes
RobotCore        -> orchestrates
Application      -> composes and owns
```

The initial minimum behavioral contract is:

``` text
Not operational
    -> STOP

Operational + no objective
    -> WAIT

Operational + ReachTargetPosition + target not reached
    -> MOVE

Operational + ReachTargetPosition + target reached
    -> STOP
```

"Reached target" is a derived control condition based on the
relationship between the current Position and the target Position. A
dedicated `has_reached_target` field is not part of the initial
RobotState.

# 13. Implementation Guidance

The implementation phase should follow this sequence:

``` text
Architecture Baseline
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
Integration Tests
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

The first implementation should materialize the domain model
incrementally:

``` text
Length
    |
    v
Position
    |
    v
RobotState
    |
    v
ControlObjective
    |
    v
RobotCommand
    |
    v
Controller
    |
    v
Actuator
    |
    v
RobotCore
```

The implementation should begin with the smallest useful system and
should not introduce future subsystems prematurely.

Initial validation should cover at least:

-   Length represents a valid meter value.
-   Position represents a 3D World Frame position.
-   RobotState represents operational status and current position.
-   A Controller with no objective produces WAIT when operational.
-   A non-operational robot does not produce MOVE.
-   ReachTargetPosition produces MOVE when the target has not been
    reached.
-   ReachTargetPosition produces STOP when the target has been reached.
-   RobotCore forwards the resulting command to the Actuator.

# 14. Change Policy

This document represents Architecture Baseline v1.1.

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

# 15. Current Status

Architecture sections completed:

-   Architectural Vision
-   Architectural Model
-   Component Responsibilities & Contracts
-   Ownership & Dependency Injection
-   Architecture Baseline
-   Initial C++ Domain Model
-   Initial Control Objective Model
-   Initial Spatial Model

Phase 4.1 --- C++ Types & Contracts is complete at the conceptual/design
level.

Next phase:

> Materialize the approved domain model and contracts in C++/CMake,
> beginning with the `Length` domain type and its tests.

The implementation should follow the engineering workflow:

``` text
Understand
    ->
Design
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

# 16. Final Engineering Principles

The Robot Core should evolve according to these principles:

``` text
Understand
    ->
Design
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

Version 1.1 records the decisions made during Phase 4.1 without treating
future robotics capabilities as current implementation requirements.
