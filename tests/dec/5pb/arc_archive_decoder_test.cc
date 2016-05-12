#include "dec/5pb/arc_archive_decoder.h"
#include "algo/locale.h"
#include "io/memory_stream.h"
#include "test_support/catch.h"
#include "test_support/decoder_support.h"
#include "test_support/file_support.h"

using namespace au;
using namespace au::dec::_5pb;

static std::unique_ptr<io::File> get_arc_file(
    const std::vector<std::shared_ptr<io::File>> &expected_files)
{
    size_t current_offset = 0;
    io::MemoryStream table_stream;
    for (const auto &file : expected_files)
    {
        table_stream.write_le<u32>(file->stream.size());
        table_stream.write_le<u32>(current_offset);
        table_stream.write(algo::utf8_to_utf16(file->path.str()));
        table_stream.write("\x00\x00"_b);
        current_offset += file->stream.size();
    }
    auto output_file = std::make_unique<io::File>("test.arc", ""_b);
    output_file->stream.write_le<u32>(expected_files.size());
    output_file->stream.write_le<u32>(table_stream.size());
    output_file->stream.write(table_stream.seek(0));
    for (const auto &file : expected_files)
        output_file->stream.write(file->stream.seek(0));
    return output_file;
}

TEST_CASE("5pb ARC archives", "[dec]")
{
    const std::vector<std::shared_ptr<io::File>> expected_files =
    {
        tests::stub_file("123.txt", "1234567890"_b),
        tests::stub_file("abc.txt", "abcdefghijklmnopqrstuvwxyz"_b),
    };

    auto input_file = get_arc_file(expected_files);
    const auto decoder = ArcArchiveDecoder();
    const auto actual_files = tests::unpack(decoder, *input_file);
    tests::compare_files(actual_files, expected_files, true);
}
