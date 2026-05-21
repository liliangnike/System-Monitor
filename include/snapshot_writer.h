#ifndef __SNAPSHOT_WRITER_HEADER_FILE__
#define __SNAPSHOT_WRITER_HEADER_FILE__

#include <string>
#include <fstream>

#include "process_info.h"
#include "ring_buffer.h"

class SnapshotWriter
{
public:
    // using explicit to avoid implicit conversion, e.g., SnapshotWriter path = "/tmp/file.csv". SnapshotWriter path("/tmp/file.csv") is must because the constructor is explicit
    // By default, C++ agrees to convert the data to object implicitly
    explicit SnapshotWriter(const std::string& filepath);
    ~SnapshotWriter();

    // Copy construtor or copy assignment is not allowed for file descriptor
    SnapshotWriter(const SnapshotWriter&)            = delete;
    SnapshotWriter& operator=(const SnapshotWriter&) = delete;

    // move is allowed
    SnapshotWriter(SnapshotWriter&&)            = default;
    SnapshotWriter& operator=(SnapshotWriter&&) = default;

    void write(const process_info_t& proc);

    // template function
    // When there is code calling funtion with int type, compiler generates a code for int type;
    // When there is function witn double type, compiler generates a code for double type.....
    //
    // 1. N is generic placeholder
    // 2. Template function MUST be implemented in header file!!
    template <std::size_t N>
    void flush_history(const RingBuffer<process_info_t, N>& buf)
    {
        // lambda function object
        buf.for_each( [this](const process_info_t& p){ write(p); } );
    }

    bool is_open() const { return file_.is_open(); }
    std::size_t written_rows() const { return row_; }

private:
    void write_header();

    std::ofstream file_;
    std::size_t row_ = 0;
    std::string filepath_;
};

#endif
