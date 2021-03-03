// Copyright 2015 Daniel Parker 
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_JSON_READER_HPP
#define JSONCONS_JSON_READER_HPP

#include <memory> // std::allocator
#include <string>
#include <vector>
#include <stdexcept>
#include <system_error>
#include <ios>
#include <cstring>
#include <utility> // std::move
#include <jsoncons/source.hpp>
#include <jsoncons/json_exception.hpp>
#include <jsoncons/json_visitor.hpp>
#include <jsoncons/json_filter.hpp>
#include <jsoncons/json_parser.hpp>

namespace jsoncons {

enum class encoding_kind 
{
    undetected,
    utf8,
    utf16le,
    utf16be,
    utf32le,
    utf32be
};

struct detect_encoding_result
{
    encoding_kind encoding;
    std::size_t offset;
};

template<class CharT,class Src=jsoncons::stream_source<CharT>,class Allocator=std::allocator<char>>
class basic_json_reader 
{
public:
    using char_type = CharT;
    using source_type = Src;
    using string_view_type = jsoncons::basic_string_view<CharT>;
    using temp_allocator_type = Allocator;
private:
    typedef typename std::allocator_traits<temp_allocator_type>:: template rebind_alloc<CharT> char_allocator_type;

    static constexpr size_t default_max_buffer_length = 16384;

    basic_default_json_visitor<CharT> default_visitor_;

    basic_json_visitor<CharT>& visitor_;

    basic_json_parser<CharT,Allocator> parser_;

    source_type source_;
    bool eof_;
    bool begin_;
    std::size_t buffer_length_;
    std::size_t raw_buffer_length_;
    std::vector<CharT,char_allocator_type> buffer_;

    // Noncopyable and nonmoveable
    basic_json_reader(const basic_json_reader&) = delete;
    basic_json_reader& operator=(const basic_json_reader&) = delete;

public:
    template <class Source>
    explicit basic_json_reader(Source&& source, const Allocator& alloc = Allocator())
        : basic_json_reader(std::forward<Source>(source),
                            default_visitor_,
                            basic_json_decode_options<CharT>(),
                            default_json_parsing(),
                            alloc)
    {
    }

    template <class Source>
    basic_json_reader(Source&& source, 
                      const basic_json_decode_options<CharT>& options, 
                      const Allocator& alloc = Allocator())
        : basic_json_reader(std::forward<Source>(source),
                            default_visitor_,
                            options,
                            default_json_parsing(),
                            alloc)
    {
    }

    template <class Source>
    basic_json_reader(Source&& source,
                      std::function<bool(json_errc,const ser_context&)> err_handler, 
                      const Allocator& alloc = Allocator())
        : basic_json_reader(std::forward<Source>(source),
                            default_visitor_,
                            basic_json_decode_options<CharT>(),
                            err_handler,
                            alloc)
    {
    }

    template <class Source>
    basic_json_reader(Source&& source, 
                      const basic_json_decode_options<CharT>& options,
                      std::function<bool(json_errc,const ser_context&)> err_handler, 
                      const Allocator& alloc = Allocator())
        : basic_json_reader(std::forward<Source>(source),
                            default_visitor_,
                            options,
                            err_handler,
                            alloc)
    {
    }

    template <class Source>
    basic_json_reader(Source&& source, 
                      basic_json_visitor<CharT>& visitor, 
                      const Allocator& alloc = Allocator())
        : basic_json_reader(std::forward<Source>(source),
                            visitor,
                            basic_json_decode_options<CharT>(),
                            default_json_parsing(),
                            alloc)
    {
    }

    template <class Source>
    basic_json_reader(Source&& source, 
                      basic_json_visitor<CharT>& visitor,
                      const basic_json_decode_options<CharT>& options, 
                      const Allocator& alloc = Allocator())
        : basic_json_reader(std::forward<Source>(source),
                            visitor,
                            options,
                            default_json_parsing(),
                            alloc)
    {
    }

