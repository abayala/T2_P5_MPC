# CarND-Controls-MPC
Self-Driving Car Engineer Nanodegree Program

---
## Model and state description
In this project a Model Predictive controller was implemented to drive a car through a track, avoiding the car to get out of the road. 

To express the possible movement of the car in the track a Kinematic model was used, which incorporates the next state variables:

* x and y = Displaclement of the car in a flat world with the right handed coordinate frame, where x points to the front y to the left and z up. Measure units are meters

* psi = heading of the vehicle, which is the angle around the z axis. Measure units rads

* v = Magntiud of the linear velocity of the car towards the front 
.
I also introduce the next two error measurements into the state vector, so they are also optimized in further steps

* cte ( cross track  error): Difference between my current position and the reference trajectory (center fo the driving lane)

* epsi : Error in the heading angle 

To modify the previous variables the model has the next actuators that will move the car:

* acceleration (a) : Is a consecuence of how much the gas pedal is pressed, transmitting the power to the wheels . A possitive acceleration means that the car is moving forward, a negative acceleration means is going backwards. The measure units are m/s2 and is constrained for this project to lay on the range of [-1 , 1 ]  m/s2.

* steering angle (delta) : Is the steering angle of the front wheels, and due to the physical constrains of the car, it may lay between [-25 , 25] radians. 

The state vector then conatins [x, y, psi,v ] 

The update equations of each of the variables are :

x[t+1] = x[t] + v * cos ( psi ) * dt
y[t+1] = y[t] + v * sin ( psi ) * dt 
psi[t+1] = psi[t] - v * delta / Lf * dt
v[t+1] = v[t] + a*dt
cte[t+1] = f(x[t]) -  y[t] + v[t] * sin ( epsi[t] ) * dt )
epsi[t+1] = ( psi[t] - psides[t] ) - v[t] * delta[t] / Lf * dt

where f is the 3rd degree polynomial of the reference or desired trajectory. 
Lf is the distance from the center of mass of the car and the front axel and for this project Lf = 2.67 mts according to the class material. 
psides[t] is the desired heading to reach, which can be calculated by computing the tangential angle of the deritave of the polynomial f, in other words psides[t] = arctan(f'(x))

## Development

After describing the state variables and their update functions I will describe now the steps followed to perform MPC

1.- I read from the simulator the state variables x,y,psi, delta and a. 
2.- I read from the simulator the desired trajectory waypoints in ptsx and ptsy
3.- since the waypoints are in global frame and I want them in local frame, I transformed them into vehicle coordinate frame by substractin the current position of the car in the world and rotation by the negative value of the current psi angle.

´´´
              //translate
              double x = ptsx [ i ] - px;
              double y = ptsy [ i ] - py;
              //rotate 
              ptsx [ i ] = x * cos ( -psi ) - y * sin ( -psi );
              ptsy [ i ] = x * sin ( -psi ) + y * cos ( -psi );
´´´
4.- With the trasnformed waypoints I fitted a 3rd degree ploynomial and extracted its coefficients with the provided polyfit function

5.- With the coefficients I 

5.- 

