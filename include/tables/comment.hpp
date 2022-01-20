#pragma once
#include <eosio/eosio.hpp>

namespace hypha {
    using eosio::name;

    struct [[eosio::table("comments"), eosio::contract("comments.hypha")]] Comment {
        uint64_t id;
        uint64_t parent_id;

        name tenant;
        name section;
        name author;
        string content;

        name status;

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

        uint64_t by_parent() const {
            return parent_id;
        }
    };

    using comments_by_section = eosio::indexed_by<
        eosio::name("bykey"),
        eosio::const_mem_fun<Comment, uint128_t, &Comment::by_key>
    >;

    using comments_by_parent = eosio::indexed_by<
        eosio::name("byparent"),
        eosio::const_mem_fun<Comment, uint64_t, &Comment::by_parent>
    >;

    using Comments = eosio::multi_index<
        "comments"_n,
        Comment,
        comments_by_section,
        comments_by_parent
    >;
}