    template <class Source>
    basic_json_reader(Source&& source,
                      basic_json_visitor<CharT>& visitor,
                      std::function<bool(json_errc,const ser_context&)> err_handler, 
                      const Allocator& alloc = Allocator())
        : basic_json_reader(std::forward<Source>(source),
                            visitor,
                            basic_json_decode_options<CharT>(),
                            err_handler,
                            alloc)
    {
    }

    template <class Source>
    basic_json_reader(Source&& source,
                      basic_json_visitor<CharT>& visitor, 
                      const basic_json_decode_options<CharT>& options,
                      std::function<bool(json_errc,const ser_context&)> err_handler, 
                      const Allocator& alloc = Allocator(),
                      typename std::enable_if<!std::is_constructible<jsoncons::basic_string_view<CharT>,Source>::value>::type* = 0)
       : visitor_(visitor),
         parser_(options,err_handler,alloc),
         source_(std::forward<Source>(source)),
         eof_(false),
         begin_(true),
         buffer_length_(default_max_buffer_length),
         raw_buffer_length_(default_max_buffer_length),
         buffer_(alloc)
    {
        buffer_.reserve(buffer_length_);
    }

    template <class Source>
    basic_json_reader(Source&& source,
                      basic_json_visitor<CharT>& visitor, 
                      const basic_json_decode_options<CharT>& options,
                      std::function<bool(json_errc,const ser_context&)> err_handler, 
                      const Allocator& alloc = Allocator(),
                      typename std::enable_if<std::is_constructible<jsoncons::basic_string_view<CharT>,Source>::value>::type* = 0)
       : visitor_(visitor),
         parser_(options,err_handler,alloc),
         eof_(false),
         begin_(false),
         buffer_length_(0),
         raw_buffer_length_(0),
         buffer_(alloc)
    {
        jsoncons::basic_string_view<CharT> sv(std::forward<Source>(source));
        auto result = unicons::skip_bom(sv.begin(), sv.end());
        if (result.ec != unicons::encoding_errc())
        {
            JSONCONS_THROW(ser_error(result.ec,parser_.line(),parser_.column()));
        }
        std::size_t offset = result.it - sv.begin();
        parser_.update(sv.data()+offset,sv.size()-offset);
    }

    std::size_t buffer_length() const
    {
        return buffer_length_;
    }

    void buffer_length(std::size_t length)
    {
        buffer_length_ = length;
        buffer_.reserve(buffer_length_);
    }
#if !defined(JSONCONS_NO_DEPRECATED)
    JSONCONS_DEPRECATED_MSG("Instead, use max_nesting_depth() on options")
    int max_nesting_depth() const
    {
        return parser_.max_nesting_depth();
    }

    JSONCONS_DEPRECATED_MSG("Instead, use max_nesting_depth(int) on options")
    void max_nesting_depth(int depth)
    {
        parser_.max_nesting_depth(depth);
    }
#endif
    void read_next()
    {
        std::error_code ec;
        read_next(ec);
        if (ec)
        {
            JSONCONS_THROW(ser_error(ec,parser_.line(),parser_.column()));
        }
    }

    void read_next(std::error_code& ec)
    {
        if (source_.is_error())
        {
            ec = json_errc::source_error;
            return;
        }        
        parser_.reset();
        while (!parser_.finished())
        {
            if (parser_.source_exhausted())
            {
                if (!source_.eof())
                {
                    read_buffer(ec);
                    if (ec) return;
                }
                else
                {
                    eof_ = true;
                }
            }
            parser_.parse_some(visitor_, ec);
            if (ec) return;
        }
        
        while (!eof_)
        {
            parser_.skip_whitespace();
            if (parser_.source_exhausted())
            {
                if (!source_.eof())
                {
                    read_buffer(ec);
                    if (ec) return;
                }
                else
                {
                    eof_ = true;
                }
            }
            else
            {
                break;
            }
        }
    }

