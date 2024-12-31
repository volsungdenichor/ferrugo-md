#pragma once

#include <cstdint>
#include <ferrugo/md_v3/array.hpp>
#include <fstream>

namespace ferrugo
{
namespace md_v3
{

namespace detail
{

template <class T, class U = T>
void write(std::ostream& os, const U& value)
{
    os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <class T>
T read(std::istream& is)
{
    T value;
    is.read(reinterpret_cast<char*>(&value), sizeof(T));
    return value;
}

inline void write_n(std::ostream& os, std::size_t count, std::uint8_t value = 0)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        write<std::uint8_t>(os, value);
    }
}

inline auto get_padding(std::size_t width, std::size_t bits_per_pixel) -> std::size_t
{
    return ((bits_per_pixel * width + 31) / 32) * 4 - (width * bits_per_pixel / 8);
}

struct bmp_header
{
    static const inline std::size_t size = 14;

    std::size_t file_size;
    std::size_t data_offset;

    bmp_header() : file_size(0), data_offset(0)
    {
    }

    void save(std::ostream& os) const
    {
        write<std::uint8_t>(os, 'B');
        write<std::uint8_t>(os, 'M');
        write<std::uint32_t>(os, file_size);
        write<std::uint16_t>(os, 0); /* reserved1 */
        write<std::uint16_t>(os, 0); /* reserved2 */
        write<std::uint32_t>(os, data_offset);
    }

    static auto load(std::istream& is) -> bmp_header
    {
        bmp_header result = {};
        read<std::uint8_t>(is);                       /* B */
        read<std::uint8_t>(is);                       /* M */
        result.file_size = read<std::uint32_t>(is);   /**/
        read<std::uint16_t>(is);                      /* reserved1 */
        read<std::uint16_t>(is);                      /* reserved2 */
        result.data_offset = read<std::uint32_t>(is); /* data_offset */
        return result;
    }
};

struct dib_header
{
    static const inline std::size_t size = 40;

    dib_header()
        : width(0)
        , height(0)
        , color_plane_count(0)
        , bits_per_pixel(0)
        , compression(0)
        , data_size(0)
        , horizontal_pixel_per_meter(0)
        , vertical_pixel_per_meter(0)
        , color_count(0)
        , important_color_count(0)
    {
    }

    std::size_t width;
    std::size_t height;
    std::size_t color_plane_count;
    std::size_t bits_per_pixel;
    std::size_t compression;
    std::size_t data_size;
    std::size_t horizontal_pixel_per_meter;
    std::size_t vertical_pixel_per_meter;
    std::size_t color_count;
    std::size_t important_color_count;

    void save(std::ostream& os) const
    {
        write<std::uint32_t>(os, size);
        write<std::uint32_t>(os, width);
        write<std::uint32_t>(os, height);
        write<std::uint16_t>(os, color_plane_count);
        write<std::uint16_t>(os, bits_per_pixel);
        write<std::uint32_t>(os, compression);
        write<std::uint32_t>(os, data_size);
        write<std::uint32_t>(os, horizontal_pixel_per_meter);
        write<std::uint32_t>(os, vertical_pixel_per_meter);
        write<std::uint32_t>(os, color_count);
        write<std::uint32_t>(os, important_color_count);
    }

