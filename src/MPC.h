#ifndef MPC_H
#define MPC_H

#include <vector>
#include "Eigen-3.3/Eigen/Core"

using namespace std;

class MPC {
 public:
  MPC();

  virtual ~MPC();

  // Solve the model given an initial state and polynomial coefficients.
  // Return the first actuatotions.
  vector<double> Solve(Eigen::VectorXd state, Eigen::VectorXd coeffs);
  /*
  * \brief Function to  compute the next state given a current state input vector and an actuator vector
  * \return the next state vector , .*/
  Eigen::VectorXd globalKinematic ( Eigen::VectorXd state , Eigen::VectorXd actuators , double dt );
};

#endif /* MPC_H */
