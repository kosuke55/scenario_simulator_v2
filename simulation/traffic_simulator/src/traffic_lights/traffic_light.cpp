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

#include <limits>
#include <scenario_simulator_exception/exception.hpp>
#include <string>
#include <traffic_simulator/color_utils/color_utils.hpp>
#include <traffic_simulator/traffic_lights/traffic_light.hpp>
#include <unordered_map>
#include <utility>
#include <vector>

namespace traffic_simulator
{
auto TrafficLight::Color::make(const std::string & name) -> Color
{
  try {
    return table.at(name);
  } catch (const std::out_of_range &) {
    throw common::SyntaxError("Invalid traffic light color name ", std::quoted(name), " given.");
  }
}

auto operator>>(std::istream & is, TrafficLight::Color & color) -> std::istream &
{
  std::string name;
  is >> name;
  color.value = TrafficLight::Color::make(name);
  return is;
}

auto operator<<(std::ostream & os, const TrafficLight::Color & color) -> std::ostream &
{
  switch (color.value) {
    case TrafficLight::Color::green:
      return os << "green";
    case TrafficLight::Color::yellow:
      return os << "yellow";
    case TrafficLight::Color::red:
      return os << "red";
    case TrafficLight::Color::white:
      return os << "white";
    default:
      return os;
  }
}

auto TrafficLight::Status::make(const std::string & name) -> Status
{
  try {
    return table.at(name);
  } catch (const std::out_of_range &) {
    throw common::SyntaxError("Invalid traffic light status name ", std::quoted(name), " given.");
  }
}

auto operator>>(std::istream & is, TrafficLight::Status & status) -> std::istream &
{
  std::string name;
  is >> name;
  status.value = TrafficLight::Status::make(name);
  return is;
}

auto operator<<(std::ostream & os, const TrafficLight::Status & status) -> std::ostream &
{
  switch (status.value) {
    case TrafficLight::Status::solid_on:
      return os << "solidOn";
    case TrafficLight::Status::solid_off:
      return os << "solidOff";
    case TrafficLight::Status::flashing:
      return os << "flashing";
    default:
    case TrafficLight::Status::unknown:
      return os << "unknown";
  }
}

auto TrafficLight::Shape::make(const std::string & name) -> Shape
{
  try {
    return table.at(name);
  } catch (const std::out_of_range &) {
    throw common::SyntaxError("Invalid traffic light shape name ", std::quoted(name), " given.");
  }
}

auto operator>>(std::istream & is, TrafficLight::Shape & shape) -> std::istream &
{
  std::string name;
  is >> name;
  shape.value = TrafficLight::Shape::make(name);
  return is;
}

auto operator<<(std::ostream & os, const TrafficLight::Shape & shape) -> std::ostream &
{
  switch (shape.value) {
    case TrafficLight::Shape::circle:
      return os << "circle";
    case TrafficLight::Shape::cross:
      return os << "cross";
    case TrafficLight::Shape::left:
      return os << "left";
    case TrafficLight::Shape::down:
      return os << "down";
    case TrafficLight::Shape::up:
      return os << "up";
    case TrafficLight::Shape::right:
      return os << "right";
    case TrafficLight::Shape::lower_left:
      return os << "lowerLeft";
    case TrafficLight::Shape::upper_left:
      return os << "upperLeft";
    case TrafficLight::Shape::lower_right:
      return os << "lowerRight";
    case TrafficLight::Shape::upper_right:
      return os << "upperRight";
    default:
      return os;
  }
}

auto TrafficLight::Bulb::make(const std::string & s) -> Value
{
  auto make_pattern_from = [](auto && map) {
    std::stringstream ss;
    auto const * separator = "";
    for (auto && [name, value] : map) {
      ss << separator << name;
      separator = "|";
    }
    return "(" + ss.str() + ")";
  };

  static const auto pattern = std::regex(
    R"(^)" + make_pattern_from(Color::table) + R"(?\s*)" + make_pattern_from(Status::table) +
    R"(?\s*)" + make_pattern_from(Shape::table) + R"(?$)");

  if (std::smatch result; std::regex_match(s, result, pattern)) {
    auto color = [](auto && name) { return name.empty() ? Color() : Color(name); };
    auto status = [](auto && name) { return name.empty() ? Status() : Status(name); };
    auto shape = [](auto && name) { return name.empty() ? Shape() : Shape(name); };
    return std::make_tuple(color(result.str(1)), status(result.str(2)), shape(result.str(3)));
  } else {
    throw common::SyntaxError("Invalid traffic light state ", std::quoted(s), " given.");
  }
}

auto operator<<(std::ostream & os, const TrafficLight::Bulb & bulb) -> std::ostream &
{
  return os << std::get<TrafficLight::Color>(bulb.value) << " "
            << std::get<TrafficLight::Status>(bulb.value) << " "
            << std::get<TrafficLight::Shape>(bulb.value);
}

template <typename TrafficLightBulbMessageType>
TrafficLight::Bulb::operator TrafficLightBulbMessageType() const
{
  auto color = [this]() {
    switch (std::get<Color>(value).value) {
      case Color::green:
        return TrafficLightBulbMessageType::GREEN;
      case Color::yellow:
        return TrafficLightBulbMessageType::AMBER;
      case Color::red:
        return TrafficLightBulbMessageType::RED;
      case Color::white:
        return TrafficLightBulbMessageType::WHITE;
      default:
        throw common::SyntaxError(std::get<Color>(value), " is not supported as a color for.");
        // TODO
    }
  };

  auto status = [this]() {
    switch (std::get<Status>(value).value) {
      case Status::solid_on:
        return TrafficLightBulbMessageType::SOLID_ON;
      case Status::solid_off:
        return TrafficLightBulbMessageType::SOLID_OFF;
      case Status::flashing:
        return TrafficLightBulbMessageType::FLASHING;
      case Status::unknown:
        return TrafficLightBulbMessageType::UNKNOWN;
      default:
        throw common::SyntaxError(
          std::get<Status>(value),
          " is not supported as a status for "
          "autoware_auto_perception_msgs::msg::TrafficLight.");
    }
  };

  auto shape = [this]() {
    switch (std::get<Shape>(value).value) {
      case Shape::circle:
        return TrafficLightBulbMessageType::CIRCLE;
      case Shape::cross:
        return TrafficLightBulbMessageType::CROSS;
      case Shape::left:
        return TrafficLightBulbMessageType::LEFT_ARROW;
      case Shape::down:
        return TrafficLightBulbMessageType::DOWN_ARROW;
      case Shape::up:
        return TrafficLightBulbMessageType::UP_ARROW;
      case Shape::right:
        return TrafficLightBulbMessageType::RIGHT_ARROW;
      case Shape::lower_left:
        return TrafficLightBulbMessageType::DOWN_LEFT_ARROW;
      case Shape::lower_right:
        return TrafficLightBulbMessageType::DOWN_RIGHT_ARROW;
      default:
        throw common::SyntaxError(
          std::get<Shape>(value),
          " is not supported as a shape for autoware_auto_perception_msgs::msg::TrafficLight.");
    }
  };

  TrafficLightBulbMessageType traffic_light_bulb;
  traffic_light_bulb.color = color();
  traffic_light_bulb.status = status();
  traffic_light_bulb.shape = shape();
  traffic_light_bulb.confidence = 1.0;
  return traffic_light_bulb;
}

TrafficLight::TrafficLight(const std::int64_t way_id, hdmap_utils::HdMapUtils & map_manager)
: way_id(way_id),
  relation_id(map_manager.getTrafficLightRelationIDFromWayID(way_id)),
  positions{
    std::make_pair(
      Bulb(Color::green, Status::solid_on, Shape::circle).hash(),
      map_manager.getTrafficLightBulbPosition(way_id, "green")),
    std::make_pair(
      Bulb(Color::yellow, Status::solid_on, Shape::circle).hash(),
      map_manager.getTrafficLightBulbPosition(way_id, "yellow")),
    std::make_pair(
      Bulb(Color::red, Status::solid_on, Shape::circle).hash(),
      map_manager.getTrafficLightBulbPosition(way_id, "red")),
  }
{
  if (not map_manager.isTrafficLight(way_id)) {
    throw common::scenario_simulator_exception::Error(
      "Given lanelet ID ", way_id, " is not a traffic light ID.");
  }
}

auto TrafficLight::set(const std::string & states) -> void
{
  auto split = [](auto && given) {
    static const auto pattern = std::regex(R"(^(\w[\w\s]+)(,\s*)?(.*)$)");
    if (std::smatch result; std::regex_match(given, result, pattern)) {
      return std::make_pair(result.str(1), result.str(3));
    } else {
      throw common::SyntaxError("Invalid traffic light state ", std::quoted(given), " given.");
    }
  };

  if (not states.empty()) {
    auto && [head, tail] = split(states);
    emplace(head);
    set(tail);
  }
}

auto operator<<(std::ostream & os, const TrafficLight & traffic_light) -> std::ostream &
{
  std::string separator = "";
  for (auto && bulb : traffic_light.bulbs) {
    os << separator << bulb;
    separator = ", ";
  }
  return os;
}
}  // namespace traffic_simulator
