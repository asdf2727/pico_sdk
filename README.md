# Line follower & obstacle avoidance robot

## Introduction

This is my solution for the CSE2425 (2025-2026 edition) robot lab, from TU Delft. It runs on a RP2040 and rebuilds most of the library from the ground up, using only the registers and structs documented in the datasheet.

## Table of contents

- [Introduction](#introduction)
- [Table of contents](#table-of-contents)
- [Usage](#usage)
- [Structure](#structure)
  - [Hardware Library](#hardware-library)
    - [Pulse Width Modulation](#pulse-width-modulation)
    - [Analog to Digital Converter](#analog-to-digital-converter)
  - [Components Library](#components-library)
    - [Wheel Encoder](#wheel-encoder)
    - [Infrared Sensors](#infrared-sensor)
    - [DC Motors](#dc-motors)
    - [Ultrasonic Distance Sensor](#ultrasonic-distance-sensor)
  - [Task Code](#task-code)
    - [Movement Utilities](#movement-utilities)
    - [Task 1](#task-1)
    - [Task 2](#task-2)
    - [Task 3](#task-3)
    - [Task 4](#task-4)
    - [Task 5](#task-5)
- [Contribution & Forking](#contribution--forking)

## Usage

The code uses the [Raspberry Pi Pico SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html), so first install that and then build it. The pins used are defined in `src/comp/all.c` and can be changed up to hardware limitations. In order to change the running program, go into `src/main.c` and edit the contents of `loop()`. Do not edit `main()` directly, as the setup and hardware logic ran there is required for the proper functioning of the robot.

## Structure

The project is divided into three main parts: the [hardware library](#hardware-library), the [components library](#components-library), and the implementation of [tasks](#task-code).

### Hardware library

Part of the requirements of the project were to rebuild the sdk from the ground up, in order to learn about the low level functionality of embeded processors. The code can be seen in `src/my_hardware` and implements low level functionality useful for interfacing with the rest of the robot, such as simple IO (`sio.h`), pulse width modulation setup and editing (`pwm.c`), setup of the analog-digital converter (`adc.c`), and other utilities for timing (`timer.c`, `pll.c`), interupts (`irq.h`) and general register manipulation (`utils.h`). Among these, some interesting highligts are:

#### Pulse width modulation

Seen in `pwm.c` and `pwm.h`, this code enables pwm on a channel and edits the pulse width. The main part of the code is however responsible for abstracting away the hardware level functionality, enabling the use of units such as frequency (pulses/second) and percentage. Once the PWM is set up, the percentage can be easily converted into an actual threshold, but setting it up requires setting up the clock divider and the counter loop back at the same time in order to acheive the frequency desired.

The first step in this is setting up the loop back value to the maximum possible (assuming no clock divider), in order to maintain the maximum precision possible for pulse width resolution. After this is decided, the clock divider is computed to match the requested frequency as closely as possible.

#### Analog to Digital Converter

In order to turn the raw voltage from the infrared sensors into useful data, setting up and reading from the ADC is required. However, we only have one ADC in the RP2040 that can read from 5 channels, and the actual decoding of the signal takes 96 clock cycles using the USB 48MHz clock. If we want real time, accurate data, without pausing to compute or handling interupts every few microseconds, an advanced solution is required.

The ADC has a builtin FIFO output buffer of size 8, and a round-robin style mechanism, for quickly and automatically switching between inputs. The code in `adc.c` sets up these 2 mechanisms such that the ADC cycles through all requested inputs twice, and then triggers an interrupt that updates the value at a given pointer using these measurements. This makes sure that accurate and recent data is always available in the values written to by the IRQ, without any code required from the user, and an IRQ that triggers rarely.

There is one problem remaining however, that being that if the buffer fills up, results start being thrown out, and the parity of our fifo breaks (the IRQ thinks it's reading from 0,1,0,1 when it is actually reading from 0,1,1,0). This can be remedied however by checking if an overflow or underflow of the FIFO has occured, and resetting the ADC to get it back into a valid state. This is part of the hardware maintenance part of the loop, implemented by `adc_sanity_check()`.

*Note:* originally, there was an attempt to make the DMA move the data from the ADC FIFO in order to remove interrupts completely and acheive much greater polling frequencies, but unfortunatelly I coudn't get it to work in time for the final presentation. A WIP version of the DMA setup can be seen in `dma.c`.

### Components library

The next layer of the program manages the various components that control or obtain information for the robot. All the components are colectivlly defined and setup in `all.c`, together with the general IRQ for GPIO pins, that are handled on a case by case basis by different components. Any change or addition of hardware should be first handled there. More detail follows for each component:

#### Wheel encoder

Provided for only one of the 2 motors is a cheap light based wheel encoder, reading if light can pass through a perforated wheel with 20 spokes along it's circumference. The digitalised output of the light sensor is given directly to the processor, so interupts on every falling an rising edge are required in order to count how many spokes have passed over the sensor, and as a result how much the wheel has turned.

This is what is implemented in `encoder.c`, the IRQ storing only the time at which it has been triggered, and the rest of the logic for counting and speed being handled by the update function running as part of the hardware maintenance loop.

One annoying problem is that the encoder sometimes glitches and outputs very short pulses, that would count as extra inexistent spokes that pass by at incredible speeds. This can be fixed by ignoring any short pulses, but the limit where a glitch turns into an actual signal is unclear.

Another annoying problem is that the encoder is actually incapable of determining the direction the wheel turns, and needs to be given that information from the motor itself. This is generally not a problem, but if the motor switches directions quicky, the encoder might think that the wheel is turning the opposite direction instantly, even though the inertia of the wheel still turns it a bit in the original direction.

#### Infrared sensor

The infrared sensor takes the raw 13 bit value (remember, we add 2 measurements from the ADC), normalises them linearly and uses that as a measure of how light or dark the surface under it is. This works **amazingly** well in reliable and constant conditions (often giving accuracy in the order tens of micrometers when directly on the line), but the whole system brakes under inconsistent lighting, a problem I experienced in the final presentation. My hacky solution was to turn the normalised signal into a binary 0/1 by considering anything above a 0.2 to be a 1. This works better than nothing, but it really messes up with the line followed in the layer above, causing it to behave eratically.

#### DC Motors

The robot moves with the help of 2 DC Motors that can move independently. These motors are however, weak, unreliable, nonlinear, with a gigantic static friction, and generally cheap. In order to get any use out of them I needed to add around 4 filters, turning the raw speed (cm/s) request into an actual pulse width to be given to the hardware layer.

The first step is to obtain a relation between the raw pulse width given to the motors and the resulting speed at which the wheel moves. This is done in `src/solution/calibrating.c` by running the pulse width from a triangle signal, continually reading the real speed from the encoder, and then solving a quadratic regression in order to get the best fitting parameters. These are then given when creating the motor struct and used every time speed needs to be turned into pulse width.

After the pulse width is determined, some postprocessing steps need to be done in order to make sure the wheels actually move. There are first limiting the minimum pulse width in order to give enough powers to the motor to overome friction, and then overriding full throttle every time the direction of the speed changes, in order to overcome static friction and getting the motor to actually start moving in the opposite direction. This is then kept for a short duration by the acceleration limiter, also put in place in order to avoid wheel slip.

#### Ultrasonic distance sensor

The final component is the ultrasonic sensor, reading the distance to the closest object in front of it by sending a short sound pulse and waiting for the response. This is done by pulling high the trigger pin for a few microseconds (over 10us from the datasheet), and then listening for a signal on echo pin, the duration of which corresponds with the duration of the sound pulse.

Everything with this component is straightforward, done with SIO and interupts. One caveat is that a pulse cannot be sent to frequently, otherwise the sensor will hear the echo from a previous pulse and terminate too early. The datasheet recommends 60ms, so that is the value I use.

As for precision, this is one of the easier components. The sensor sometimes grossly overestimates, but never underestimates, so the minimum of the last reads can be used with no problem. The precision of the sensor itself is of a few milimiters, so I opted to return the computed distance as an integer of the milimiters, in order to minimise floating point operations that are expensive and prone to accumulated errors.

### Task Code

We finally get to the actual solution of the tasks, which include line following, obstacle detection and avoidance, and distance travelling. All code for the tasks can be seen in `src/solution/taskX.c`.

#### Movement Utilities

Before we get into the actual solution, we first need to discuss some movement utilies used in all the tasks. These are implemented in `src/solution/move.c`, and include straight and turned movement, stopping, and line following, all with variable speeds (given in percentage of maximum speed). Straight movement and stopping are straightforward (pun intended), while rotated movement is a bit more complex to allow for any turning radius and speed.

The main highlight however is the line follower, for which there are two versions. One of them is a straightforward PID running on the offset from the IR. The other, however, is a much more advanced solution that analitically resolves the current state of the car and computes the movement required to get it back on track. I'm pretty sure that nobody bothered to read this far, but in case I'm wrong, thank you for the attention, and this is how it works: I consider the robot to have a position X, Y and a rotation theta and a sensor in front of it that reads the distance to Ox, divided by cos(theta), since the sensor sees a "wider" line when at an angle. In this model, each movement step following the Euler method, where you first move forward then rotate. Both the previous and current states are unknown, but what is known is the displacement between them and the value from the sensor. This is actually enough to resolve both states, up to displacement along the line. An exact solution is possible, but it requires resolving a cubic and some more, so I opted for an aproximation by considering the rotation and initial theta to be small. From there you can obtain the rotation speed required to get the sensor on top of Ox after a given distance moved. The theory for this model is sound, and an implementation can be seen in [this desmos graph](https://www.desmos.com/calculator/c61qjgxort), but the robot hardware does not have the precision required to run this propperly.

#### Task 1

The first task is to run the wheels at a variable speed, which can be easily done with a schedule that updates the speed of the wheels based on the time.

#### Task 2

The second task requires moving in a straight line and stopping before hitting the object in front of you. This can be done by moving with a speed dependent on the distance given by the ultrasonic, and when that distance is small enough, stopping.

#### Task 3

The third task requires following a line and turning at corners, so a state machine that alternates between rotation and following is used.

#### Task 4

This task adds upon the previous by detecting obstacles and turning around them before going back to following the line. This uses 4 states, being follow -> obstacle turn 1 -> obstacle turn 2 -> intersection turn -> follow. This handles both intersections and obstacle turns with minimum code duplication.

#### Task 5

This requires moving along the line for a determined length. This can be done similarly to task 2, but using the remaining distance computed using the encoder instead of the ultrasonic.

## Contribution & Forking

I'm not planning on working on this anymore, as I even had to give back the robot after the course, so if you want to work on this or use this code in any other way, please fork it and credit me appropriately.
