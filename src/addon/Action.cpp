//
// Created by Seres67 on 22/05/2024.
//


#include "addon/Action.hpp"
#include "addon/Wheel.hpp"


namespace nlohmann {
    template<>
    struct adl_serializer<std::variant<Key, std::string>> {

        static void to_json(json &obj, const std::variant<Key, std::string> &foo) {
            if (std::holds_alternative<Key>(foo))
                Key::to_json(obj, std::get<Key>(foo));
            else if (std::holds_alternative<std::string>(foo))
                nlohmann::to_json(obj, std::get<std::string>(foo));
            else
                throw std::runtime_error("non exhaustive pattern match in void "
                                         "to_json(json &obj, const FoobarVariant &foo)");
        }

        static void from_json(const json &obj, std::variant<Key, std::string> &foo) {
            auto t = obj["type"].get<bool>();
            if (t)
                Key::from_json(obj, std::get<Key>(foo));
            else
                nlohmann::from_json(obj, std::get<std::string>(foo));

        }
    };

    template<>
    struct adl_serializer<Action> {
        static void to_json(json &obj, const Action &action) {
            obj["type"] = action.m_type;
            obj["name"] = action.m_action_name;
            if (std::holds_alternative<Key>(action.m_action)) {
                json nested;
                Key::to_json(nested, std::get<Key>(action.m_action));
                obj["action"] = nested;
            } else if (std::holds_alternative<std::string>(action.m_action)) {
                json nested;
                nlohmann::to_json(nested, std::get<std::string>(action.m_action));
                obj["action"] = nested;
            } else
                throw std::runtime_error("non exhaustive pattern match in void "
                                         "to_json(json &obj, const FoobarVariant &foo)");
        }

        static void from_json(const json &obj, Action &action) {
            action.m_type = obj["type"];
            action.m_action_name = obj["name"];
            if (action.m_type)
                Key::from_json(obj, std::get<Key>(action.m_action));
            else
                nlohmann::from_json(obj, std::get<std::string>(action.m_action));
        }
    };

    template<>
    struct adl_serializer<Wheel> {
        static void to_json(json &obj, const Wheel &wheel) {
            obj["name"] = wheel.get_wheel_name();
            {
                json nested;
                Key::to_json(nested, wheel.get_key());
                obj["key"] = nested;
            }
            {
                json out;
                for (const auto &action: wheel.get_elements()) {
                    json nested;
                    adl_serializer<Action>::to_json(nested, action);
                    out.push_back(nested);
                }
                obj["elements"] = out;
            }
        }

        static void from_json(const json &obj, Wheel &wheel) {
            obj["name"].get_to(wheel.m_name);
            Key::from_json(obj["key"], wheel.m_key);
            for (auto &element: obj["elements"]) {
                Action tmp{nullptr, 0};
                adl_serializer<Action>::from_json(element, tmp);
                wheel.m_elements.emplace_back(tmp);
            }
        }
    };
}