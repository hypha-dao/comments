#pragma once
#include <eosio/eosio.hpp>

namespace hypha {
    using eosio::name;
    struct [[eosio::table("sections"), eosio::contract("comments.hypha")]] Section {
        uint64_t id;

        name tenant;
        name section;

        name author;
        uint64_t comments;
        vector<name> likes;
        map<name, bool> config; // future, allow to enable/disable

        uint32_t created;
        uint32_t updated;

        static uint128_t build_key(const name& tenant, const name& section) {
            return ((uint128_t)tenant.value << 64) | section.value;
        }

        uint64_t primary_key() const {
            return id;
        }

        uint128_t by_key() const {
            return build_key(tenant, section);
        }
    };

    using by_key = eosio::indexed_by<
        eosio::name("bykey"),
        eosio::const_mem_fun<Section, uint128_t, &Section::by_key>
    >;

    using Sections = eosio::multi_index<name("sections"), Section, by_key>;
}