    void check_done()
    {
        std::error_code ec;
        check_done(ec);
        if (ec)
        {
            JSONCONS_THROW(ser_error(ec,parser_.line(),parser_.column()));
        }
    }

    std::size_t line() const
    {
        return parser_.line();
    }

    std::size_t column() const
    {
        return parser_.column();
    }

    void check_done(std::error_code& ec)
    {
        if (source_.is_error())
        {
            ec = json_errc::source_error;
            return;
        }   
        if (eof_)
        {
            parser_.check_done(ec);
            if (ec) return;
        }
        else
        {
            while (!eof_)
            {
                if (parser_.source_exhausted())
                {
                    if (!source_.eof())
                    {
                        read_buffer(ec);     
                        if (ec) return;
                    }
                    else
                    {
                        eof_ = true;
                    }
                }
                if (!eof_)
                {
                    parser_.check_done(ec);
                    if (ec) return;
                }
            }
        }
    }

    bool eof() const
    {
        return eof_;
    }

    void read()
    {
        read_next();
        check_done();
    }

    void read(std::error_code& ec)
    {
        read_next(ec);
        if (!ec)
        {
            check_done(ec);
        }
    }

#if !defined(JSONCONS_NO_DEPRECATED)

    JSONCONS_DEPRECATED_MSG("Instead, use buffer_length()")
    std::size_t buffer_capacity() const
    {
        return buffer_length_;
    }

    JSONCONS_DEPRECATED_MSG("Instead, use buffer_length(std::size_t)")
    void buffer_capacity(std::size_t length)
    {
        buffer_length_ = length;
        buffer_.reserve(buffer_length_);
    }
#endif

private:

    void read_buffer(std::error_code& ec)
    {
        buffer_.clear();
        buffer_.resize(buffer_length_);
        std::size_t count = source_.read(buffer_.data(), buffer_length_);
        buffer_.resize(static_cast<std::size_t>(count));
        if (buffer_.size() == 0)
        {
            eof_ = true;
        }
        else if (begin_)
        {
            auto result = unicons::skip_bom(buffer_.begin(), buffer_.end());
            if (result.ec != unicons::encoding_errc())
            {
                ec = result.ec;
                return;
            }
            std::size_t offset = result.it - buffer_.begin();
            parser_.update(buffer_.data()+offset,buffer_.size()-offset);
            begin_ = false;
        }
        else
        {
            parser_.update(buffer_.data(),buffer_.size());
        }
    }
};

template<class CharT,class Src=jsoncons::stream_source<char>,class Allocator=std::allocator<char>>
class basic_json_raw_reader 
{
public:
    using char_type = CharT;
    using raw_char_type = typename Src::value_type;
    using source_type = Src;
    using string_view_type = jsoncons::basic_string_view<CharT>;
    using temp_allocator_type = Allocator;
private:
    static constexpr unsigned char bom_utf8[3] = { 0xef,0xbb,0xbf };
    static constexpr unsigned char bom_utf16le[2] = { 0xff, 0xfe };
    static constexpr unsigned char bom_utf16be[2] = { 0xfe, 0xff };
    static constexpr unsigned char bom_utf32le[4] = { 0xff, 0xfe, 0, 0 };
    static constexpr unsigned char bom_utf32be[4] = { 0, 0, 0xfe, 0xff};

    typedef typename std::allocator_traits<temp_allocator_type>:: template rebind_alloc<CharT> char_allocator_type;

    static constexpr size_t default_max_buffer_length = 16384;

    basic_default_json_visitor<raw_char_type> default_visitor_;
    json_visitor_adaptor<raw_char_type,char_type> visitor_adaptor_;
    basic_json_visitor<CharT>& visitor_;

    basic_json_parser<raw_char_type,Allocator> parser_;

    source_type source_;
    bool eof_;
    std::size_t buffer_length_;
    std::size_t raw_buffer_length_;
    std::vector<CharT,char_allocator_type> buffer_;
    std::vector<raw_char_type> raw_buffer_;
    encoding_kind encoding_;

