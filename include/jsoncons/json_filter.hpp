// Copyright 2013 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_JSON_FILTER_HPP
#define JSONCONS_JSON_FILTER_HPP

#include <string>

#include <jsoncons/json_visitor.hpp>

namespace jsoncons {

template <class CharT>
class basic_json_filter : public basic_json_visitor<CharT>
{
public:
    using typename basic_json_visitor<CharT>::char_type;
    using typename basic_json_visitor<CharT>::string_view_type;
private:
    basic_json_visitor<char_type>* destination_;

    // noncopyable 
    basic_json_filter(const basic_json_filter&) = delete;
    basic_json_filter& operator=(const basic_json_filter&) = delete;
public:
    basic_json_filter(basic_json_visitor<char_type>& visitor)
        : destination_(std::addressof(visitor))
    {
    }

    // moveable
    basic_json_filter(basic_json_filter&&) = default;
    basic_json_filter& operator=(basic_json_filter&&) = default;

    basic_json_visitor<char_type>& destination()
    {
        return *destination_;
    }

#if !defined(JSONCONS_NO_DEPRECATED)

    JSONCONS_DEPRECATED_MSG("Instead, use destination()")
    basic_json_visitor<char_type>& to_handler()
    {
        return destination_;
    }
    JSONCONS_DEPRECATED_MSG("Instead, use destination") 
    basic_json_visitor<char_type>& input_handler()
    {
        return destination_;
    }

    JSONCONS_DEPRECATED_MSG("Instead, use destination") 
    basic_json_visitor<char_type>& downstream_handler()
    {
        return destination_;
    }

    JSONCONS_DEPRECATED_MSG("Instead, use destination") 
    basic_json_visitor<char_type>& destination_handler()
    {
        return destination_;
    }
#endif

private:
    void visit_flush() override
    {
        destination_->flush();
    }

    bool visit_begin_object(semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->begin_object(tag, context, ec);
    }

    bool visit_begin_object(std::size_t length, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->begin_object(length, tag, context, ec);
    }

    bool visit_end_object(const ser_context& context, std::error_code& ec) override
    {
        return destination_->end_object(context, ec);
    }

    bool visit_begin_array(semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->begin_array(tag, context, ec);
    }

    bool visit_begin_array(std::size_t length, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->begin_array(length, tag, context, ec);
    }

    bool visit_end_array(const ser_context& context, std::error_code& ec) override
    {
        return destination_->end_array(context, ec);
    }

    bool visit_key(const string_view_type& name,
                 const ser_context& context,
                 std::error_code& ec) override
    {
        return destination_->key(name, context, ec);
    }

    bool visit_string(const string_view_type& value,
                      semantic_tag tag,
                      const ser_context& context,
                      std::error_code& ec) override
    {
        return destination_->string_value(value, tag, context, ec);
    }

    bool visit_byte_string(const byte_string_view& b, 
                           semantic_tag tag,
                           const ser_context& context,
                           std::error_code& ec) override
    {
        return destination_->byte_string_value(b, tag, context, ec);
    }

    bool visit_byte_string(const byte_string_view& b, 
                           uint64_t ext_tag,
                           const ser_context& context,
                           std::error_code& ec) override
    {
        return destination_->byte_string_value(b, ext_tag, context, ec);
    }

    bool visit_uint64(uint64_t value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->uint64_value(value, tag, context, ec);
    }

    bool visit_int64(int64_t value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->int64_value(value, tag, context, ec);
    }

    bool visit_half(uint16_t value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->half_value(value, tag, context, ec);
    }

    bool visit_double(double value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->double_value(value, tag, context, ec);
    }

    bool visit_bool(bool value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->bool_value(value, tag, context, ec);
    }

