# CarND-Controls-MPC
Self-Driving Car Engineer Nanodegree Program

---
## Model and state description
In this project a Model Predictive controller was implemented to drive a car through a track, avoiding the car to get out of the road. 

To express the possible movement of the car in the track a Kinematic model was used, which incorporates the next state variables:

* x and y = Displaclement of the car in a flat world with a right handed coordinate system, where x points to the front y to the left and z up. Measure units are meters

* psi = heading of the vehicle, which is the angle around the z axis. Measure units rads

* v = Magntiud of the linear velocity of the car towards the front in m/s 

I also introduce the next two error measurements into the state vector, so they are also optimized in further steps

* cte ( cross track  error): Difference between my current position and the reference trajectory (center of the driving lane)

* epsi : Error in the heading angle 

To modify the previous variables the model has the next actuators that will move the car:

* acceleration (a) : Is a consecuence of how much the gas pedal is pressed, transmitting the power to the wheels . A possitive acceleration means that the car is moving forward, a negative acceleration means is going backwards. The measure units are m/s2 and is constrained for this project to lay on the range of [-1 , 1 ]  m/s2.

* steering angle (delta) : Is the steering angle of the front wheels, and due to the physical constrains of the car, it may lay between [-25 , 25] radians. 

The state vector then conatins [x, y, psi,v, cte, epsi ] 

The update equations of each of the variables are :

x[t+1] = x[t] + v * cos ( psi ) * dt
y[t+1] = y[t] + v * sin ( psi ) * dt 
psi[t+1] = psi[t] - v * delta / Lf * dt
v[t+1] = v[t] + a*dt
cte[t+1] = f(x[t]) -  y[t] + v[t] * sin ( epsi[t] ) * dt )
epsi[t+1] = ( psi[t] - psides[t] ) - v[t] * delta[t] / Lf * dt

where f is the 3rd degree polynomial of the reference or desired trajectory. 
Lf is the distance from the center of mass of the car and the front axel, in which for this project Lf = 2.67 mts according to the class material. 
psides[t] is the desired heading to reach, which can be calculated by computing the tangential angle of the deritave of the polynomial f, in other words psides[t] = arctan(f'(x))

## Development

After describing the state variables and their update functions I will describe now the steps followed to perform MPC

1.- I read from the simulator the state variables x,y,psi, delta and a. 
2.- I read from the simulator the desired trajectory waypoints in ptsx and ptsy
3.- since the waypoints are in global frame and I want them in local frame, I transformed them into vehicle coordinate frame by substracting the current position of the car in the world. thren i rotated them by the negative value of the current psi angle.

```c
//translate
double x = ptsx [ i ] - px;
double y = ptsy [ i ] - py;
//rotate 
ptsx [ i ] = x * cos ( -psi ) - y * sin ( -psi );
ptsy [ i ] = x * sin ( -psi ) + y * cos ( -psi );
```

4.- With the trasnformed waypoints I fitted a 3rd degree ploynomial and extracted its coefficients with the provided polyfit function

5.- To calculate cte[t], I used the coefficients computed in 4, and evaluated the polynomial at x =0  with the provided polyeval function. Since y =0 we have that cte[t] = polyeval ( coeffs , 0 )

6.-  To compute epsi[t] = psi[t] - psides[t], since psi[t] = 0, then epsi[t] = -arctan(coeffs[1])

7.- To compensate the latency of the system, I predicted the state by adding the displacement to the current state with the update equiations of the model
´´´
          double pred_px = 0.0 + v * dt; // Since psi is zero, cos(0) = 1, can leave out
          const double pred_py = 0.0; // Since sin(0) = 0, y stays as 0 (y + v * 0 * dt)
          double pred_psi = 0.0 + v * delta / Lf * dt; 
          double pred_v = v + a * dt;
          double pred_cte = cte - v * sin ( epsi ) * dt;
          double pred_epsi = epsi + v * delta / Lf * dt;
 ´´´         
8.- Then the coefficients and the predicted state is input to the solve function in the mpc.cpp file, for which I set some contraints in the state variables to optimize in the range of values previoulsy desribed for the actuators, and to [mindouble , maxdouble] for non acutators

9.- Regarding the function FG_Eval::void operator() function used inside solve function, I used the next wieghts in the error constraints vector fg following the recomendations in the class project walkthorugh:
´´´
        // The part of the cost based on the reference state.
        for ( int t = 0; t < N; t++ )
        {
            fg [ 0 ] += 2000* CppAD::pow ( vars [ cte_start + t ] , 2 );
            fg [ 0 ] += 2000* CppAD::pow ( vars [ epsi_start + t ] , 2 );
            fg [ 0 ] +=  CppAD::pow ( vars [ v_start + t ] - ref_v , 2 );
        }

        // Minimize the use of actuators.
        for ( int t = 0; t < N - 1; t++ )
        {
            fg [ 0 ] += 5 * CppAD::pow ( vars [ delta_start + t ] , 2 );
            fg [ 0 ] += 5 * CppAD::pow ( vars [ a_start + t ] , 2 );
           
        }

        // Minimize the value gap between sequential actuations.
        for ( int t = 0; t < N - 2; t++ )
        {
            fg [ 0 ] += 200 * CppAD::pow ( vars [ delta_start + t + 1 ] - vars [ delta_start + t ] , 2 );
            fg [ 0 ] += 10 * CppAD::pow ( vars [ a_start + t + 1 ] - vars [ a_start + t ] , 2 );
        }
´´´

Where it can be observed that the cte and epsi error are the ones with most weight. I also added a reference velocity term to avoid the car stopping in the curves, this is the ref_v = 70 km/h

Some important comments of the next part of the code 

´´´
            AD<double> f0 = coeffs [ 0 ] + coeffs [ 1 ] * x0 + coeffs [ 2 ] * pow ( x0 , 2 ) + coeffs [ 3 ] * pow ( x0 , 3 );
            AD<double> psides0 = CppAD::atan ( coeffs [ 1 ] + 2 * coeffs [ 2 ] * x0 + 3 * coeffs [ 3 ] * pow ( x0 , 2 ) );

            // Here's `x` to get you started.
            // The idea here is to constraint this value to be 0.
            //
           
            fg [ 1 + x_start + t ] = x1 - ( x0 + v0 * CppAD::cos ( psi0 ) * dt );
            fg [ 1 + y_start + t ] = y1 - ( y0 + v0 * CppAD::sin ( psi0 ) * dt );
            fg [ 1 + psi_start + t ] = psi1 - ( psi0 - v0 * delta0 / Lf * dt );
            fg [ 1 + v_start + t ] = v1 - ( v0 + a0 * dt );
            fg [ 1 + cte_start + t ] = cte1 - ( ( f0 - y0 ) + ( v0 * CppAD::sin ( epsi0 ) * dt ) );
            fg [ 1 + epsi_start + t ] = epsi1 - ( ( psi0 - psides0 ) - v0 * delta0 / Lf * dt );
´´´
 f0 and psides0 are evaluted with the computed coefficients and a 3rd degree polynomial. As seen in the code comments, there are also constraints in the state vector, it shall be equal to 0, for example x[t+1] - x[t] + v * cos ( psi ) * dt = 0
 

10.- The result from the solve function is extracted, where in vars[0] the steering value is stored, and which I dived by deg2rad(25)*Lf

11.- In vars[1] is the throttle value

12.- both computed values are sent as actuatoors to the simulator. 


 