    // Noncopyable and nonmoveable
    basic_json_raw_reader(const basic_json_raw_reader&) = delete;
    basic_json_raw_reader& operator=(const basic_json_raw_reader&) = delete;

public:
    template <class Source>
    explicit basic_json_raw_reader(Source&& source, const Allocator& alloc = Allocator())
        : basic_json_raw_reader(std::forward<Source>(source),
                                default_visitor_,
                                basic_json_decode_options<CharT>(),
                                default_json_parsing(),
                                alloc)
    {
    }

    template <class Source>
    basic_json_raw_reader(Source&& source, 
                      const basic_json_decode_options<CharT>& options, 
                      const Allocator& alloc = Allocator())
        : basic_json_raw_reader(std::forward<Source>(source),
                                default_visitor_,
                                options,
                                default_json_parsing(),
                                alloc)
    {
    }

    template <class Source>
    basic_json_raw_reader(Source&& source,
                      std::function<bool(json_errc,const ser_context&)> err_handler, 
                      const Allocator& alloc = Allocator())
        : basic_json_raw_reader(std::forward<Source>(source),
                                default_visitor_,
                                basic_json_decode_options<CharT>(),
                                err_handler,
                                alloc)
    {
    }

    template <class Source>
    basic_json_raw_reader(Source&& source, 
                      const basic_json_decode_options<CharT>& options,
                      std::function<bool(json_errc,const ser_context&)> err_handler, 
                      const Allocator& alloc = Allocator())
        : basic_json_raw_reader(std::forward<Source>(source),
                                default_visitor_,
                                options,
                                err_handler,
                                alloc)
    {
    }

    template <class Source>
    basic_json_raw_reader(Source&& source, 
                      basic_json_visitor<CharT>& visitor, 
                      const Allocator& alloc = Allocator())
        : basic_json_raw_reader(std::forward<Source>(source),
                                visitor,
                                basic_json_decode_options<CharT>(),
                                default_json_parsing(),
                                alloc)
    {
    }

    template <class Source>
    basic_json_raw_reader(Source&& source, 
                      basic_json_visitor<CharT>& visitor,
                      const basic_json_decode_options<CharT>& options, 
                      const Allocator& alloc = Allocator())
        : basic_json_raw_reader(std::forward<Source>(source),
                                visitor,
                                options,
                                default_json_parsing(),
                                alloc)
    {
    }

    template <class Source>
    basic_json_raw_reader(Source&& source,
                      basic_json_visitor<CharT>& visitor,
                      std::function<bool(json_errc,const ser_context&)> err_handler, 
                      const Allocator& alloc = Allocator())
        : basic_json_raw_reader(std::forward<Source>(source),
                                visitor,
                                basic_json_decode_options<CharT>(),
                                err_handler,
                                alloc)
    {
    }

    template <class Source>
    basic_json_raw_reader(Source&& source,
                          basic_json_visitor<CharT>& visitor, 
                          const basic_json_decode_options<CharT>& options,
                          std::function<bool(json_errc,const ser_context&)> err_handler, 
                          const Allocator& alloc = Allocator(),
                          typename std::enable_if<!std::is_constructible<jsoncons::basic_string_view<CharT>,Source>::value>::type* = 0)
       : visitor_adaptor_(visitor), 
         visitor_(std::is_same<char_type,raw_char_type>::value ? visitor : visitor_adaptor_),         
         parser_(options,err_handler,alloc),
         source_(std::forward<Source>(source)),
         eof_(false),
         buffer_length_(default_max_buffer_length),
         raw_buffer_length_(default_max_buffer_length),
         buffer_(alloc),
         encoding_(encoding_kind::undetected)
    {
        buffer_.reserve(buffer_length_);
    }

