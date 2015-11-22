#include "fmt/leaf/pak2_group/pak2_audio_decoder.h"
#include "log.h"

using namespace au;
using namespace au::fmt::leaf;

static const bstr magic = "\x03\x95\xAD\x4B"_b;

bool Pak2AudioDecoder::is_recognized_impl(io::File &input_file) const
{
    input_file.stream.seek(4);
    return input_file.stream.read(4) == magic;
}

std::unique_ptr<io::File> Pak2AudioDecoder::decode_impl(
    io::File &input_file) const
{
    input_file.stream.seek(12);
    const auto size_comp = input_file.stream.read_u32_le();
    input_file.stream.skip(4);
    const auto data = input_file.stream.read(size_comp);
    if (!input_file.stream.eof())
        Log.warn("Extra data after EOF.\n");
    auto output_file = std::make_unique<io::File>(input_file.name, data);
    output_file->guess_extension();
    return output_file;
}

static auto dummy = fmt::register_fmt<Pak2AudioDecoder>("leaf/pak2-audio");