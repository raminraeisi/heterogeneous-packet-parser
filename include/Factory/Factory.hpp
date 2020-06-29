/**
 * @file include/Factory/Factory.hpp
 *
 * @author Ramin Raeisi <rr@epen.io>
 */
#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <unordered_map>
#include <optional>
#include <Globals.hpp>

namespace cdp {
    namespace factory {
        /**
        * @brief: A Generic Factory Class
        * @tparam CommonInterface refers to the common abstract class from which
        * to be registered classses have been inherited from
        */
        template<class CommonInterface>
        class Factory {
        public:
            /// Makes the factory register method accessible by any FactoryRegistrar class 
            template<class Interface, class Derived, class Factory, cdp::globals::PacketIDType ID>
            friend class FactoryRegistrar;

            /// Type definition for the CreateInstance method provided by any to be registered class
            using CreateInstanceCallable = CommonInterface * (*)();
            /// Type definition for the map of registered classes in factory
            using RegisteredClassesMap = std::unordered_map<uint8_t, CreateInstanceCallable>;

            /// Type definition for the GetInstance method provided by any to be registered class
            using InstancePtr = std::shared_ptr<CommonInterface>;
            /// Type definition for the map of currently created instances in factory
            using InstancesMap = std::unordered_map<uint8_t, InstancePtr>;

        public:
            /// Returns the reference of map of instantiated classes in factory
            static InstancesMap& Instances() {
                static InstancesMap instances{};
                return instances;
            }

            /// Returns the reference of map of registered classes in factory
            static RegisteredClassesMap& RegisteredClasses() {
                static RegisteredClassesMap registeredClasses{};
                return registeredClasses;
            }

            /// Possibly creates and returns an instance of a registered class in factory
            static InstancePtr GetObject(cdp::globals::PacketIDType classID) {
                auto it = Instances().find(classID);
                if (it != Instances().end()) {
                    return(it->second);
                }
                else {
                    auto NullPtr = std::shared_ptr<CommonInterface>{};
                    return(NullPtr);
                }
            }

            /// Registers a class in factory by an id and its CreateInstance method pointer
            static bool Register(uint8_t id, CreateInstanceCallable createInstanceFunction) {
                auto it = RegisteredClasses().find(id);
                if (it == RegisteredClasses().end()) {
                    RegisteredClasses()[id] = createInstanceFunction;
                    Instances().emplace(id, std::shared_ptr<CommonInterface>(createInstanceFunction()));
                    return true;
                }
                return false;
            }
        };
    } // namespace factory
} // namespace cdp
#endif // !FACTORY_HPP
