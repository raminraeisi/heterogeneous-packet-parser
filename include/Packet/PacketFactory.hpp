/**
 * @file include/Packet/PacketFactory.hpp
 *
 * @author Ramin Raeisi <rr@epen.io>
 */
#ifndef PACKETFACTORY_HPP
#define PACKETFACTORY_HPP

#include <Factory/Factory.hpp>
#include <Packet/PacketBase.hpp>

namespace cdp {
    namespace factory {
/**
* @brief: The Packet Factory Class which is a specialized version of general 
* factory class, passing PacketInterface as the parent of all automatically 
* registered packet classes. 
* It is a singleton class
*/
class PacketFactory : public cdp::factory::Factory<cdp::packet::PacketBase> {
public:
    /// Deleted copy constructor
    PacketFactory(const PacketFactory&) = delete;
    /// Deleted move constructor
    PacketFactory(const PacketFactory&&) = delete;
    /// Deleted copy assignment
    PacketFactory& operator=(const PacketFactory&) = delete;
    /// Deleted move assignment
    PacketFactory& operator=(PacketFactory&&) = delete;

    /// Returns the singleton reference
    static PacketFactory& getInstance() {
      static PacketFactory packetFactory;
      return(packetFactory);
  }
private:
    /// Default constructor as private
    PacketFactory() = default;
    /// Default destructor as private
    ~PacketFactory() = default;
};
    } // namespace factory
} // namespace cdp

#endif // !PACKETFACTORY_HPP