    bool visit_null(semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->null_value(tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const uint8_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const uint16_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const uint32_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const uint64_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const int8_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const int16_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const int32_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const int64_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(half_arg_t, 
                        const jsoncons::span<const uint16_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(half_arg, s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const float>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const double>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_begin_multi_dim(const jsoncons::span<const size_t>& shape,
                            semantic_tag tag,
                            const ser_context& context, 
                            std::error_code& ec) override
    {
        return destination_->begin_multi_dim(shape, tag, context, ec);
    }

    bool visit_end_multi_dim(const ser_context& context,
                          std::error_code& ec) override
    {
        return destination_->end_multi_dim(context, ec);
    }
};

template <class CharT>
class basic_rename_object_key_filter : public basic_json_filter<CharT>
{
public:
    using typename basic_json_filter<CharT>::string_view_type;

private:
    std::basic_string<CharT> name_;
    std::basic_string<CharT> new_name_;
public:
    basic_rename_object_key_filter(const std::basic_string<CharT>& name,
                             const std::basic_string<CharT>& new_name,
                             basic_json_visitor<CharT>& visitor)
        : basic_json_filter<CharT>(visitor), 
          name_(name), new_name_(new_name)
    {
    }

private:
    bool visit_key(const string_view_type& name,
                 const ser_context& context,
                 std::error_code& ec) override
    {
        if (name == name_)
        {
            return this->destination().key(new_name_,context, ec);
        }
        else
        {
            return this->destination().key(name,context,ec);
        }
    }
};

template <class CharT,class OtherCharT>
class json_visitor_adaptor_base : public basic_json_visitor<CharT>
{
public:
    using supertype = basic_json_visitor<CharT>;
    using destination_type = basic_json_visitor<OtherCharT>;

    using typename supertype::string_view_type;
private:
    destination_type* destination_;

    // noncopyable
    json_visitor_adaptor_base(const json_visitor_adaptor_base&) = delete;
    json_visitor_adaptor_base& operator=(const json_visitor_adaptor_base&) = delete;
public:
    json_visitor_adaptor_base(basic_json_visitor<OtherCharT>& visitor)
        : destination_(std::addressof(visitor))
    {
    }

    // moveable
    json_visitor_adaptor_base(json_visitor_adaptor_base&&) = default;
    json_visitor_adaptor_base& operator=(json_visitor_adaptor_base&&) = default;

    destination_type& destination()
    {
        return *destination_;
    }

private:
    void visit_flush() override
    {
        destination_->flush();
    }

    bool visit_begin_object(semantic_tag tag, 
                         const ser_context& context,
                         std::error_code& ec) override
    {
        return destination_->begin_object(tag, context, ec);
    }

    bool visit_begin_object(std::size_t length, 
                         semantic_tag tag, 
                         const ser_context& context,
                         std::error_code& ec) override
    {
        return destination_->begin_object(length, tag, context, ec);
    }

    bool visit_end_object(const ser_context& context, std::error_code& ec) override
    {
        return destination_->end_object(context, ec);
    }

    bool visit_begin_array(semantic_tag tag, 
                        const ser_context& context,
                        std::error_code& ec) override
    {
        return destination_->begin_array(tag, context, ec);
    }

    bool visit_begin_array(std::size_t length, 
                        semantic_tag tag, 
                        const ser_context& context,
                        std::error_code& ec) override
    {
        return destination_->begin_array(length, tag, context, ec);
    }

    bool visit_end_array(const ser_context& context, std::error_code& ec) override
    {
        return destination_->end_array(context, ec);
    }

    bool visit_byte_string(const byte_string_view& b, 
                           semantic_tag tag,
                           const ser_context& context,
                           std::error_code& ec) override
    {
        return destination_->byte_string_value(b, tag, context, ec);
    }

    bool visit_byte_string(const byte_string_view& b, 
                           uint64_t ext_tag,
                           const ser_context& context,
                           std::error_code& ec) override
    {
        return destination_->byte_string_value(b, ext_tag, context, ec);
    }

    bool visit_half(uint16_t value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->half_value(value, tag, context, ec);
    }

    bool visit_double(double value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->double_value(value, tag, context, ec);
    }

    bool visit_int64(int64_t value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->int64_value(value, tag, context, ec);
    }

    bool visit_uint64(uint64_t value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->uint64_value(value, tag, context, ec);
    }

    bool visit_bool(bool value, semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->bool_value(value, tag, context, ec);
    }

    bool visit_null(semantic_tag tag, const ser_context& context, std::error_code& ec) override
    {
        return destination_->null_value(tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const uint8_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const uint16_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const uint32_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const uint64_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const int8_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const int16_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const int32_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const int64_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(half_arg_t, 
                        const jsoncons::span<const uint16_t>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(half_arg, s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const float>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_typed_array(const jsoncons::span<const double>& s, 
                        semantic_tag tag,
                        const ser_context& context, 
                        std::error_code& ec) override
    {
        return destination_->typed_array(s, tag, context, ec);
    }

    bool visit_begin_multi_dim(const jsoncons::span<const size_t>& shape,
                            semantic_tag tag,
                            const ser_context& context, 
                            std::error_code& ec) override
    {
        return destination_->begin_multi_dim(shape, tag, context, ec);
    }

    bool visit_end_multi_dim(const ser_context& context,
                          std::error_code& ec) override
    {
        return destination_->end_multi_dim(context, ec);
    }

};

template <class CharT,class OtherCharT,class Enable=void>
class json_visitor_adaptor 
{
};

template <class CharT,class OtherCharT>
class json_visitor_adaptor<CharT,OtherCharT,typename std::enable_if<jsoncons::detail::is_narrow_character<CharT>::value &&
                                                                    jsoncons::detail::is_narrow_character<OtherCharT>::value>::type> : public json_visitor_adaptor_base<CharT,OtherCharT>
{
    using supertype = json_visitor_adaptor_base<CharT,OtherCharT>;
    using to_char_type = OtherCharT;
    using from_char_type = CharT;
public:
    using typename supertype::string_view_type;
    using supertype::destination;
    using destination_type = supertype::destination_type;
private:

    // noncopyable
    json_visitor_adaptor(const json_visitor_adaptor&) = delete;
    json_visitor_adaptor& operator=(const json_visitor_adaptor&) = delete;
public:
    json_visitor_adaptor(destination_type& visitor)
        : supertype(visitor)
    {
    }

    // moveable
    json_visitor_adaptor(json_visitor_adaptor&&) = default;
    json_visitor_adaptor& operator=(json_visitor_adaptor&&) = default;

private:

    bool visit_key(const string_view_type& key,
                 const ser_context& context,
                 std::error_code& ec) override
    {
        return destination().key(string_view_type(reinterpret_cast<const to_char_type*>(key.data()),key.size()), context, ec);
    }

    bool visit_string(const string_view_type& value,
                      semantic_tag tag,
                      const ser_context& context,
                      std::error_code& ec) override
    {
        return destination().string_value(string_view_type(reinterpret_cast<const to_char_type*>(value.data()),value.size()), tag, context, ec);
    }
};

template <class CharT,class OtherCharT>
class json_visitor_adaptor<CharT,OtherCharT,typename std::enable_if<!(jsoncons::detail::is_narrow_character<CharT>::value &&
                                                                      jsoncons::detail::is_narrow_character<OtherCharT>::value)>::type> : public json_visitor_adaptor_base<CharT,OtherCharT>
{
    using supertype = json_visitor_adaptor_base<CharT,OtherCharT>;

    std::basic_string<OtherCharT> buffer_;
public:
    using typename supertype::string_view_type;
    using supertype::destination;
    using destination_type = supertype::destination_type;
private:

    // noncopyable
    json_visitor_adaptor(const json_visitor_adaptor&) = delete;
    json_visitor_adaptor& operator=(const json_visitor_adaptor&) = delete;
public:
    json_visitor_adaptor(destination_type& visitor)
        : supertype(visitor)
    {
    }

    // moveable
    json_visitor_adaptor(json_visitor_adaptor&&) = default;
    json_visitor_adaptor& operator=(json_visitor_adaptor&&) = default;

private:

    bool visit_key(const string_view_type& name,
                   const ser_context& context,
                   std::error_code& ec) override
    {
        buffer_.clear();
        auto result = unicons::convert(name.begin(),name.end(),std::back_inserter(buffer_),unicons::conv_flags::strict);
        if (result.ec != unicons::conv_errc())
        {
            ec = result.ec;
        }
        return destination().key(buffer_, context, ec);
    }

    bool visit_string(const string_view_type& value,
                      semantic_tag tag,
                      const ser_context& context,
                      std::error_code& ec) override
    {
        buffer_.clear();
        auto result = unicons::convert(value.begin(),value.end(),std::back_inserter(buffer_),unicons::conv_flags::strict);
        if (result.ec != unicons::conv_errc())
        {
            JSONCONS_THROW(ser_error(result.ec));
        }
        return destination().string_value(buffer_, tag, context, ec);
    }
};

template <class CharT,class OtherCharT>
json_visitor_adaptor<CharT,OtherCharT> make_json_visitor_adaptor(basic_json_visitor<OtherCharT>& to)
{
    return json_visitor_adaptor<CharT, OtherCharT>(to);
}

using json_filter = basic_json_filter<char>;
using wjson_filter = basic_json_filter<wchar_t>;
using rename_object_key_filter = basic_rename_object_key_filter<char>;
using wrename_object_key_filter = basic_rename_object_key_filter<wchar_t>;

#if !defined(JSONCONS_NO_DEPRECATED)
template <class CharT>
using basic_json_content_filter = basic_json_filter<CharT>;
JSONCONS_DEPRECATED_MSG("Instead, use json_filter") typedef basic_json_filter<char> json_content_filter;
JSONCONS_DEPRECATED_MSG("Instead, use wjson_filter") typedef basic_json_filter<wchar_t> wjson_content_filter;
JSONCONS_DEPRECATED_MSG("Instead, use rename_object_key_filter") typedef basic_rename_object_key_filter<char> rename_name_filter;
JSONCONS_DEPRECATED_MSG("Instead, use wrename_object_key_filter") typedef basic_rename_object_key_filter<wchar_t> wrename_name_filter;
JSONCONS_DEPRECATED_MSG("Instead, use rename_object_key_filter") typedef basic_rename_object_key_filter<char> rename_object_member_filter;
JSONCONS_DEPRECATED_MSG("Instead, use wrename_object_key_filter") typedef basic_rename_object_key_filter<wchar_t> wrename_object_member_filter;
#endif

}

#endif
