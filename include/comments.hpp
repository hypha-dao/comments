#pragma once

#include <eosio/eosio.hpp>

#include <string>

namespace hypha {

    using std::uint64_t;
    using std::string;
    using eosio::name;

    class [[eosio::contract("comments.hypha")]] comments : public eosio::contract {

        public:
            using contract::contract;

            [[eosio::action]]
            void addsection(
                const name& scope,
                const name& section,
                const name& author
            );

            [[eosio::action]]
            void delsection(
                const name& scope,
                const name& section
            );

            [[eosio::action]]
            void addcomment(
                const name& scope,
                const name& section,
                const name& author, 
                const string& content, 
                const std::optional<std::uint64_t>& parent_id
            );

            [[eosio::action]]
            void editcomment(
                const name& scope,
                const name& author,
                const uint64_t& comment_id,
                const string& content
            );

            [[eosio::action]]
            void delcomment(
                const name& scope,
                const name& author,
                const uint64_t& comment_id
            );

        private:
            struct [[eosio::table]] Section {
                name section;

                name author;
                uint64_t comments;

                uint32_t created;
                uint32_t updated;

                uint64_t primary_key() const {
                    return section.value;
                }
            };

            struct [[eosio::table]] Comment {
                uint64_t id;
                uint64_t parent_id;

                name section;
                name author;
                string content;

                name status;

                uint32_t created;
                uint32_t updated;

                uint64_t primary_key() const {
                    return id;
                }

                uint64_t by_section() const {
                    return section.value;
                }

                uint64_t by_parent() const {
                    return parent_id;
                }
            };

            typedef eosio::multi_index< 
                "sections"_n, Section
            > Sections;

            typedef eosio::multi_index< 
                "comments"_n, Comment,
                eosio::indexed_by<eosio::name("bysection"), eosio::const_mem_fun<Comment, uint64_t, &Comment::by_section>>,
                eosio::indexed_by<eosio::name("byparent"), eosio::const_mem_fun<Comment, uint64_t, &Comment::by_parent>>
            > Comments;

    };
}