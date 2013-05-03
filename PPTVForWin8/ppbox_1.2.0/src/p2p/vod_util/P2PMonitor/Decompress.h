class Decompressor
{
private:
    int max_raw_data_size_;

    int raw_data_size_;
    char* raw_data_buffer_;

public:
    Decompressor(int max_raw_data_size)
        :max_raw_data_size_(max_raw_data_size), raw_data_size_(0), raw_data_buffer_(0)
    {}

    ~Decompressor()
    {
        delete[] raw_data_buffer_;
    }

    bool Decompress(const string path);

    int GetRawDataSize() const { return raw_data_size_; }

    const char* GetRawData() const { return raw_data_buffer_; }

private:

    bool DoDecompress(char* buffer, boost::uint32_t compress_data_size, boost::uint32_t expected_crc);

    static boost::uint8_t ReadByte(std::istream& input)
    {
        char character(0);
        input.read(&character, sizeof(character));

        return static_cast<boost::uint8_t>(character);
    }

    static boost::uint32_t ReadValue (std::istream& input)
    {
        boost::uint32_t x = ReadByte(input);

        x += ReadByte(input)<<8;
        x += ReadByte(input)<<16;
        x += ReadByte(input)<<24;
        return x;
    }
};