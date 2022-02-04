#include <comments.hpp>
#include <tables/comment.hpp>
#include <tables/section.hpp>
#include <eosio/system.hpp>

namespace hypha {
    const uint64_t NULL_PARENT = 0;

    namespace Status {
        constexpr name Active = name("active");
        constexpr name Deleted = name("deleted");
    }

    void comments::addsection(
        const name& scope,
        const name& tenant,
        const name& section,
        const name& author
    ) {
        require_auth(scope);
        Sections section_table(get_self(), scope.value);

        auto section_index = section_table.get_index<"bykey"_n>();

        uint32_t now = eosio::current_time_point().sec_since_epoch();
        eosio::check(section_index.find(Section::build_key(tenant, section)) == section_index.end(), "Section already exists" + section.to_string());
        section_table.emplace(scope, [&](auto &s) {
            s.id = section_table.available_primary_key();
            s.section = section;
            s.author = author;
            s.tenant = tenant;
            s.comments = 0;
            s.created = now;
            s.updated = now;
        });
    }

    void comments::delsection(const name& scope, const name& tenant, const name& section) {
        require_auth(scope);
        Sections section_table(get_self(), scope.value);
        auto section_index = section_table.get_index<"bykey"_n>();
        auto sectionItr = section_index.require_find(Section::build_key(tenant, section), "Section does not exist");

        Comments comment_table(get_self(), scope.value);
        auto comment_index = comment_table.get_index<"bykey"_n>();

        auto commentsItr = comment_index.find(Comment::build_key(tenant, section));
        for (;commentsItr != comment_index.end(); ++commentsItr) {
            comment_index.erase(commentsItr);
        }

        section_index.erase(sectionItr);
    }

    void comments::likesec(const name& scope, const name& tenant, const name& section, const name& user) {
        require_auth(user);
        Sections section_table(get_self(), scope.value);
        auto section_index = section_table.get_index<"bykey"_n>();
        auto sectionItr = section_index.require_find(Section::build_key(tenant, section), "Section does not exist");

        auto it = std::find(sectionItr->likes.begin(), sectionItr->likes.end(), user);
        eosio::check(it == sectionItr->likes.end(), "Already liked the section");

        section_index.modify(sectionItr, scope, [&](auto &s) {
            s.likes.push_back(user);
        });
    }

    void comments::unlikesec(const name& scope, const name& tenant, const name& section, const name& user) {
        require_auth(user);
        Sections section_table(get_self(), scope.value);
        auto section_index = section_table.get_index<"bykey"_n>();
        auto sectionItr = section_index.require_find(Section::build_key(tenant, section), "Section does not exist");

        auto it = std::find(sectionItr->likes.begin(), sectionItr->likes.end(), user);
        eosio::check(it != sectionItr->likes.end(), "User has not liked the section");

        section_index.modify(sectionItr, scope, [&](auto &s) {
            s.likes.erase(
                std::find(s.likes.begin(), s.likes.end(), user)
            );
        });
    }

    void comments::addcomment(
        const name& scope,
        const name& tenant,
        const name& section,
        const name& author,
        const string& content,
        const std::optional<std::uint64_t>& parent_id
    ) {
        require_auth(author);

        Sections section_table(get_self(), scope.value);
        auto section_index = section_table.get_index<"bykey"_n>();
        auto sectionItr = section_index.require_find(Section::build_key(tenant, section), "Section does not exist");

        Comments comments(get_self(), scope.value);

        uint64_t parent = parent_id.value_or(NULL_PARENT);
        if (parent != NULL_PARENT) {
            auto parentItr = comments.require_find(parent);
            eosio::check(parentItr->section == section, "Only allowed to post a child comment in the same section");
            eosio::check(parentItr->tenant == tenant, "Only allowed to post a child comment in the same tenant");
        }

        uint32_t now = eosio::current_time_point().sec_since_epoch();
        comments.emplace(scope, [&](auto &c) {
            c.id = comments.available_primary_key();
            if (c.id == NULL_PARENT) {
                c.id++;
            }
            c.tenant = tenant;
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
        const name& tenant,
        const name& author,
        const uint64_t& comment_id,
        const string& content
    ) {
        require_auth(author);
        Comments comments(get_self(), scope.value);
        auto commentItr = comments.require_find(comment_id);

        eosio::check(commentItr->author == author, "Only the author is allowed to edit its comment");
        eosio::check(commentItr->tenant == tenant, "Only allowed to edit comments in the same tenant");
        eosio::check(commentItr->status == Status::Active, "Only active comments are editable");
        uint32_t now = eosio::current_time_point().sec_since_epoch();

        comments.modify(commentItr, scope, [&](auto &c) {
            c.content = content;
            c.updated = now;
        });
    }

    void comments::delcomment(
        const name& scope,
        const name& tenant,
        const name& author,
        const uint64_t& comment_id
    ) {
        require_auth(author);
        Comments comments(get_self(), scope.value);
        auto commentItr = comments.require_find(comment_id);

        eosio::check(commentItr->author == author, "Only the author is allowed to delete its own comment");
        eosio::check(commentItr->tenant == tenant, "Only allowed to edit comments in the same tenant");
        eosio::check(commentItr->status == Status::Active, "Only active comments are deletable");
        uint32_t now = eosio::current_time_point().sec_since_epoch();

        comments.modify(commentItr, scope, [&](auto &c) {
            c.content = "";
            c.status = Status::Deleted;
            c.updated = now;
        });
    }

}
