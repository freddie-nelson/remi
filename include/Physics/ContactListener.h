#pragma once

#include "../World/World.h"
#include "ContactInfo.h"

#include <box2d/b2_world.h>
#include <unordered_map>

namespace Physics {
    class ContactListener : public b2ContactListener {
        public:
            /**
             * Create a new contact listener
            */
            ContactListener();

            /**
             * Set the world to use for the contact listener.
             *
             * @param world The world to use.
            */
            void setWorld(World::World *world);

            /**
             * Called when two fixtures begin to touch.
             *
             * @param contact The contact between the two fixtures.
            */
            void BeginContact(b2Contact* contact) override;

            /**
             * Called when two fixtures cease to touch.
             * 
             * @param contact The contact between the two fixtures.
            */
            void EndContact(b2Contact* contact) override;
        
        private:
            World::World *world = nullptr;

            struct ContactData {
                ContactInfo contactInfo;
                bool ignore;
            };

            /**
             * Helper function to get the entities from a contact.
             *
             * @param contact The contact to get the entities from.
             *
             * @returns The contact info and an ignore flag.
            */
            ContactData contactHelper(b2Contact *contact);
    };
}