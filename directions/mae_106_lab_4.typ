#set page(paper: "us-letter", margin: 0.5in, fill: white)
#set text(size: 10pt, fill: black)
#set par(justify: false, leading: 0.72em, spacing: 1.05em)
#set list(marker: [-], spacing: 0.5em, body-indent: 0.4em)
#set enum(spacing: 0.5em, body-indent: 0.4em)
#show list: set block(above: 0.8em, below: 0.8em)
#show enum: set block(above: 0.8em, below: 0.8em)

// Layout helpers with consistent vertical spacing hierarchy
#let sec(num, title, body) = {
  block(above: 1.4em, below: 0.75em, sticky: true)[
    #box(width: 1.4em, align(right, num))
    #h(0.4em)
    #title
  ]
  pad(left: 1.8em, body)
}

#let sub(label, body) = {
  block(above: 0pt, below: 1.05em)[#body]
}

#let cont(body) = {
  block(above: 0pt, below: 1.05em)[#body]
}

#let fig(path, w, caption) = {
  block(above: 0.4em, below: 1.05em, breakable: false, width: 100%)[
    #align(center)[
      #image(path, width: w)
      #v(0.25em)
      #caption
    ]
  ]
}

// Centered Title Block
#align(center)[
  #text(size: 16pt)[MAE106 Lab \#4 - Proportional control of DC motor velocity]
  #v(0.35em)
  #text(size: 11pt)[
    University of California, Irvine \
    Department of Mechanical and Aerospace Engineering
  ]
]
#v(0.5em)

#sec[1][Goals][
  #sub[a][Design a Proportional (P) velocity controller using a DC motor coupled with a rotary encoder.]
]

#sec[2][Parts & equipment][
  #sub[a][Equipment list:]
  #cont[
    #pad(left: -1.8em)[
      #align(center)[
        #table(
          columns: (auto, auto),
          stroke: 0.5pt,
          inset: 4pt,
          [*Qty*], [*Part/Equipment*],
          [1], [Arduino],
          [1], [Motor driver],
          [1], [DC motor with encoder],
        )
      ]
    ]
  ]
]

#sec[3][Introduction][
  #sub[a][As a mechanical and/or aerospace designer, you will sometimes want to control actuators on a machine, such as a robot, car, plane, or space vehicle, to make the machine move/behave in a desired way. The purpose of this laboratory is to learn how to use a microcontroller (e.g., an Arduino) to control a motor's angular velocity.]
  #cont[One of the major benefits of using a microcontroller is that you can express the control law and control gains in software. Changing the control law just involves typing in new software code. This will be very useful for your final project. Note that an alternate way to set up controllers is with analog circuit elements, such as op-amps. Such controllers may respond more quickly than a computer, but changing parameters requires changing, e.g., resistors or capacitors. With the decreasing cost and increasing speed of processors, it's more common now to implement control systems with microcontrollers.]
  #sub[b][The basic idea of computer-based control for this lab is to:]
  #cont[
    1. Electronically measure the system performance. In this lab, the system performance we are interested in, i.e., the thing we are trying to control, is the angular velocity of the motor shaft. We will measure this with a rotary encoder and the microcontroller. You will read the pulses from the encoder into two of the digital inputs of the Arduino using 2 interrupt service routines (ISRs). If the output of your sensor was analog, you could read it using one of the Arduino's analog pins.
    2. Program an appropriate control law to run on the microcontroller that will calculate and then generate an output voltage that is sent to the motor to control the angular velocity of the shaft.
  ]
]

#sec[4][Rotary encoder][
  #sub[a][A rotary encoder is a device that converts the rotation of its shaft into digital voltage pulses. Two common types of rotary encoders are optical and Hall Effect encoders.]
  #cont[Consider an optical encoder first (Figure 1), as they are easier to understand. An optical rotary encoder is made of a rotating disk with slits cut in it, and a light emitter shining light through the disk toward a light sensor. The disk is connected to the shaft of the encoder. As the shaft rotates, the disk alternately blocks or allows light to enter the light sensor, depending on whether there is currently a slit in front of the light emitter or not. Thus, each time a slit passes the light emitter, the light sensor outputs a pulse of voltage.]
  #cont[For an "incremental encoder" or "relative encoder" such as the one we are using in lab, the microcontroller measures the amount of rotation by counting the pulses being sent by the encoder. Incremental encoders typically have two light emitters and two light sensors, with two tracks of slits, which allows them to determine the direction of motion using "quadrature encoding."]
  #fig("figure_1.png", 34%, [Figure 1. Optical encoder (image from National Instruments)])
]