    static auto load(std::istream& is) -> dib_header
    {
        dib_header result = {};
        read<std::uint32_t>(is); /* size */
        result.width = read<std::uint32_t>(is);
        result.height = read<std::uint32_t>(is);
        result.color_plane_count = read<std::uint16_t>(is);
        result.bits_per_pixel = read<std::uint16_t>(is);
        result.compression = read<std::uint32_t>(is);
        result.data_size = read<std::uint32_t>(is);
        result.horizontal_pixel_per_meter = read<std::uint32_t>(is);
        result.vertical_pixel_per_meter = read<std::uint32_t>(is);
        result.color_count = read<std::uint32_t>(is);
        result.important_color_count = read<std::uint32_t>(is);
        return result;
    }
};

inline void save_header(
    std::ostream& os,  //
    std::size_t width,
    std::size_t height,
    std::size_t padding,
    std::size_t bits_per_pixel,
    std::size_t palette_size)
{
    const std::size_t data_size = (width + padding) * height * (bits_per_pixel / 8);
    const std::size_t data_offset = bmp_header::size + dib_header::size + palette_size;
    const std::size_t file_size = data_offset + data_size;

    bmp_header bmp_hdr = {};
    bmp_hdr.file_size = file_size;
    bmp_hdr.data_offset = data_offset;

    dib_header dib_hdr = {};
    dib_hdr.width = width;
    dib_hdr.height = height;
    dib_hdr.color_plane_count = 1;
    dib_hdr.bits_per_pixel = bits_per_pixel;
    dib_hdr.compression = 0;
    dib_hdr.data_size = data_size;

    bmp_hdr.save(os);
    dib_hdr.save(os);
}

inline auto prepare_array(const dib_header& header) -> array<byte, 3>
{
    const auto shape = dim_t<3>{ dim_t<>{ header.height, 3 * header.width },  //
                                 dim_t<>{ header.width, 3 },                  //
                                 dim_t<>{ 3, 1 } };

    return array<byte, 3>{ shape, std::vector<byte>(volume(shape)) };
}

inline auto load_bitmap_8(std::istream& is, const dib_header& header) -> array<byte, 3>
{
    const auto padding = get_padding(header.width, header.bits_per_pixel);

    array<byte, 3> result = prepare_array(header);
    auto ref = result.mut_ref();

    std::array<std::array<byte, 3>, 256> palette;

    for (std::size_t i = 0; i < 256; ++i)
    {
        const auto b = read<byte>(is);
        const auto g = read<byte>(is);
        const auto r = read<byte>(is);
        is.ignore(1);

        palette[i] = { r, g, b };
    }

    const auto h = result.shape().at(0).size;
    const auto w = result.shape().at(1).size;

    for (int y = h - 1; y >= 0; --y)
    {
        for (int x = 0; x < w; ++x)
        {
            const auto color = read<byte>(is);
            const auto [r, g, b] = palette.at(color);
            ref[location_t<3>{ y, x, 0 }] = r;
            ref[location_t<3>{ y, x, 1 }] = r;
            ref[location_t<3>{ y, x, 2 }] = r;
        }

        is.ignore(padding);
    }

    return result;
}

inline auto load_bitmap_24(std::istream& is, const dib_header& header) -> array<byte, 3>
{
    const auto padding = get_padding(header.width, header.bits_per_pixel);

    array<byte, 3> result = prepare_array(header);
    auto ref = result.mut_ref();

    const auto h = result.shape().at(0).size;
    const auto w = result.shape().at(1).size;

    for (int y = h - 1; y >= 0; --y)
    {
        for (int x = 0; x < w; ++x)
        {
            const auto b = read<byte>(is);
            const auto g = read<byte>(is);
            const auto r = read<byte>(is);
            ref[location_t<3>{ y, x, 0 }] = r;
            ref[location_t<3>{ y, x, 1 }] = g;
            ref[location_t<3>{ y, x, 2 }] = b;
        }

        is.ignore(padding);
    }
    return result;
}

struct load_bitmap_fn
{
    auto operator()(std::istream& is) const -> array<byte, 3>
    {
        if (!is)
        {
            throw std::runtime_error{ "load_bitmap: invalid stream" };
        }

        const detail::bmp_header bmp_hdr = detail::bmp_header::load(is);
        const detail::dib_header dib_hdr = detail::dib_header::load(is);

        switch (dib_hdr.bits_per_pixel)
        {
            case 8: return load_bitmap_8(is, dib_hdr);
            case 24: return load_bitmap_24(is, dib_hdr);
            default: throw std::runtime_error{ "load_bitmap: format not supported" };
        }
    }

    auto operator()(const std::string& path) const -> array<byte, 3>
    {
        std::ifstream fs(path.c_str(), std::ifstream::binary);
        if (!fs)
        {
            throw std::runtime_error{ "load_bitmap: can not load file '" + path + "'" };
        }
        return (*this)(fs);
    }
};

struct save_bitmap_fn
{
    void operator()(array_ref<const byte, 2> image, std::ostream& os) const
    {
        static const size_t bits_per_pixel = 8;
        size_t padding = detail::get_padding(image.shape().at(1).size, bits_per_pixel);

        const auto h = image.shape().at(0).size;
        const auto w = image.shape().at(1).size;

        save_header(os, w, h, padding, bits_per_pixel, 256 * 4);

        for (std::size_t i = 0; i < 256; ++i)
        {
            for (std::size_t j = 0; j < 3; ++j)
            {
                detail::write<std::uint8_t>(os, i);
            }
            detail::write<std::uint8_t>(os, 0);
        }

        for (int y = h - 1; y >= 0; --y)
        {
            for (int x = 0; x < w; ++x)
            {
                const auto value = image[location_t<2>{ y, x }];
                write<byte>(os, value);
            }

            write_n(os, padding);
        }
    }

    void operator()(array_ref<const byte, 3> image, std::ostream& os) const
    {
        static const size_t bits_per_pixel = 24;
        const size_t padding = detail::get_padding(image.shape().at(1).size, bits_per_pixel);

        const auto h = image.shape().at(0).size;
        const auto w = image.shape().at(1).size;

        save_header(os, w, h, padding, bits_per_pixel, 0);

        for (int y = h - 1; y >= 0; --y)
        {
            for (int x = 0; x < w; ++x)
            {
                write<byte>(os, image[location_t<3>{ y, x, 2 }]);
                write<byte>(os, image[location_t<3>{ y, x, 1 }]);
                write<byte>(os, image[location_t<3>{ y, x, 0 }]);
            }

            write_n(os, padding);
        }
    }

    void operator()(array_ref<const byte, 2> image, const std::string& path) const
    {
        std::ofstream fs(path.c_str(), std::ofstream::binary);
        (*this)(image, fs);
    }

    void operator()(array_ref<const byte, 3> image, const std::string& path) const
    {
        std::ofstream fs(path.c_str(), std::ofstream::binary);
        (*this)(image, fs);
    }
};

}  // namespace detail

static constexpr inline auto load_bitmap = detail::load_bitmap_fn{};
static constexpr inline auto save_bitmap = detail::save_bitmap_fn{};

}  // namespace md_v3
}  // namespace ferrugo
