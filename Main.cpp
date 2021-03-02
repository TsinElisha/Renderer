//
// Created by tsin on 2021/3/2.
//
#include "tgaimage.h"

int main(){
    TGAImage img(100, 100, TGAImage::RGB);
    TGAColor clr(255, 255, 255);
    img.set(10, 10, clr);
    //img.flip_v();
    img.flip_h();
    img.write_tga_file("out.tga");

}