#sec[5][Hall Effect sensor][
  #sub[a][For the motor in this lab, the rotary encoder is a Hall Effect encoder, which produces voltage pulses just like an optical encoder (Figure 1). The difference is that the pulses are produced when a magnet on the shaft passes close to a Hall Effect sensor.]
  #cont[A Hall Effect sensor makes use of the Lorentz force law. When a magnetic field comes close to the sensor, it creates a Lorentz force on the charges flowing through the sensor, pushing them to one side of the sensor, thereby causing a small voltage difference in the direction perpendicular to current flow (see Figure 2). This voltage changes with the proximity and polarity of the magnetic field, so by putting magnets on a shaft, turning the shaft will cause the Hall Effect sensor to output voltage pulses.]
  #fig("figure_2.png", 36%, [Figure 2. Hall Effect sensor (image from http://www.akm.com/akm/en/product/add/magnetic_sensors/0029/)])
  #sub[b][For a detailed tutorial on using a Hall Effect sensor see: http://www.electronics-tutorials.ws/electromagnetism/hall-effect.html]
]

#sec[6][Interrupts][
  #sub[a][Interrupts are hardware signals that are used to temporarily stop the main program and allow a special program to run. Many microcontrollers, including the Arduino, have dedicated hardware that can continuously monitor a voltage on a pre-defined input line, called an "interrupt line" (your Arduino has 2 interrupt lines).]
  #cont[When the voltage on the interrupt input line changes, this hardware immediately tells the microcontroller to stop what it is doing and run the "Interrupt Service Routine" (ISR), which is a piece of code that does something in response to the "interruption" indicated on the interrupt line.]
  #cont[ISRs are useful when you have a device connected to a microcontroller that provides rapidly changing information at random times, such as rotary encoders. Rather than having your main program periodically check the encoder (in which case it might miss a pulse if it isn't checking often enough), you set up an ISR to respond to the encoder immediately when it provides new information.]
]

#sec[7][Reading from the encoder][
  #sub[a][There are six wires coming out of the encoder:]
  #cont[
    - Red wire: motor power,
    - Black wire: motor power,
    - Green wire: encoder GND,
    - Blue wire: encoder VCC (5 V),
    - Yellow: encoder A output,
    - White: encoder B output.
  ]
  #sub[b][The vendor of the DC motor provides instructions on using the encoder that is coupled to your DC motor (see section "Using the Encoder" in: https://www.pololu.com/product/3214, see also the video on the course website: `motorEncoder DemoMAE106.mp4`, and Figure 4). Make sure that you understand what happens when you spin the motor in both directions.]
  #fig("figure_3.png", 48%, [Figure 4. Output from Encoders A and B shown in an oscilloscope. (Image from: https://www.pololu.com/product/1442)])
]

#sec[8][Part 1: Proportional control of the velocity of the DC motor][
  #sub[a][We will use the encoder to measure the angular velocity of the motor shaft and use the measurement as a feedback signal to control the motor. Use the code provided on the course website (`Lab4Pvelocity.ino`), to control the velocity of the motor. Go through it and relate the parts of the code to the steps mentioned below.]
  #sub[b][To implement the P-type (proportional-type) velocity feedback controller, the program needs to:]
  #cont[
    1. read the signals from the encoders,
    2. compute the velocity of the motor,
    3. calculate the control law, and
    4. send the control signal to the motor driver.
  ]
  #sub[c][Control law: Since we are able to measure the velocity of the motor, the next step is to implement the control law and send the appropriate signal to the motor driver. The equation that describes the control law is: $u = -K_p (omega - omega_d)$]
  #cont[
    where: \
    $u =$ control signal to the motor driver, \
    $K_p =$ proportional gain, \
    $omega =$ motor velocity, \
    $omega_d =$ desired motor velocity.
  ]
  #cont[The basic idea of this controller is to measure the velocity error of the motor shaft and then apply torque that is proportional to those errors (hence the name proportional-type controller), so that the motor shaft turns at an angular velocity closer to the desired angular velocity. If the error is zero, the correction torque to be applied is zero; if the error is large, the correction torque to be applied is large.]
  #fig("figure_4.png", 48%, [Figure 5. Motor and encoder connections to motor driver (NOTE: the color of the wires drawn here, between the motor's 6-pin connector and the shield, correspond to the color of wire they should be connected to)])
  #sub[d][Wiring and execution procedure: Wire the motor and its encoder as shown in Figure 5. PLEASE READ THE APPENDIX AT THIS POINT. Make sure that you uploaded the code to the Arduino. If you reset your Arduino, the motor should spin for a short amount of time and then stop following the procedure:]
  #cont[
    1. Turn the motor for a short time (~1 s) attempting to follow $omega_d$, while collecting data.
    2. Motor stops.
    3. Arduino prints data collected from Step 1 to Serial Monitor.
    4. Waits for the Arduino to restart, or the Serial Monitor window to be opened, then starts over at Step 1.
  ]
  #sub[e][Serial Monitor note: There is a bug in Arduino IDE 2.X that makes it difficult to copy data more than one screen of data from the serial port. If you use an older version of Arduino IDE (e.g. 1.8.19), it doesn't have this problem and you can use ctrl-a to copy the data. Or you can use a terminal program such as one called CoolTerm. If you use CoolTerm, you should replace the tabs with dashes in the print lines in the sketch so CoolTerm works properly.]
  #sub[f][*Practical Exam \#1:* Collect the data for $K_p = 1, 2, 3,$ and $4$. Make sure to set `desVel_amplitude` ($omega_d$) to $60 "rad/s"$. Hold the body of the motor tightly while collecting data! Make sure that `constantVelocity` is set to `true`. Plot the four curves on the same plot and show your instructor. Be prepared to explain the effect of $K_p$ on the steady state error and the time constant. Save your data for the report!]
]

