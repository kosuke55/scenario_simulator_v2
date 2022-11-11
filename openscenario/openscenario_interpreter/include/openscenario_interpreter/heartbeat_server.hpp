// Copyright 2015 TIER IV, Inc. All rights reserved.
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

#ifndef OPENSCENARIO_INTERPRETER__HEARTBEAT_SERVER_HPP_
#define OPENSCENARIO_INTERPRETER__HEARTBEAT_SERVER_HPP_

#include <rclcpp_lifecycle/lifecycle_node.hpp>
#include <std_srvs/srv/empty.hpp>

namespace openscenario_interpreter
{
class HeartbeatServer
{
private:
  std::shared_ptr<std::promise<void>> update_notifier = nullptr;

  rclcpp::Service<std_srvs::srv::Empty>::SharedPtr service;

  rclcpp::CallbackGroup::SharedPtr service_callback_group;

public:
  explicit HeartbeatServer(rclcpp_lifecycle::LifecycleNode & node)
  : service_callback_group(node.create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive))
  {
    using namespace std::placeholders;
    service = node.create_service<std_srvs::srv::Empty>(
      "heartbeat", std::bind(&HeartbeatServer::handleHeartbeat, this, _1, _2),
      rmw_qos_profile_services_default, service_callback_group);
  }

  auto handleHeartbeat(
    const std::shared_ptr<std_srvs::srv::Empty::Request> request,
    std::shared_ptr<std_srvs::srv::Empty::Response> response) -> void
  {
    if (update_notifier == nullptr) {
      update_notifier = std::make_shared<std::promise<void>>();
      auto future = update_notifier->get_future();
      future.get();
      update_notifier = nullptr;
    } else {
      throw std::runtime_error(
        "Multiple heartbeat requests are accumulated. main loop in openscenario_interpreter may "
        "not be responding");
    }
  }

  void update()
  {
    if (update_notifier) {
      update_notifier->set_value();
    }
  }
};
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__HEARTBEAT_SERVER_HPP_
