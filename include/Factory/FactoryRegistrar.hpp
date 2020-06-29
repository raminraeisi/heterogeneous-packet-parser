/**
 * @file include/Factory/FactoryRegistrar.hpp
 *
 * @author Ramin Raeisi <rr@epen.io>
 */

#ifndef FACTORYREGISTRAR_HPP
#define FACTORYREGISTRAR_HPP

#include <utility>
#include <Globals.hpp>

namespace cdp {
    namespace factory {
/**
  * @brief: Factory Registrar Class
  * @tparam Interface the pure abstract class which is the base class of all registered and derived classes in the factory
  * @tparam Derived the class which is going to be registered
  * @tparam Factory the factory class in which the derived class would be registered
  * @tparam ID the desired id by which the derived class would be registered in the factory
  */
//template<class Interface, class Derived, class Factory, cdp::PacketIDType ID>
template<class Interface, class Derived, class Factory, cdp::globals::PacketIDType ID>
class FactoryRegistrar {
public:
    /// A static member which its initilization will do the trick
    /// of initialization of the derived class prior to the main function
    static bool _registered;

    /// A static method which is responsible for the creation of an 
    /// instance of the derived class using . it will be called by the 
    /// facgory class in case of need 
    static Interface* CreateInstance() {
      return new Derived();
    }
};

/// _registered member initialization which will eventually do the trick of
/// derived class initilization
template<class Base, class Derived, class Factory, cdp::globals::PacketIDType ID>
bool FactoryRegistrar<Base, Derived, Factory, ID>::_registered =
Factory::Register(ID, FactoryRegistrar::CreateInstance);

    } // namespace factory
} // namespace cdp

#endif // !FACTORYREGISTRAR_HPP
