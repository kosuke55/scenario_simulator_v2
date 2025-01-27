
// Copyright 2025 The Autoware Foundation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ACC_PID_CONTROLLER_HPP
#define ACC_PID_CONTROLLER_HPP

class AccPIDController
{
public:
  AccPIDController(double kp, double ki, double kd, double dt);
  double compute(double target, double current);

private:
  double kp_{0.0}, ki_{0.0}, kd_{0.0}, dt_{0.0};
  double prev_error_{0.0}, integral_{0.0};
};

#endif  // ACC_PID_CONTROLLER_HPP
