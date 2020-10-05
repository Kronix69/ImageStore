#include <vector>
#include <string>
#include <sstream>
auto FS_VERSION = 0;
auto FS_FILE = 0;
auto FS_FOLDER = 1;
auto FS_SIZE = sizeof(size_t);

class fs_entry {
public:
	uint8_t type;
	std::string name;
	std::string data;
	std::vector<fs_entry> children;
	fs_entry(uint8_t type, std::string&& name, std::string&& data) : type(type), name(name), data(data) {};
};

class embeddedfs {
private:
	const char* raw;
	size_t read_location = 0;

	void compile_filearray(std::stringstream& stream, const std::vector<fs_entry>& file_vector) {
		stream << file_vector.size();
		for (const auto& file : file_vector) {
			stream << file.type << file.name.size() << file.name << file.data.size() << file.data;
			compile_filearray(stream, file.children);
		}
	};

	void readtable(std::vector<fs_entry>& fs) {
		for (size_t i = 0; i < read<size_t>(); i++) {
			uint8_t type = read<uint8_t>();
			std::string name = std::string(raw + read_location + sizeof(size_t), read<size_t>());
			read_location += name.size();
			std::string data = std::string(raw + read_location + sizeof(size_t), read<size_t>());
			read_location += data.size();
			fs_entry f(type, std::move(name), std::move(data));
			readtable(f.children);
			fs.push_back(f);
		}
	}
	template<typename t>
	t read() {
		reader_loc += sizeof(t);
		return *reinterpret_cast<t*>(fs + (reader_loc - sizeof(t)))
	};
public:
	std::vector<fs_entry> files;
	embeddedfs(const char* data = nullptr) : raw(data) {};
	void read() {
		read_location = 0;
		if (false) std::runtime_error("Incorrect Header");
		if (read<uint16_t>() != FS_VERSION) std::runtime_error("Incorrect EFS Version");
		if (read<uint8_t>() != sizeof(size_t)) std::runtime_error("size_t mismatch");
		readtable(files);
	}
	std::string compile() {
		std::stringstream FS;
		FS << "\01EFS" << FS_VERSION << FS_SIZE;
		compile_filearray(FS, files);
		return FS.str();
	}
};