    template <class Source>
    basic_json_raw_reader(Source&& source,
                          basic_json_visitor<CharT>& visitor, 
                          const basic_json_decode_options<CharT>& options,
                          std::function<bool(json_errc,const ser_context&)> err_handler, 
                          const Allocator& alloc = Allocator(),
                          typename std::enable_if<std::is_constructible<jsoncons::basic_string_view<CharT>,Source>::value>::type* = 0)
       : visitor_adaptor_(visitor), 
         visitor_(std::is_same<char_type,raw_char_type>::value ? visitor : visitor_adaptor_),         
         parser_(options,err_handler,alloc),
         eof_(false),
         buffer_length_(0),
         raw_buffer_length_(0),
         buffer_(alloc),
         encoding_(encoding_kind::undetected)
    {
        jsoncons::basic_string_view<CharT> sv(std::forward<Source>(source));

        auto result = detect_json_encoding(sv.data(), sv.size());
        if (result.encoding == encoding_kind::undetected)
        {
            JSONCONS_THROW(ser_error(json_errc::illegal_codepoint,parser_.line(),parser_.column()));
        }
        encoding_ = result.encoding;

        std::error_code ec;
        update_parser(sv.data()+result.offset, sv.size()-result.offset, ec);
    }

    std::size_t buffer_length() const
    {
        return buffer_length_;
    }

    void buffer_length(std::size_t length)
    {
        buffer_length_ = length;
        buffer_.reserve(buffer_length_);
    }
#if !defined(JSONCONS_NO_DEPRECATED)
    JSONCONS_DEPRECATED_MSG("Instead, use max_nesting_depth() on options")
    int max_nesting_depth() const
    {
        return parser_.max_nesting_depth();
    }

    JSONCONS_DEPRECATED_MSG("Instead, use max_nesting_depth(int) on options")
    void max_nesting_depth(int depth)
    {
        parser_.max_nesting_depth(depth);
    }
#endif
    void read_next()
    {
        std::error_code ec;
        read_next(ec);
        if (ec)
        {
            JSONCONS_THROW(ser_error(ec,parser_.line(),parser_.column()));
        }
    }

    void read_next(std::error_code& ec)
    {
        if (source_.is_error())
        {
            ec = json_errc::source_error;
            return;
        }        
        parser_.reset();
        while (!parser_.finished())
        {
            if (parser_.source_exhausted())
            {
                if (!source_.eof())
                {
                    read_buffer(ec);
                    if (ec) return;
                }
                else
                {
                    eof_ = true;
                }
            }
            parser_.parse_some(visitor_, ec);
            if (ec) return;
        }
        
        while (!eof_)
        {
            parser_.skip_whitespace();
            if (parser_.source_exhausted())
            {
                if (!source_.eof())
                {
                    read_buffer(ec);
                    if (ec) return;
                }
                else
                {
                    eof_ = true;
                }
            }
            else
            {
                break;
            }
        }
    }

    void check_done()
    {
        std::error_code ec;
        check_done(ec);
        if (ec)
        {
            JSONCONS_THROW(ser_error(ec,parser_.line(),parser_.column()));
        }
    }

    std::size_t line() const
    {
        return parser_.line();
    }

    std::size_t column() const
    {
        return parser_.column();
    }

    void check_done(std::error_code& ec)
    {
        if (source_.is_error())
        {
            ec = json_errc::source_error;
            return;
        }   
        if (eof_)
        {
            parser_.check_done(ec);
            if (ec) return;
        }
        else
        {
            while (!eof_)
            {
                if (parser_.source_exhausted())
                {
                    if (!source_.eof())
                    {
                        read_buffer(ec);     
                        if (ec) return;
                    }
                    else
                    {
                        eof_ = true;
                    }
                }
                if (!eof_)
                {
                    parser_.check_done(ec);
                    if (ec) return;
                }
            }
        }
    }

    bool eof() const
    {
        return eof_;
    }

    void read()
    {
        read_next();
        check_done();
    }

    void read(std::error_code& ec)
    {
        read_next(ec);
        if (!ec)
        {
            check_done(ec);
        }
    }

#if !defined(JSONCONS_NO_DEPRECATED)

