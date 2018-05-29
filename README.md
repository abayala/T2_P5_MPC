# CarND-Controls-MPC
Self-Driving Car Engineer Nanodegree Program

---
## Model and state description
In this project a Model Predictive controller was implemented to drive a car through a track, avoiding the car to get out of the road. 

To express the possible movement of the car in the track a Kinematic model was used, which incorporates the next state variables:

* x and y = Displaclement of the car in a flat world with the right handed coordinate frame, where x points to the front y to the left and z up. Measure units are meters

* psi = Yaw rate or change rate of the heading of the vehicle, which is the angle around the z axis. Measure units rads/seconds

* v = Magntiud of the linear velocity of the car towards the front 

To modify the previous variables the model has the next actuators that will move the car:

* acceleration (a) : Is a consecuence of how much the gas pedal is pressed, transmitting the power to the wheels . A possitive acceleration means that the car is moving forward, a negative acceleration means is going backwards. The measure units are m/s2 and is constrained for this project to lay on the range of [-1 , 1 ]  m/s2.

* steering angle (delta) : Is the steering angle of the front wheels, and due to the physical constrains of the car, it may lay between [-25 , 25] radians. 

The state vector then conatins [x, y, psi,v ] 

The update equations of each of the variables are :

x[t+1] = x[t] + v * cos ( psi ) * dt
y[t+1] = y[t] + v * sin ( psi ) * dt 
psi[t+1] = psi[t] - v * delta / Lf * dt
v[t+1] = v + a*dt

Where Lf is the distance from the center of mass of the car and the front axel. For this project Lf = 2.67 mts

