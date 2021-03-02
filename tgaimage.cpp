//
// Created by tsin on 2021/3/2.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include "tgaimage.h"

TGAImage::TGAImage(): data(), width(0), height(0), bytespp(0) {}
TGAImage::TGAImage(const int w, const int h, const int bpp) : data(w*h*bpp, 0), width(w),height(h), bytespp(bpp){}

bool TGAImage::read_tga_file(const std::string filename) {
    std::ifstream in;
    in.open(filename, std::ios::binary);
    if(!in.is_open()){
        std::cerr<< "can't open file " << filename << "\n";
        in.close();
        return false;
    }

    TGAHeader header;
    in.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (!in.good()){
        in.close();
        std::cerr << "an error occurred while reading the headern\n";
        return false;
    }
    width = header.width;
    height = header.height;
    bytespp = header.bitsperpixel >> 3; // bitsperpixel / 8

    if (width <= 0 || height <= 0 || (bytespp!=GRAYSCALE && bytespp!=RGB && bytespp!=RGBA)){
        in.close();
        std::cerr << "bad bpp (or width/height) value" << std::endl;
        return false;
    }
    size_t nbytes = width * height * bytespp;
    data = std::vector<std::uint8_t>(nbytes, 0);

    if (3 == header.datatypecode || 2 == header.datatypecode){
        in.read(reinterpret_cast<char *>(data.data()), nbytes);
        if (!in.good()){
            in.close();
            std::cerr << "an error occurred while reading data \n";
            return false;
        }
    } else {
        in.close();
        std::cerr <<"unknown file format " << (int) header.datatypecode << "\n";
        return false;
    }

    if (!(header.imagedescriptor & 0x20))
        flip_v();
    if (!(header.imagedescriptor & 0x10))
        flip_h();

    std::cout << width << "" << height << "/" << bytespp*8 <<"\n";
    in.close();
    return true;
}

bool TGAImage::write_tga_file(const std::string filename, const bool vflip) const {
    std::uint8_t developer_area_ref[4] = {0, 0, 0, 0};
    std::uint8_t extension_area_ref[4] = {0, 0, 0, 0};
    std::uint8_t footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-',
                               'X', 'F', 'I', 'L', 'E', '.', '\0'};

    std::ofstream out;
    out.open(filename, std::ios::binary);

    if (!out.is_open()){
        out.close();
        std::cerr <<" can't open file" << std::endl;
        return false;
    }
    TGAHeader header;
    header.bitsperpixel = bytespp << 3;
    header.width = width;
    header.height = height;
    header.datatypecode = (bytespp==GRAYSCALE?3:2);
    header.imagedescriptor = vflip ? 0x00 : 0x20; // top-left or bottom-left origin
    out.write(reinterpret_cast<const char *>(&header), sizeof(header));
    if (!out.good()){
        out.close();
        std::cerr<< "can't dump the tga file\n";
        return false;
    }
    out.write(reinterpret_cast<const char *>(data.data()), width*height*bytespp);
    if (!out.good()){
        out.close();
        std::cerr<< "can't write raw data\n";
    }
    out.write(reinterpret_cast<const char *>(developer_area_ref), sizeof(developer_area_ref));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write(reinterpret_cast<const char *>(extension_area_ref), sizeof(extension_area_ref));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write(reinterpret_cast<const char *>(footer), sizeof(footer));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.close();
    return true;
}

TGAColor TGAImage::get(const int x, const int y) const {
    if (!data.size() || x < 0 || y < 0 || x >= width || y >= height)
        return {};
    return TGAColor(data.data()+(x+y+width)*bytespp, bytespp);
}

void TGAImage::set(const int x, const int y, const TGAColor &c) {
    if (!data.size() || x<0 || y<0 || x>=width || y>=height) return;
    memcpy(data.data()+(x+y*width)*bytespp, c.bgra, bytespp);
}

int TGAImage::get_w() const {
    return width;
}

int TGAImage::get_h() const {
    return height;
}


void TGAImage::flip_h() {
    if (!data.size()) return;
    int half = width>>1;
    for (int i=0; i<half; i++) {
        for (int j=0; j<height; j++) {
            TGAColor c1 = get(i, j);
            TGAColor c2 = get(width-1-i, j);
            set(i, j, c2);
            set(width-1-i, j, c1);
        }
    }
}
void TGAImage::flip_v() {
    if (!data.size()) return;
    size_t bytes_per_line = width*bytespp;
    std::vector<std::uint8_t> line(bytes_per_line, 0);
    int half = height>>1;
    for (int j=0; j<half; j++) {
        size_t l1 = j*bytes_per_line;
        size_t l2 = (height-1-j)*bytes_per_line;
        std::copy(data.begin()+l1, data.begin()+l1+bytes_per_line, line.begin());
        std::copy(data.begin()+l2, data.begin()+l2+bytes_per_line, data.begin()+l1);
        std::copy(line.begin(), line.end(), data.begin()+l2);
    }
}

std::uint8_t * TGAImage::buffer() {
    return data.data();
}

void TGAImage::scale(const int w, const int h) {

}
void TGAImage::clear() {

}





























