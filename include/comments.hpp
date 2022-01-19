#pragma once

#include <eosio/eosio.hpp>

#include <vector>
#include <map>
#include <string>

namespace hypha {

    using std::uint64_t;
    using std::string;
    using std::map;
    using std::vector;
    using eosio::name;

    class [[eosio::contract("comments.hypha")]] comments : public eosio::contract {

        public:
            using contract::contract;

            [[eosio::action]]
            void addsection(
                const name& scope,
                const name& tenant,
                const name& section,
                const name& author
            );

            [[eosio::action]]
            void delsection(
                const name& scope,
                const name& tenant,
                const name& section
            );

            [[eosio::action]]
            void likesec(
                const name& scope,
                const name& tenant,
                const name& section,
                const name& user
            );

            [[eosio::action]]
            void unlikesec(
                const name& scope,
                const name& tenant,
                const name& section,
                const name& user
            );

            [[eosio::action]]
            void addcomment(
                const name& scope,
                const name& tenant,
                const name& section,
                const name& author,
                const string& content,
                const std::optional<std::uint64_t>& parent_id
            );

            [[eosio::action]]
            void editcomment(
                const name& scope,
                const name& tenant,
                const name& author,
                const uint64_t& comment_id,
                const string& content
            );

            [[eosio::action]]
            void delcomment(
                const name& scope,
                const name& tenant,
                const name& author,
                const uint64_t& comment_id
            );

    };
}
