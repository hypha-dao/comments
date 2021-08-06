#include <comments.hpp>
#include <eosio/system.hpp>

namespace hypha {
    const uint64_t NULL_PARENT = std::numeric_limits<uint64_t>::max();

    namespace Status {
        constexpr name Active = name("active");
        constexpr name Deleted = name("deleted");
    }

    void comments::addsection(
        const name& scope,
        const name& section,
        const name& author
    ) {
        require_auth(scope);
        Sections section_index(get_self(), scope.value);

        uint32_t now = eosio::current_time_point().sec_since_epoch();
        eosio::check(section_index.find(section.value) == section_index.end(), "Section already exists");
        section_index.emplace(scope, [&](auto &s) {
            s.section = section;
            s.author = author;
            s.comments = 0;
            s.created = now;
            s.updated = now;
        });
    }

    void comments::delsection(const name& scope, const name& section) {
        require_auth(scope);
        Sections section_index(get_self(), scope.value);
        auto sectionItr = section_index.require_find(section.value, "Section does not exist");
        
        Comments comment_index(get_self(), scope.value);
        auto bySection = comment_index.get_index<"bysection"_n>();
        auto commentsItr = bySection.find(section.value);
        for (;commentsItr != bySection.end(); ++commentsItr) {
            bySection.erase(commentsItr);
        }

        section_index.erase(sectionItr);
    }

    void comments::addcomment(
        const name& scope,
        const name& section,
        const name& author,
        const string& content, 
        const std::optional<std::uint64_t>& parent_id
    ) {
        require_auth(scope);

        Sections section_index(get_self(), scope.value);
        auto sectionItr = section_index.require_find(section.value, "Section does not exist");

        Comments comments(get_self(), scope.value);

        uint64_t parent = parent_id.value_or(NULL_PARENT);
        if (parent != NULL_PARENT) {
            auto parentItr = comments.require_find(parent);
            eosio::check(parentItr->section == section, "Only allowed to post a child comment in the same section");
        }
        
        uint32_t now = eosio::current_time_point().sec_since_epoch();
        comments.emplace(scope, [&](auto &c) {
            c.id = comments.available_primary_key();
            c.parent_id = parent;
            c.section = section;
            c.author = author;
            c.content = content;
            c.status = Status::Active;
            c.created = now;
            c.updated = now;
        });

    }

    void comments::editcomment(
        const name& scope,
        const name& author,
        const uint64_t& comment_id,
        const string& content
    ) {
        require_auth(scope);
        Comments comments(get_self(), scope.value);
        auto commentItr = comments.require_find(comment_id);

        eosio::check(commentItr->author == author, "Only the author is allowed to edit its comment");
        eosio::check(commentItr->status == Status::Active, "Only active comments are editable");
        uint32_t now = eosio::current_time_point().sec_since_epoch();

        comments.modify(commentItr, scope, [&](auto &c) {
            c.content = content;
            c.updated = now;
        });
    }

    void comments::delcomment(
        const name& scope,
        const name& author,
        const uint64_t& comment_id
    ) {
        require_auth(scope);
        Comments comments(get_self(), scope.value);
        auto commentItr = comments.require_find(comment_id);

        eosio::check(commentItr->author == author, "Only the author is allowed to delete its own comment");
        eosio::check(commentItr->status == Status::Active, "Only active comments are deletable");
        uint32_t now = eosio::current_time_point().sec_since_epoch();

        comments.modify(commentItr, scope, [&](auto &c) {
            c.content = "";
            c.status = Status::Deleted;
            c.updated = now;
        });
    }

}