#sec[9][Part 2: Frequency response for the velocity of the DC motor][
  #sub[a][The goal of this part of the lab is to characterize the frequency response of the system and show that it resembles the behavior of a low pass filter. In particular, you will explore how well the system tracks the desired input velocity when the input is a sinusoid, across a range of frequencies, following the equation: $omega_d = A times sin(2 pi times f times t)$]
  #sub[b][To characterize the frequency response of the system, follow the steps below. REMEMBER: DO NOT LET YOUR MOTOR OVERHEAT.]
  #cont[
    1. Set `Kp` equal to 4;
    2. Set `constantVelocity` to `false`;
    3. Set `desVel_amplitude` ($A$) to 60;
    4. Set `desVel_frequency` ($f$) to the desired frequency.
  ]
  #sub[c][*Practical Exam \#2:* Collect the data for five different frequencies by setting `desVel_frequency` ($f$) $= 1, 5, 10, 20,$ and $40$. Hold the body of the motor tightly while collecting data! Make sure that `constantVelocity` is set to `false`. Plot the four curves on separate plots and show your TA. Be prepared to explain the effect of frequency on the amplitude and phase shift and explain how you would measure amplitude. Save your data for the report!]
]

#sec[10][Lab Report][
  #sub[a][Present the following five plots:]
  #cont[
    1. Motor speed as a function of time for all values of $K_p$. Also include the desired velocity (part 1).
    2. Use the control law to calculate the control input ($u$) that was applied at each data point collected. Plot $u$ as a function of time for each value of $K_p$ in a single figure (part 1).
       - Write the control law. Briefly explain how it used the desired and actual velocities.
    3. Steady state error as a function of $K_p$ (part 1).
       - Is this plot roughly consistent with the equation relating steady state error and $K_p$ from class? Explain.
    4. Time constant as a function of $K_p$ (part 1).
       - Is this plot roughly consistent with the equation relating the time constant and $K_p$ from class? Explain.
       - Hint: It may be easier to measure 5 time-constants, which is the time at which signal reaches 99.3% of steady state.
    5. Amplitude of output sine wave as a function of the frequency (part 2).
       - Explain why this data resembles a low pass filter. What happens to low frequencies? High frequencies?
  ]
]

#sec[11][Appendix][
  #sub[a][Power-up and connection sequence: There is a sequence to follow when connecting power supply, motor, Arduino, and computer. If this sequence is not followed, you may get '0 velocity' data—data where either the motor does not move, or the movement is not recorded. Alternatively, if the power supply is not set correctly before attaching it to the motor, you may experience unexpected or inconsistent motor behavior. Please follow this sequence:]
  #cont[
    1. Wire together the motor's wire header to the Arduino shield, if the setup has not yet been assembled. Follow the circuit diagram above (Figure 5), paying particular attention to the colors of wires coming from the motor and which pins on the motor shield they attach to.
    2. Upload the appropriate Arduino `.ino` file to the Arduino + Shield, before you have connected the motor shield to the power supply. This ensures whatever code is already on the Arduino cannot interfere with the motor setup.
    3. Set the power supply to 12V, before it is connected to the shield. Use the COARSE setting on the VOLTAGE side to make large voltage adjustments, the FINE settings for small adjustments, until the power supply is set to ~12V. Then connect the power supply to the shield. Be careful that the alligator clips from the power supply do not touch each other, nor can they be connected through a conductive surface (ex. a metal laptop frame). We recommend using a physical barrier, such as a sheet of paper or your jumper wire bin, to keep the alligator clips separated. If you try to run the Arduino code before the power supply is turned on and connected to the shield, the code will run but the motor will not move (since it has no voltage source).
    4. Begin to run the Arduino code by opening the Serial Monitor. Uploading the code will make the motor move, but if the Serial Monitor is not open, data won't be retrievable.
    5. When you first run the Arduino code, you may get 'junk' data printed in the first few lines of Serial Monitor. This is 'junk' that was in the Arduino's memory when you declared the array to store the motor velocity data, without initializing the contents of the array. You can get a 'clean' run by pressing the 'reset' button on the Arduino (button next to USB port).
  ]
]