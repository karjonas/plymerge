#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::string join(const std::vector<std::string>& strings,
                 const std::string& joinWith)
{
    const size_t numStrings = strings.size();
    if (numStrings == 0)
        return "";

    std::stringstream ss;
    ss << strings[0];
    for (size_t i = 1; i < numStrings; i++)
        ss << joinWith << strings[i];
    return ss.str();
}

std::vector<std::string> split(const std::string& s)
{
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    return vstrings;
}

void merge_plys(std::vector<std::string> files, std::string& vertices_tot,
                std::string& faces_tot, size_t& num_vertices_tot,
                size_t& num_faces_tot)
{
    size_t ctr = 1;
    for (auto filename : files)
    {
        std::ifstream infile(filename);

        const size_t vertices_offset = num_vertices_tot;

        bool header_done = false;

        std::string line;
        while (std::getline(infile, line))
        {
            if (!header_done)
            {
                header_done = line == "end_header";
                continue;
            }

            const auto num_spaces = std::count(line.begin(), line.end(), ' ');

            // Vertex
            if (num_spaces == 5)
            {
                num_vertices_tot += 1;
                vertices_tot += line;
                vertices_tot += '\n';
                continue;
            }
            assert(num_spaces == 3 || num_spaces == 4);

            // Face
            const auto nums_str = split(line);
            std::vector<std::string> adjusted_nums;
            adjusted_nums.reserve(4);
            adjusted_nums.push_back(nums_str[0]);
            for (size_t i = 1; i < num_spaces + 1; ++i)
            {
                const auto v = std::stoul(nums_str[i]);
                adjusted_nums.push_back(std::to_string(v + vertices_offset));
            }
            const std::string face_adj = join(adjusted_nums, " ");
            faces_tot += face_adj;
            faces_tot += '\n';
            num_faces_tot += 1;
        }

        std::cout << ctr++ << "/" << files.size() << std::endl;
    }
}

std::vector<std::string> read_filelist(const std::string input)
{
    std::vector<std::string> output;
    std::ifstream infile(input);

    std::string line;
    while (std::getline(infile, line))
        output.push_back(line);
    return output;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./plymerge inputfile output.ply" << std::endl;
        return 1;
    }

    std::string input = argv[1];
    std::string output = argv[2];
    std::cout << "Reading file list from " << input << std::endl;
    std::vector<std::string> infiles = read_filelist(input);

    std::cout << "Merging " << infiles.size() << " files" << std::endl;

    std::string vertices_tot;
    std::string faces_tot;
    size_t num_vertices_tot = 0;
    size_t num_faces_tot = 0;

    merge_plys(infiles, vertices_tot, faces_tot, num_vertices_tot,
               num_faces_tot);

    std::cout << "Saving merged ply to " << output << "\n";
    std::ofstream out(output);

    out << "ply\n";
    out << "format ascii 1.0\n";
    out << "comment merged with plymerge\n";
    out << "element vertex " + std::to_string(num_vertices_tot) + "\n";
    out << "property float x\n";
    out << "property float y\n";
    out << "property float z\n";
    out << "property float nx\n";
    out << "property float ny\n";
    out << "property float nz\n";
    out << "element face " + std::to_string(num_faces_tot) + "\n";
    out << "property list uchar uint vertex_indices\n";
    out << "end_header\n";
    out << vertices_tot;
    vertices_tot.clear();
    out << faces_tot;
    faces_tot.clear();

    out.close();

    return 0;
}