    JSONCONS_DEPRECATED_MSG("Instead, use buffer_length()")
    std::size_t buffer_capacity() const
    {
        return buffer_length_;
    }

    JSONCONS_DEPRECATED_MSG("Instead, use buffer_length(std::size_t)")
    void buffer_capacity(std::size_t length)
    {
        buffer_length_ = length;
        buffer_.reserve(buffer_length_);
    }
#endif

private:

    void read_buffer(std::error_code& ec)
    {
        raw_buffer_.resize(buffer_length_);
        std::size_t count = source_.read(raw_buffer_.data(), buffer_length_);
        raw_buffer_.resize(static_cast<std::size_t>(count));
        if (raw_buffer_.size() == 0)
        {
            eof_ = true;
        }
        else if (encoding_ == encoding_kind::undetected)
        {
            auto result = detect_json_encoding(raw_buffer_.data(), buffer_length_);
            if (result.encoding == encoding_kind::undetected)
            {
                ec = json_errc::illegal_codepoint;
                return;
            }
            encoding_ = result.encoding;
            update_parser(raw_buffer_.data()+result.offset,raw_buffer_.size()-result.offset, ec);
        }
        else
        {
            update_parser(raw_buffer_.data(),raw_buffer_.size(), ec);
        }
    }

    void update_parser(const raw_char_type* data, std::size_t length, std::error_code& )
    {
        parser_.update(data,length);
    }

    static detect_encoding_result detect_json_encoding(const raw_char_type* data, std::size_t length)
    {
        encoding_kind encoding = encoding_kind::undetected;
        std::size_t offset = 0;

        if (length >= 2 && !memcmp(data, bom_utf16le, 2))
        {
            encoding = encoding_kind::utf16le;
            offset = 2;
        }
        else if (length >= 2 && !memcmp(data, bom_utf16be, 2))
        {
            encoding = encoding_kind::utf16be;
            offset = 2;
        }
        else if (length >= 4 && !memcmp(data, bom_utf32le, 4))
        {
            encoding = encoding_kind::utf32le;
            offset = 4;
        }
        else if (length >= 4 && !memcmp(data, bom_utf32be, 4))
        {
            encoding = encoding_kind::utf32be;
            offset = 4;
        }
        else if (length >= 3 && !memcmp(data, bom_utf8, 3))
        {
            encoding = encoding_kind::utf8;
            offset = 3;
        }
        else
        {
            if (length >= 2 && data[0] == 0 && data[1] == 0)
            {
                encoding = encoding_kind::utf32be;
            }
            else if (length >= 2 && data[0] == 0 && data[1] != 0)
            {
                encoding = encoding_kind::utf16be;
            }
            else if (length >= 4 && data[0] != 0 && data[1] == 0 && 
                     data[2] == 0 && data[3] == 0)
            {
                encoding = encoding_kind::utf32le;
            }
            else if (length >= 4 && data[0] != 0 && data[1] == 0 && 
                     data[2] == 0 && data[3] != 0)
            {
                encoding = encoding_kind::utf16le;
            }
            else if (length >= 3 && data[0] != 0 && data[1] == 0 && 
                     data[2] != 0)
            {
                encoding = encoding_kind::utf16le;
            }
            else if (length >= 2 && data[0] != 0 && data[1] != 0)
            {
                encoding = encoding_kind::utf8;
            }
            else if (length >= 1 && data[0] != 0)
            {
                encoding = encoding_kind::utf8;
            }
        }
        return detect_encoding_result{encoding, offset};
    }
};

using json_reader = basic_json_reader<char>;
using wjson_reader = basic_json_reader<wchar_t>;

#if !defined(JSONCONS_NO_DEPRECATED)
JSONCONS_DEPRECATED_MSG("Instead, use json_reader") typedef json_reader json_string_reader;
JSONCONS_DEPRECATED_MSG("Instead, use wjson_reader") typedef wjson_reader wjson_string_reader;
#endif

}

#endif

