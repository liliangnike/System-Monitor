#ifndef __SNAPSHOT_WRITER_HEADER_FILE__
#define __SNAPSHOT_WRITER_HEADER_FILE__

#include <string>
#include <fstream>

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
    SnapshotWriter(const SnapshotWriter&&)            = default;
    SnapshotWriter& operator=(const SnapshotWriter&&) = default;

private:
    std::ofstream file_;
    std::size_t row_ = 0;
    std::string filepath_;
};

#endif
